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

#define BLOCO_PERGUNTAS 10

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
        size_t novaCapacidade = cadastro->capacidade + BLOCO_PERGUNTAS;

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

Pergunta *buscarPerguntaPorId(Cadastro *cadastro, int id){
    // Verifica se o cadastro existe e se o ID informado é válido.
    if (cadastro == NULL || id <= 0){
        return NULL;
    }

    // Percorre todas as perguntas efetivamente cadastradas.
    for (size_t i = 0; i < cadastro->quantidade; i++){
        // Compara o ID da pergunta atual com o identificador procurado.
        if (cadastro->perguntas[i].id == id){

             // Retorna o endereço da pergunta encontrada no vetor.
            return &cadastro->perguntas[i];
        }
    }

     // Retorna NULL caso nenhuma pergunta corresponda ao ID informado.
    return NULL;
}

int atualizarPergunta(Cadastro *cadastro, int id, const Pergunta *novaPergunta){
    // Verifica se o cadastro existe e se o ID informado é válido e se os novos dados são existentes.
    if (cadastro == NULL || id <= 0 || novaPergunta == NULL){
        return 0;
    }

    //Armazenamento do resultado da busca.
    Pergunta *perguntaEncontrada = buscarPerguntaPorId(cadastro, id);

    //Verificação de pergunta encontrada.
    if (perguntaEncontrada == NULL){
        return 0;
    }

    // Guarda o identificador original.
    int idOriginal = perguntaEncontrada->id;

    // Substitui os dados da pergunta.
    *perguntaEncontrada = *novaPergunta;

    // Restaura o identificador original.
    perguntaEncontrada->id = idOriginal;

    //Retorna 1 que significa sucesso
    return 1;
}

int removerPergunta(Cadastro *cadastro, int id){
    // Verifica se o cadastro existe e se o ID informado é válido.
    if (cadastro == NULL || id <= 0){
        return 0;
    }

    // Busca a pergunta correspondente ao ID informado.
    Pergunta *perguntaEncontrada = buscarPerguntaPorId(cadastro, id);

    // Verifica se a pergunta foi encontrada.
    if (perguntaEncontrada == NULL){
        return 0;
    }

    // Calcula o índice da pergunta encontrada no vetor.
    size_t indice = perguntaEncontrada - cadastro->perguntas;

    // Desloca as perguntas seguintes para preencher a posição removida.
    for (size_t i = indice; i < cadastro->quantidade - 1; i++){
        cadastro->perguntas[i] = cadastro->perguntas[i + 1];
    }

    // Atualiza a quantidade de perguntas cadastradas.
    cadastro->quantidade--;

    if (cadastro->quantidade == 0){
        liberarCadastro(cadastro);
    } else if (cadastro->capacidade - cadastro->quantidade >= 2 * BLOCO_PERGUNTAS){
        size_t novaCapacidade = cadastro->capacidade - BLOCO_PERGUNTAS;
        Pergunta *temp = realloc(cadastro->perguntas, sizeof(Pergunta) * novaCapacidade);
        if (temp != NULL){
            cadastro->perguntas = temp;
            cadastro->capacidade = novaCapacidade;
        }
    }

    // Indica que a remoção foi realizada com sucesso.
    return 1;
}

void liberarCadastro(Cadastro *cadastro){
    // Verifica se o cadastro existe.
    if (cadastro == NULL){
        return;
    }

    // Libera a memória do vetor.
    free(cadastro->perguntas);

    // Reinicializa os campos do cadastro.
    inicializarCadastro(cadastro);
}
