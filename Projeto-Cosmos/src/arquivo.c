/**
 * @file arquivo.c
 * @author Henrique Dourado
 * @brief 
 * @version 0.1
 * @date 2026-11-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "../include/arquivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int carregarCSV(Cadastro *cadastro, const char *caminho){
    // Verifica se os parâmetros são válidos.
    if (cadastro == NULL || caminho == NULL){
        return 0;
    }

    // Abre o arquivo CSV para leitura.
    FILE *arquivo = fopen(caminho, "r");

    // Verifica se o arquivo foi aberto.
    if (arquivo == NULL){
        return 0;
    }

    // A leitura será implementada aqui.
    fclose(arquivo);

    return 1;
}