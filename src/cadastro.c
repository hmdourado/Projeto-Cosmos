/**
 * @file cadastro.c
 * @author Henrique Dourado
 * @brief Implementação das operações de gerenciamento de perguntas.
 * @version 0.1
 * @date 2026-11-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "../include/cadastro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inicializarCadastro (Cadastro *cadastro){
    //Verificação para evitar acesssar estrutura inexistente.
    if (cadastro == NULL){
        return;
    }

    //Indica que nenhum vetor foi alocado.
    cadastro->perguntas = NULL;

    //Indica que nenhuma pergunta foi cadastrada.
    cadastro->quantidade = 0;

    //Indica que nenhuma posição foi reservada.
    cadastro->capacidade = 0;
}