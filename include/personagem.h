/**
 * @file personagem.h
 * @brief Interface do personagem ficticio que acompanha a partida.
 */
#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include "raylib.h"

#include <stdbool.h>

/** @brief Acoes visuais que podem ser representadas pelo personagem. */
typedef enum {
    PERSONAGEM_PARADO = 0,
    PERSONAGEM_PENSANDO,
    PERSONAGEM_ACERTO,
    PERSONAGEM_ERRO,
    PERSONAGEM_MARCO,
    PERSONAGEM_VITORIA
} AcaoPersonagem;

/** @brief Recursos e estado da animacao do personagem. */
typedef struct {
    Texture2D textura;
    bool carregado;
    AcaoPersonagem acao;
    float tempo;
    float tempoAcao;
    float duracaoAcao;
} PersonagemCosmos;

/** @brief Carrega a textura do personagem. Retorna false quando o arquivo nao pode ser carregado. */
bool inicializarPersonagemCosmos(PersonagemCosmos *personagem, const char *caminho);

/** @brief Define uma acao visual e reinicia seu temporizador. */
void definirAcaoPersonagem(PersonagemCosmos *personagem, AcaoPersonagem acao);

/** @brief Atualiza o tempo e a animacao do personagem. */
void atualizarPersonagemCosmos(PersonagemCosmos *personagem, float delta);

/** @brief Desenha o personagem dentro da area informada, preservando a proporcao da textura. */
void desenharPersonagemCosmos(const PersonagemCosmos *personagem, Rectangle area);

/** @brief Libera a textura carregada pelo personagem. */
void liberarPersonagemCosmos(PersonagemCosmos *personagem);

#endif
