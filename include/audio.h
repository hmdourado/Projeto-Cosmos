/**
 * @file audio.h
 * @brief Interfaces de gerenciamento dos recursos sonoros da aplicação.
 */
#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

#include <stdbool.h>

#define QUANT_EFEITOS_AUDIO 7

/** @brief Efeitos sonoros disponíveis durante a partida. */
typedef enum {
    EFEITO_HOVER = 0,
    EFEITO_SELECAO,
    EFEITO_TRANSICAO,
    EFEITO_RESPOSTA_CORRETA,
    EFEITO_RESPOSTA_INCORRETA,
    EFEITO_MARCO_SEGURANCA,
    EFEITO_VITORIA
} EfeitoAudio;

/** @brief Recursos e estados utilizados pelo sistema de áudio. */
typedef struct {
    Music ambiente;
    Sound efeitos[QUANT_EFEITOS_AUDIO];

    bool ambienteCarregado;
    bool efeitosCarregados[QUANT_EFEITOS_AUDIO];
    bool dispositivoInicializadoPeloModulo;

    float volumeAmbiente;
    float volumeEfeitos;
} AudioJogo;

/**
 * @brief Inicializa o dispositivo de áudio e carrega os recursos sonoros.
 * @return true quando o dispositivo de áudio estiver disponível; false em caso de falha.
 */
bool inicializarAudioJogo(AudioJogo *audio);

/** @brief Atualiza o fluxo de áudio ambiente; deve ser chamada a cada quadro. */
void atualizarAudioJogo(AudioJogo *audio);

/** @brief Inicia a reprodução contínua do ambiente espacial. */
void iniciarAmbienteAudio(AudioJogo *audio);

/** @brief Pausa a reprodução do ambiente espacial. */
void pausarAmbienteAudio(AudioJogo *audio);

/** @brief Retoma a reprodução do ambiente espacial após uma pausa. */
void retomarAmbienteAudio(AudioJogo *audio);

/** @brief Reproduz um efeito sonoro quando o recurso estiver carregado. */
void reproduzirEfeitoAudio(AudioJogo *audio, EfeitoAudio efeito);

/** @brief Define o volume do ambiente, limitado ao intervalo entre 0 e 1. */
void definirVolumeAmbienteAudio(AudioJogo *audio, float volume);

/** @brief Define o volume dos efeitos, limitado ao intervalo entre 0 e 1. */
void definirVolumeEfeitosAudio(AudioJogo *audio, float volume);

/** @brief Libera os sons, a música e o dispositivo quando pertencente ao módulo. */
void liberarAudioJogo(AudioJogo *audio);

#endif
