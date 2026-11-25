/**
 * @file audio.c
 * @brief Implementação do carregamento, reprodução e liberação dos recursos sonoros.
 */
#include "../../include/audio.h"

#include <stdio.h>
#include <string.h>

#define DIRETORIO_AUDIO "assets/audio/"
#define ARQUIVO_AMBIENTE "themesong.wav"

// Mantém os caminhos dos efeitos na mesma ordem definida em EfeitoAudio.
static const char *ARQUIVOS_EFEITOS[QUANT_EFEITOS_AUDIO] = {
    "hover.mp3",
    "selecao.wav",
    "transition.mp3",
    "correct.mp3",
    "resposta_incorreta.wav",
    "marco_seguranca.wav",
    "musicaganhador.mp3"
};

// Limita o volume para evitar valores fora do intervalo aceito pela Raylib.
static float limitarVolume(float volume) {
    if (volume < 0.0f) {
        return 0.0f;
    }
    if (volume > 1.0f) {
        return 1.0f;
    }
    return volume;
}

// Monta o caminho de um recurso e verifica se o arquivo está disponível.
static bool montarCaminhoAudio(
    char *destino,
    size_t capacidade,
    const char *arquivo
) {
    if (destino == NULL || capacidade == 0 || arquivo == NULL) {
        return false;
    }

    int tamanho = snprintf(
        destino,
        capacidade,
        "%s%s",
        DIRETORIO_AUDIO,
        arquivo
    );

    if (tamanho < 0 || (size_t)tamanho >= capacidade) {
        destino[0] = '\0';
        return false;
    }

    if (!FileExists(destino)) {
        TraceLog(LOG_WARNING, "Arquivo de audio nao encontrado: %s", destino);
        return false;
    }

    return true;
}

bool inicializarAudioJogo(AudioJogo *audio) {
    if (audio == NULL) {
        return false;
    }

    // Inicializa a estrutura para permitir uma liberação segura mesmo após falhas parciais.
    memset(audio, 0, sizeof(*audio));
    audio->volumeAmbiente = 0.22f;
    audio->volumeEfeitos = 0.70f;

    // Reutiliza o dispositivo existente para evitar inicializações duplicadas.
    if (!IsAudioDeviceReady()) {
        InitAudioDevice();
        audio->dispositivoInicializadoPeloModulo = IsAudioDeviceReady();
    }

    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_ERROR, "Nao foi possivel inicializar o dispositivo de audio.");
        return false;
    }

    // Carrega a ambiência espacial como fluxo contínuo para reduzir o uso de memória.
    char caminho[256];
    if (montarCaminhoAudio(caminho, sizeof(caminho), ARQUIVO_AMBIENTE)) {
        audio->ambiente = LoadMusicStream(caminho);
        audio->ambienteCarregado = audio->ambiente.frameCount > 0;

        if (audio->ambienteCarregado) {
            audio->ambiente.looping = true;
            SetMusicVolume(audio->ambiente, audio->volumeAmbiente);
        } else {
            TraceLog(LOG_WARNING, "Nao foi possivel carregar o ambiente espacial.");
        }
    }

    // Carrega individualmente os efeitos para permitir reprodução por evento.
    for (int i = 0; i < QUANT_EFEITOS_AUDIO; i++) {
        if (!montarCaminhoAudio(caminho, sizeof(caminho), ARQUIVOS_EFEITOS[i])) {
            continue;
        }

        audio->efeitos[i] = LoadSound(caminho);
        audio->efeitosCarregados[i] = audio->efeitos[i].frameCount > 0;

        if (audio->efeitosCarregados[i]) {
            SetSoundVolume(audio->efeitos[i], audio->volumeEfeitos);
        } else {
            TraceLog(LOG_WARNING, "Nao foi possivel carregar o efeito: %s", caminho);
        }
    }

    // A inicialização é considerada válida quando o dispositivo está disponível.
    return true;
}

void atualizarAudioJogo(AudioJogo *audio) {
    if (audio == NULL || !audio->ambienteCarregado || !IsAudioDeviceReady()) {
        return;
    }

    // Mantém o fluxo da música atualizado durante o loop principal da aplicação.
    UpdateMusicStream(audio->ambiente);
}

void iniciarAmbienteAudio(AudioJogo *audio) {
    if (audio == NULL || !audio->ambienteCarregado || !IsAudioDeviceReady()) {
        return;
    }

    // Inicia a ambiência espacial em reprodução contínua.
    PlayMusicStream(audio->ambiente);
}

void pausarAmbienteAudio(AudioJogo *audio) {
    if (audio == NULL || !audio->ambienteCarregado || !IsAudioDeviceReady()) {
        return;
    }

    // Pausa o ambiente sem descarregar o fluxo de áudio.
    PauseMusicStream(audio->ambiente);
}

void retomarAmbienteAudio(AudioJogo *audio) {
    if (audio == NULL || !audio->ambienteCarregado || !IsAudioDeviceReady()) {
        return;
    }

    // Retoma o ambiente a partir da posição em que foi pausado.
    ResumeMusicStream(audio->ambiente);
}

void reproduzirEfeitoAudio(AudioJogo *audio, EfeitoAudio efeito) {
    if (
        audio == NULL ||
        !IsAudioDeviceReady() ||
        efeito < EFEITO_HOVER ||
        efeito >= QUANT_EFEITOS_AUDIO
    ) {
        return;
    }

    // Reproduz somente efeitos carregados e associados a uma ação válida.
    if (audio->efeitosCarregados[efeito]) {
        PlaySound(audio->efeitos[efeito]);
    }
}

void definirVolumeAmbienteAudio(AudioJogo *audio, float volume) {
    if (audio == NULL) {
        return;
    }

    audio->volumeAmbiente = limitarVolume(volume);

    // Atualiza o volume imediatamente quando a música já estiver carregada.
    if (audio->ambienteCarregado && IsAudioDeviceReady()) {
        SetMusicVolume(audio->ambiente, audio->volumeAmbiente);
    }
}

void definirVolumeEfeitosAudio(AudioJogo *audio, float volume) {
    if (audio == NULL) {
        return;
    }

    audio->volumeEfeitos = limitarVolume(volume);

    // Aplica o novo volume a todos os efeitos disponíveis.
    if (!IsAudioDeviceReady()) {
        return;
    }

    for (int i = 0; i < QUANT_EFEITOS_AUDIO; i++) {
        if (audio->efeitosCarregados[i]) {
            SetSoundVolume(audio->efeitos[i], audio->volumeEfeitos);
        }
    }
}

void liberarAudioJogo(AudioJogo *audio) {
    if (audio == NULL) {
        return;
    }

    // Descarrega a música somente quando o fluxo tiver sido carregado.
    if (audio->ambienteCarregado && IsAudioDeviceReady()) {
        StopMusicStream(audio->ambiente);
        UnloadMusicStream(audio->ambiente);
        audio->ambienteCarregado = false;
    }

    // Libera individualmente os efeitos sonoros carregados.
    if (IsAudioDeviceReady()) {
        for (int i = 0; i < QUANT_EFEITOS_AUDIO; i++) {
            if (audio->efeitosCarregados[i]) {
                UnloadSound(audio->efeitos[i]);
                audio->efeitosCarregados[i] = false;
            }
        }
    }

    // Fecha o dispositivo apenas quando sua inicialização foi responsabilidade deste módulo.
    if (audio->dispositivoInicializadoPeloModulo && IsAudioDeviceReady()) {
        CloseAudioDevice();
    }

    memset(audio, 0, sizeof(*audio));
}
