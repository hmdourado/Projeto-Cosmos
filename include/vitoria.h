/**
 * @file vitoria.h
 * @author Arthur Morais Palitot Fernandes (arthurpalitot@gmail.com | arthurmoraisfernandes@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 25-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef VITORIA_H
#define VITORIA_H

#include <stdbool.h>
#include "video.h"
#include"raylib.h"
typedef struct {
    unsigned int perguntasRespondidas;
    unsigned int totalPerguntas;
    int acertos;
    int premioFinal;
    int premioGarantido;
    unsigned int ajudasUtilizadas;
    unsigned int totalAjudas;
} DadosTelaVitoria;

typedef struct {
    VideoBackground fundo;
    bool videoDisponivel;

    Music musica;
    bool musicaDisponivel;
} EstadoVisualVitoria;

/* Gerenciamento do vídeo e do estado visual. */
void inicializarEstadoVisualVitoria(
    EstadoVisualVitoria *estado,
    bool audioDisponivel
);
void atualizarEstadoVisualVitoria(EstadoVisualVitoria *estado);
void liberarEstadoVisualVitoria(EstadoVisualVitoria *estado);

/* Renderização da página. */
void desenharTelaVitoria(
    EstadoVisualVitoria *estado,
    const DadosTelaVitoria *dados,
    const char *nomeJogador,
    bool rankingRegistrado,
    const char *mensagemRanking
);

#endif