/**
 * @file ranking.h
 * @brief Interface pública do histórico e da classificação de partidas.
 */
#ifndef RANKING_H
#define RANKING_H

#include "jogo.h"

#define RANKING_LIMITE 10
#define RANKING_TAM_NOME 41
#define RANKING_ARQUIVO "dados/ranking.tsv"

typedef struct {
    char nome[RANKING_TAM_NOME];
    int premio;
    int acertos;
    EstadoPartida estado;
} ResultadoRanking;

/** Aceita nomes de 1 a 40 bytes, sem caracteres de controle. */
int nomeRankingValido(const char *nome);

/** Registra uma partida encerrada. Retorna 1 em caso de sucesso. */
int registrarResultadoRanking(
    const char *caminho,
    const char *nome,
    const Partida *partida
);

/**
 * Lê e ordena os dez melhores resultados por prêmio e acertos decrescentes.
 * Empates completos preservam a ordem de registro. Arquivo inexistente retorna 0.
 * Retorna a quantidade de resultados ou -1 em caso de erro.
 */
int consultarRanking(
    const char *caminho,
    ResultadoRanking resultados[RANKING_LIMITE]
);

/** Retorna uma descrição estática do estado final da partida. */
const char *descreverResultadoRanking(EstadoPartida estado);

#endif
