/**
 * @file testar_modulos_ranking_pistas.c
 * @brief Testes básicos dos módulos de ranking e pistas editoriais.
 */
#include "../include/ajuda.h"
#include "../include/pistas.h"
#include "../include/ranking.h"

#include <stdio.h>
#include <string.h>

#define ARQUIVO_TESTE "dados/ranking_teste.tsv"

static int testarPistas(void) {
    Pergunta pergunta = {0};
    pergunta.id = 1;
    pergunta.nivel = MUITO_FACIL;
    strcpy(pergunta.enunciado,
           "Qual é a estrela localizada no centro do Sistema Solar?");

    const char *missao = obterTextoAjuda(&pergunta, AJUDA_MISSAO);
    const char *telescopio = obterTextoAjuda(&pergunta, AJUDA_TELESCOPIO);

    if (missao == NULL || telescopio == NULL || strcmp(missao, telescopio) == 0) {
        return 0;
    }

    /* Enunciado divergente: deve utilizar o fallback genérico por dificuldade. */
    strcpy(pergunta.enunciado, "Enunciado alterado para teste.");
    return obterTextoAjuda(&pergunta, AJUDA_MISSAO) != NULL;
}

static int testarRanking(void) {
    remove(ARQUIVO_TESTE);

    Partida partida = {PARTIDA_DERROTA, 100, 4};
    if (!registrarResultadoRanking(ARQUIVO_TESTE, "Ana", &partida)) {
        return 0;
    }

    partida.premioAtual = 200;
    partida.acertos = 3;
    if (!registrarResultadoRanking(ARQUIVO_TESTE, "Bruno", &partida)) {
        remove(ARQUIVO_TESTE);
        return 0;
    }

    partida.acertos = 5;
    if (!registrarResultadoRanking(ARQUIVO_TESTE, "Carla", &partida)) {
        remove(ARQUIVO_TESTE);
        return 0;
    }

    ResultadoRanking resultados[RANKING_LIMITE];
    int quantidade = consultarRanking(ARQUIVO_TESTE, resultados);
    int valido = quantidade == 3 &&
                 strcmp(resultados[0].nome, "Carla") == 0 &&
                 strcmp(resultados[1].nome, "Bruno") == 0 &&
                 strcmp(resultados[2].nome, "Ana") == 0 &&
                 !nomeRankingValido("   ") &&
                 nomeRankingValido("Ana Silva");

    remove(ARQUIVO_TESTE);
    return valido;
}

int main(void) {
    int pistasOk = testarPistas();
    int rankingOk = testarRanking();

    printf("Pistas: %s\n", pistasOk ? "APROVADO" : "REPROVADO");
    printf("Ranking: %s\n", rankingOk ? "APROVADO" : "REPROVADO");

    return pistasOk && rankingOk ? 0 : 1;
}
