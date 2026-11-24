/**
 * @file ajuda.h
 * @brief Tipos, limites e efeitos das ajudas de uma partida.
 */
#ifndef AJUDA_H
#define AJUDA_H

#include "pergunta.h"

typedef enum {
    AJUDA_SUPERNOVA = 0,
    AJUDA_MISSAO,
    AJUDA_TELESCOPIO,
    AJUDA_ESPECIALISTA,
    TOTAL_AJUDAS
} TipoAjuda;

/** @brief Controla os usos por partida e os efeitos da pergunta atual. */
typedef struct {
    int usadas[TOTAL_AJUDAS];
    int ativas[TOTAL_AJUDAS];
    int eliminadas[QUANT_ALTERNATIVAS];
    char sugestaoEspecialista;
} EstadoAjudas;

typedef enum {
    AJUDA_SUCESSO = 0,
    AJUDA_JA_USADA,
    AJUDA_SEM_CONTEUDO,
    AJUDA_NAO_PERMITIDA,
    AJUDA_INVALIDA
} ResultadoAjuda;

/** @brief Inicializa os usos e os efeitos de todas as ajudas. */
void inicializarAjudas(EstadoAjudas *ajudas);

/** @brief Limpa os efeitos da pergunta anterior, preservando os usos consumidos. */
void prepararAjudasPergunta(EstadoAjudas *ajudas);

/** @brief Retorna a chance percentual do especialista ou zero para nível inválido. */
int obterChanceEspecialista(NivelDificuldade nivel);

/**
 * @brief Retorna a pista específica vinculada à pergunta.
 * @param pergunta Pergunta atual, identificada pelo ID e pelo enunciado.
 * @param tipo Ajuda textual solicitada (Missão ou Telescópio).
 * @return Texto estático correspondente ou NULL sem correspondência editorial.
 */
const char *obterTextoAjuda(const Pergunta *pergunta, TipoAjuda tipo);

/**
 * @brief Aplica uma ajuda e registra seu uso somente quando a operação tem sucesso.
 * @param ajudas Estado de utilização das ajudas na partida.
 * @param pergunta Pergunta atualmente apresentada.
 * @param tipo Tipo de ajuda solicitado.
 * @return Resultado da aplicação da ajuda.
 */
ResultadoAjuda aplicarAjuda(
    EstadoAjudas *ajudas,
    const Pergunta *pergunta,
    TipoAjuda tipo
);

#endif
