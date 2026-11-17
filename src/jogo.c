/**
 * @file jogo.c
 * @brief Implementação da inicialização e seleção de perguntas da partida.
 */
#include "../include/jogo.h"

#include <stddef.h>
#include <stdlib.h>

void inicializarPartida(Partida *partida) {
    // Verifica se o ponteiro recebido é válido.
    if (partida == NULL) {
        return;
    }

    // Inicializa todas as perguntas utilizando a função do módulo de perguntas.
    for (int i = 0; i < TOTAL_PERGUNTAS_PARTIDA; i++) {
        inicializarPergunta(&partida->perguntas[i]);
    }

    // Define os contadores, os prêmios e o estado inicial da partida.
    partida->indiceAtual = 0;
    partida->acertos = 0;
    partida->premioAtual = 0;
    partida->premioGarantido = 0;
    partida->estado = PARTIDA_NAO_INICIADA;
}

ResultadoInicio iniciarPartida(Partida *partida, const Cadastro *cadastro){
    // Verifica se os ponteiros recebidos são válidos.
    if (partida == NULL || cadastro == NULL){
        return INICIO_PARAMETRO_INVALIDO;
    }

    // Verifica a consistência do vetor informado pelo cadastro.
    if (cadastro->quantidade > cadastro->capacidade ||
        (cadastro->quantidade > 0 && cadastro->perguntas == NULL)){
        return INICIO_CADASTRO_INVALIDO;
    }

    // Define a quantidade necessária para cada nível de dificuldade.
    const int necessarias[] = {0, 2, 2, 4, 4, 3};
    size_t quantidades[MUITO_DIFICIL + 1] = {0};

    // Valida os registros antes de realizar o sorteio.
    for (size_t i = 0; i < cadastro->quantidade; i++){
        Pergunta pergunta = cadastro->perguntas[i];
        if (pergunta.id <= 0 || !validarPergunta(&pergunta)){
            return INICIO_CADASTRO_INVALIDO;
        }

        // Impede que um mesmo identificador represente duas perguntas no sorteio.
        for (size_t j = 0; j < i; j++){
            if (cadastro->perguntas[j].id == pergunta.id){
                return INICIO_CADASTRO_INVALIDO;
            }
        }
        quantidades[pergunta.nivel]++;
    }

    // Verifica se todos os níveis possuem a quantidade mínima necessária.
    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        if (quantidades[nivel] < (size_t)necessarias[nivel]){
            return INICIO_PERGUNTAS_INSUFICIENTES;
        }
    }

    // Prepara uma partida temporária para preservar a original em caso de falha.
    Partida novaPartida;
    inicializarPartida(&novaPartida);
    int inicioNivel = 0;

    // Seleciona as perguntas separadamente para manter a ordem das dificuldades.
    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        size_t encontradas = 0;
        int total = necessarias[nivel];

        // Utiliza amostragem por reservatório para não repetir registros.
        for (size_t i = 0; i < cadastro->quantidade; i++){
            if (cadastro->perguntas[i].nivel != (NivelDificuldade)nivel){
                continue;
            }
            encontradas++;
            size_t posicao = encontradas <= (size_t)total
                ? encontradas - 1 : (size_t)rand() % encontradas;
            if (posicao < (size_t)total){
                novaPartida.perguntas[inicioNivel + posicao] = cadastro->perguntas[i];
            }
        }

        // Embaralha também a ordem das perguntas dentro do mesmo nível.
        for (int i = total - 1; i > 0; i--){
            int sorteada = rand() % (i + 1);
            Pergunta temporaria = novaPartida.perguntas[inicioNivel + i];
            novaPartida.perguntas[inicioNivel + i] = novaPartida.perguntas[inicioNivel + sorteada];
            novaPartida.perguntas[inicioNivel + sorteada] = temporaria;
        }
        inicioNivel += total;
    }

    // Confirma a preparação somente após concluir todas as etapas.
    novaPartida.estado = PARTIDA_EM_ANDAMENTO;
    *partida = novaPartida;
    return INICIO_SUCESSO;
}
