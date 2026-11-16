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

int adicionarPergunta(Cadastro *cadastro, const Pergunta *pergunta){

    // Verifica se os ponteiros recebidos são válidos.
    if (cadastro == NULL || pergunta == NULL){
        return 0;
    }

    // Verifica se o vetor atingiu sua capacidade máxima.
    if (cadastro->quantidade == cadastro->capacidade){

        // Calcula a nova capacidade, adicionando 10 posições.
        size_t novaCapacidade = cadastro->capacidade + 10;

        // Realoca a memória para comportar a nova capacidade.
        Pergunta *temp = realloc(cadastro->perguntas, sizeof(Pergunta) * novaCapacidade);

        // Verifica se a realocação foi bem-sucedida.
        if (temp != NULL){
            // Atualiza o ponteiro e a capacidade do cadastro.
            cadastro->perguntas = temp;
            cadastro->capacidade = novaCapacidade;

        } else {
            // Retorna falha caso não seja possível alocar memória.
            return 0;
        }
    }

    // Copia a pergunta para a próxima posição disponível.
    cadastro->perguntas[cadastro->quantidade] = *pergunta;

    // Atualiza a quantidade de perguntas cadastradas.
    cadastro->quantidade++;

    // Indica que a operação foi concluída com sucesso.
    return 1;
}