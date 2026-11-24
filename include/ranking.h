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

/** Aceita nomes de 1 a 40 bytes sem caracteres de controle. */
int nomeRankingValido(const char *nome);

/** Acrescenta uma partida encerrada ao histórico. Retorna 1 em caso de sucesso. */
int registrarResultadoRanking(const char *caminho, const char *nome, const Partida *partida);

/**
 * Preenche até RANKING_LIMITE posições, por prêmio e acertos decrescentes.
 * Empates mantêm a ordem de registro. Retorna a quantidade, ou -1 em caso de erro.
 * Arquivo inexistente representa um ranking vazio.
 */
int consultarRanking(const char *caminho, ResultadoRanking resultados[RANKING_LIMITE]);

/** Nome do estado final para exibição. */
const char *descreverResultadoRanking(EstadoPartida estado);

#endif
