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

/** @brief Uso por partida e alternativas eliminadas na pergunta atual. */
typedef struct {
    int usadas[TOTAL_AJUDAS];
    int ativas[TOTAL_AJUDAS]; /**< Efeitos disponíveis na pergunta atual. */
    int eliminadas[QUANT_ALTERNATIVAS];
    char sugestaoEspecialista; /**< Sugestão guardada até a próxima pergunta. */
} EstadoAjudas;

typedef enum {
    AJUDA_SUCESSO = 0,
    AJUDA_JA_USADA,
    AJUDA_SEM_CONTEUDO,
    AJUDA_NAO_PERMITIDA,
    AJUDA_INVALIDA
} ResultadoAjuda;

/** @brief Inicializa os usos da partida e os efeitos da pergunta. */
void inicializarAjudas(EstadoAjudas *ajudas);

/** @brief Limpa os efeitos da pergunta anterior, preservando os usos gastos. */
void prepararAjudasPergunta(EstadoAjudas *ajudas);

/** @brief Retorna a chance percentual do especialista, ou zero para nível inválido. */
int obterChanceEspecialista(NivelDificuldade nivel);

/**
 * @brief Consulta a pista ou informação cadastrada para a pergunta.
 * @return Texto de duração estática, ou NULL se não houver conteúdo correspondente.
 */
const char *obterTextoAjuda(const Pergunta *pergunta, TipoAjuda tipo);

/**
 * @brief Aplica uma ajuda e registra seu uso somente em caso de sucesso.
 * @note A interface deve usar a função usarAjuda de jogo.h para validar a partida.
 */
ResultadoAjuda aplicarAjuda(EstadoAjudas *ajudas, const Pergunta *pergunta, TipoAjuda tipo);

#endif
