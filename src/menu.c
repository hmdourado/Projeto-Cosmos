
/**
 * @file menu.c
 * @author Arthur Morais Palitot Fernandes
 * @brief Implementação do menu principal e gerenciamento dos recursos visuais e sonoros.
 * @version 0.1
 * @date 23-11-2026
 *
 * @copyright Copyright (c) 2026
 */
#include "../include/raylib.h"
#include "../include/video.h"
#include "../include/menu.h"
#include "../include/audio.h"

#include <stdbool.h>

// Executa o menu principal e gerencia os recursos utilizados pela aplicação.
void executarMenu(Cadastro *cadastro){

    // Evita avisos enquanto o cadastro não for utilizado diretamente pelo menu.
    (void)cadastro;

    // Configura a janela para ocupar a tela sem apresentar suas bordas.
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_FULLSCREEN_MODE);

    // Inicializa a janela principal da aplicação.
    InitWindow(1280, 720, "Cosmos");

    // Define a taxa de atualização da interface.
    SetTargetFPS(60);

    // Inicializa os recursos de áudio após a criação da janela.
    AudioJogo audio = {0};

    bool audioDisponivel = inicializarAudioJogo(&audio);

    // Inicia a ambiência espacial quando o dispositivo de áudio estiver disponível.
    if (audioDisponivel){

        definirVolumeAmbienteAudio(&audio, 0.22f);
        definirVolumeEfeitosAudio(&audio, 0.70f);

        iniciarAmbienteAudio(&audio);

    } else {

        TraceLog(LOG_WARNING, "Sistema de audio indisponivel.");
    }

    // Utiliza a fonte padrão como alternativa caso a fonte personalizada não esteja disponível.
    Font fonteCosmos = GetFontDefault();
    bool fonteCarregada = false;

    // Verifica a existência do arquivo antes de tentar carregar a fonte personalizada.
    if (FileExists("assets/fonts/spacerangerlaser.ttf")){

        Font fonteTemporaria = LoadFontEx(
            "assets/fonts/spacerangerlaser.ttf",
            128,
            NULL,
            0
        );

        // Substitui a fonte padrão somente quando o carregamento for válido.
        if (IsFontValid(fonteTemporaria)){

            fonteCosmos = fonteTemporaria;
            fonteCarregada = true;

            SetTextureFilter(
                fonteCosmos.texture,
                TEXTURE_FILTER_BILINEAR
            );

        } else {

            TraceLog(LOG_WARNING, "Nao foi possivel carregar a fonte personalizada.");
        }

    } else {

        TraceLog(LOG_WARNING, "Arquivo da fonte personalizada nao encontrado.");
    }

    // Inicializa a estrutura responsável pela reprodução do vídeo espacial.
    VideoBackground fundo = {0};

    // Interrompe a inicialização do menu quando o vídeo não puder ser carregado.
    if (!VideoInit(&fundo, "assets/14014441_1280_720_30fps.mp4")){

        TraceLog(LOG_ERROR, "Erro ao inicializar video!");

        // Libera a fonte personalizada quando ela tiver sido carregada.
        if (fonteCarregada){
            UnloadFont(fonteCosmos);
        }

        // Libera os recursos sonoros antes de fechar a janela.
        liberarAudioJogo(&audio);

        CloseWindow();

        return;
    }

    TraceLog(LOG_INFO, "Video inicializado com sucesso!");

    // Mantém o menu ativo enquanto a janela não receber uma solicitação de encerramento.
    while (!WindowShouldClose()){

        // Atualiza o vídeo e mantém a reprodução do áudio ambiente.
        VideoUpdate(&fundo);

        if (audioDisponivel){
            atualizarAudioJogo(&audio);
        }

        // Obtém as dimensões atuais para posicionar os elementos da interface.
        float largura = (float)GetScreenWidth();
        float altura = (float)GetScreenHeight();

        // Inicia a etapa de renderização da interface.
        BeginDrawing();

            ClearBackground(BLACK);

            // Apresenta o vídeo espacial como plano de fundo.
            VideoDraw(&fundo);

            // Aplica uma camada escura para melhorar a leitura dos elementos.
            DrawRectangle(
                0,
                0,
                (int)largura,
                (int)altura,
                (Color){0, 5, 18, 65}
            );

            // Calcula a posição horizontal para centralizar o título.
            Vector2 tamanhoTitulo = MeasureTextEx(
                fonteCosmos,
                "COSMOS",
                100.0f,
                2.0f
            );

            // Apresenta o título principal utilizando a fonte personalizada.
            DrawTextEx(
                fonteCosmos,
                "COSMOS",
                (Vector2){
                    (largura - tamanhoTitulo.x) / 2.0f,
                    50.0f
                },
                100.0f,
                2.0f,
                WHITE
            );

            // Identifica a versão atual da aplicação.
            DrawText(
                "v.1.0.0 (beta)",
                50,
                15,
                10,
                WHITE
            );

            // Apresenta a opção de iniciar uma nova partida.
            DrawText(
                "COMEÇAR PARTIDA",
                (int)largura - 355,
                (int)altura - 70,
                30,
                WHITE
            );

            // Apresenta a opção de encerramento da aplicação.
            DrawText(
                "SALVAR E SAIR",
                125,
                (int)altura - 65,
                25,
                RED
            );

            // Utiliza a fonte padrão para apresentar as configurações.
            DrawText(
                "CONFIGURACOES",
                350,
                (int)altura - 65,
                18,
                WHITE
            );

        EndDrawing();
    }

    // Encerra a reprodução do vídeo e libera seus recursos.
    VideoClose(&fundo);

    // Libera a fonte personalizada somente quando ela tiver sido carregada.
    if (fonteCarregada){
        UnloadFont(fonteCosmos);
    }

    // Descarrega os efeitos sonoros e a música ambiente.
    liberarAudioJogo(&audio);

    // Fecha a janela após liberar os recursos associados à aplicação.
    CloseWindow();
}
