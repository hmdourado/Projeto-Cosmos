#include "../include/ranking.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nomeRankingValido(const char *nome){
    if (nome == NULL || strlen(nome) >= RANKING_TAM_NOME){
        return 0;
    }
    int temTexto = 0;
    for (const unsigned char *letra = (const unsigned char *)nome; *letra; letra++){
        if (iscntrl(*letra)){
            return 0;
        }
        temTexto |= !isspace(*letra);
    }
    return temTexto;
}

const char *descreverResultadoRanking(EstadoPartida estado){
    switch (estado){
        case PARTIDA_VITORIA: return "Vitória";
        case PARTIDA_DERROTA: return "Derrota";
        case PARTIDA_DESISTENCIA: return "Desistência";
        default: return "";
    }
}

static int resultadoValido(int estado, int premio, int acertos){
    if (premio < 0 || acertos < 0 || acertos > TOTAL_PERGUNTAS_PARTIDA){
        return 0;
    }
    switch (estado){
        case PARTIDA_VITORIA:
            return acertos == TOTAL_PERGUNTAS_PARTIDA;
        case PARTIDA_DESISTENCIA:
        case PARTIDA_DERROTA:
            return acertos < TOTAL_PERGUNTAS_PARTIDA;
        default:
            return 0;
    }
}

int registrarResultadoRanking(const char *caminho, const char *nome, const Partida *partida){
    if (caminho == NULL || !nomeRankingValido(nome) || partida == NULL ||
        !resultadoValido(partida->estado, partida->premioAtual, partida->acertos)){
        return 0;
    }
    FILE *arquivo = fopen(caminho, "a");
    if (arquivo == NULL){
        return 0;
    }
    int escreveu = fprintf(arquivo, "%d\t%d\t%d\t%s\n",
                           partida->estado, partida->premioAtual, partida->acertos, nome) > 0;
    int fechou = fclose(arquivo) == 0;
    return escreveu && fechou;
}

static int lerResultado(char *linha, ResultadoRanking *resultado){
    long campos[3];
    for (int i = 0; i < 3; i++){
        char *fim;
        errno = 0;
        campos[i] = strtol(linha, &fim, 10);
        if (fim == linha || *fim != '\t' || errno != 0 || campos[i] < 0 ||
            campos[i] > INT_MAX){
            return 0;
        }
        linha = fim + 1;
    }
    char *quebra = strchr(linha, '\n');
    if (quebra == NULL){
        return 0;
    }
    *quebra = '\0';
    if (quebra > linha && quebra[-1] == '\r'){
        quebra[-1] = '\0';
    }
    if (!nomeRankingValido(linha) || !resultadoValido((int)campos[0], (int)campos[1], (int)campos[2])){
        return 0;
    }
    resultado->estado = (EstadoPartida)campos[0];
    resultado->premio = (int)campos[1];
    resultado->acertos = (int)campos[2];
    strcpy(resultado->nome, linha);
    return 1;
}

int consultarRanking(const char *caminho, ResultadoRanking resultados[RANKING_LIMITE]){
    if (caminho == NULL || resultados == NULL){
        return -1;
    }
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL){
        return errno == ENOENT ? 0 : -1;
    }
    int quantidade = 0;
    char linha[128];
    while (fgets(linha, sizeof(linha), arquivo) != NULL){
        ResultadoRanking resultado;
        if (!lerResultado(linha, &resultado)){
            fclose(arquivo);
            return -1;
        }
        int posicao = quantidade;
        while (posicao > 0 &&
               (resultado.premio > resultados[posicao - 1].premio ||
                (resultado.premio == resultados[posicao - 1].premio &&
                 resultado.acertos > resultados[posicao - 1].acertos))){
            posicao--;
        }
        if (posicao < RANKING_LIMITE){
            if (quantidade < RANKING_LIMITE){
                quantidade++;
            }
            memmove(&resultados[posicao + 1], &resultados[posicao],
                    (size_t)(quantidade - posicao - 1) * sizeof(*resultados));
            resultados[posicao] = resultado;
        }
    }
    int leu = !ferror(arquivo);
    int fechou = fclose(arquivo) == 0;
    return leu && fechou ? quantidade : -1;
}
