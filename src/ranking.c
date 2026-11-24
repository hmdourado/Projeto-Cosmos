/**
 * @file ranking.c
 * @brief Persistência e classificação dos resultados das partidas.
 */
#include "../include/ranking.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nomeRankingValido(const char *nome) {
    if (nome == NULL) {
        return 0;
    }

    size_t tamanho = strlen(nome);
    if (tamanho == 0 || tamanho >= RANKING_TAM_NOME) {
        return 0;
    }

    int temTexto = 0;
    for (const unsigned char *caractere = (const unsigned char *)nome;
         *caractere != '\0';
         caractere++) {
        if (iscntrl(*caractere)) {
            return 0;
        }
        if (!isspace(*caractere)) {
            temTexto = 1;
        }
    }

    return temTexto;
}

const char *descreverResultadoRanking(EstadoPartida estado) {
    switch (estado) {
        case PARTIDA_VITORIA:
            return "Vitória";
        case PARTIDA_DERROTA:
            return "Derrota";
        case PARTIDA_DESISTENCIA:
            return "Desistência";
        default:
            return "Desconhecido";
    }
}

static int resultadoValido(EstadoPartida estado, int premio, int acertos) {
    if (premio < 0 || acertos < 0 || acertos > TOTAL_PERGUNTAS_PARTIDA) {
        return 0;
    }

    switch (estado) {
        case PARTIDA_VITORIA:
            return acertos == TOTAL_PERGUNTAS_PARTIDA;
        case PARTIDA_DERROTA:
        case PARTIDA_DESISTENCIA:
            return acertos < TOTAL_PERGUNTAS_PARTIDA;
        default:
            return 0;
    }
}

int registrarResultadoRanking(
    const char *caminho,
    const char *nome,
    const Partida *partida
) {
    if (caminho == NULL || !nomeRankingValido(nome) || partida == NULL ||
        !resultadoValido(partida->estado, partida->premioAtual, partida->acertos)) {
        return 0;
    }

    FILE *arquivo = fopen(caminho, "a");
    if (arquivo == NULL) {
        return 0;
    }

    int escreveu = fprintf(
        arquivo,
        "%d\t%d\t%d\t%s\n",
        (int)partida->estado,
        partida->premioAtual,
        partida->acertos,
        nome
    ) >= 0;

    int fechou = fclose(arquivo) == 0;
    return escreveu && fechou;
}

static int lerResultado(char *linha, ResultadoRanking *resultado) {
    if (linha == NULL || resultado == NULL) {
        return 0;
    }

    size_t tamanho = strlen(linha);
    while (tamanho > 0 &&
           (linha[tamanho - 1] == '\n' || linha[tamanho - 1] == '\r')) {
        linha[--tamanho] = '\0';
    }

    long campos[3];
    char *cursor = linha;

    for (int i = 0; i < 3; i++) {
        char *fim = NULL;
        errno = 0;
        campos[i] = strtol(cursor, &fim, 10);

        if (fim == cursor || *fim != '\t' || errno == ERANGE ||
            campos[i] < 0 || campos[i] > INT_MAX) {
            return 0;
        }

        cursor = fim + 1;
    }

    if (!nomeRankingValido(cursor)) {
        return 0;
    }

    EstadoPartida estado = (EstadoPartida)campos[0];
    int premio = (int)campos[1];
    int acertos = (int)campos[2];

    if (!resultadoValido(estado, premio, acertos)) {
        return 0;
    }

    resultado->estado = estado;
    resultado->premio = premio;
    resultado->acertos = acertos;
    memcpy(resultado->nome, cursor, strlen(cursor) + 1);
    return 1;
}

int consultarRanking(
    const char *caminho,
    ResultadoRanking resultados[RANKING_LIMITE]
) {
    if (caminho == NULL || resultados == NULL) {
        return -1;
    }

    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        return errno == ENOENT ? 0 : -1;
    }

    int quantidade = 0;
    char linha[128];
    int erroFormato = 0;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        size_t tamanho = strlen(linha);

        /* Linha sem '\n' só é válida quando é o último registro do arquivo. */
        if (tamanho > 0 && linha[tamanho - 1] != '\n' && !feof(arquivo)) {
            erroFormato = 1;
            break;
        }

        ResultadoRanking atual;
        if (!lerResultado(linha, &atual)) {
            erroFormato = 1;
            break;
        }

        int posicao = quantidade;
        while (posicao > 0 &&
               (atual.premio > resultados[posicao - 1].premio ||
                (atual.premio == resultados[posicao - 1].premio &&
                 atual.acertos > resultados[posicao - 1].acertos))) {
            posicao--;
        }

        if (posicao < RANKING_LIMITE) {
            if (quantidade < RANKING_LIMITE) {
                quantidade++;
            }

            memmove(
                &resultados[posicao + 1],
                &resultados[posicao],
                (size_t)(quantidade - posicao - 1) * sizeof(*resultados)
            );
            resultados[posicao] = atual;
        }
    }

    int leituraOk = !ferror(arquivo) && !erroFormato;
    int fechamentoOk = fclose(arquivo) == 0;

    return leituraOk && fechamentoOk ? quantidade : -1;
}
