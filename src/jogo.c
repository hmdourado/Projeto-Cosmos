/**
 * @file jogo.c
 * @brief Implementação da inicialização e seleção de perguntas da partida.
 */
#include "../include/jogo.h"

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

int obterPremioPergunta(int numero){
    // Centraliza a tabela provisória de prêmios para facilitar ajustes do grupo.
    const int premios[TOTAL_PERGUNTAS_PARTIDA] = {
        1000, 2000, 3000, 4000, 5000,
        10000, 20000, 30000, 40000, 50000,
        100000, 200000, 300000, 500000, 1000000
    };
    if (numero < 1 || numero > TOTAL_PERGUNTAS_PARTIDA){
        return 0;
    }
    return premios[numero - 1];
}

const Pergunta *obterPerguntaAtual(const Partida *partida){
    // Impede a consulta de perguntas antes do início ou após o encerramento.
    if (partida == NULL || partida->estado != PARTIDA_EM_ANDAMENTO ||
        partida->indiceAtual < 0 || partida->indiceAtual >= TOTAL_PERGUNTAS_PARTIDA){
        return NULL;
    }
    return &partida->perguntas[partida->indiceAtual];
}

ResultadoResposta responderPergunta(Partida *partida, char resposta){
    const Pergunta *pergunta = obterPerguntaAtual(partida);
    if (pergunta == NULL){
        return RESPOSTA_NAO_PERMITIDA;
    }

    // Aceita letras minúsculas sem consumir tentativas para entradas inválidas.
    resposta = (char)toupper((unsigned char)resposta);
    if (resposta < 'A' || resposta > 'D'){
        return RESPOSTA_INVALIDA;
    }
    // Uma alternativa eliminada não pode ser usada como resposta.
    if (partida->ajudas.eliminadas[resposta - 'A']){
        return RESPOSTA_INVALIDA;
    }

    // Em caso de erro, encerra mantendo somente o valor do último marco.
    if (resposta != pergunta->respostaCorreta){
        partida->premioAtual = partida->premioGarantido;
        partida->estado = PARTIDA_DERROTA;
        return RESPOSTA_INCORRETA;
    }

    // Registra o acerto e protege o prêmio ao atingir os marcos de segurança.
    int numero = partida->indiceAtual + 1;
    partida->acertos++;
    partida->premioAtual = obterPremioPergunta(numero);
    if (numero == 5 || numero == 10){
        partida->premioGarantido = partida->premioAtual;
    }

    // A última resposta correta encerra a partida com vitória.
    if (numero == TOTAL_PERGUNTAS_PARTIDA){
        partida->estado = PARTIDA_VITORIA;
        return RESPOSTA_VITORIA;
    }
    partida->indiceAtual++;
    prepararAjudasPergunta(&partida->ajudas);
    return RESPOSTA_CORRETA;
}

ResultadoAjuda usarAjuda(Partida *partida, TipoAjuda tipo){
    const Pergunta *pergunta = obterPerguntaAtual(partida);
    if (pergunta == NULL){
        return AJUDA_NAO_PERMITIDA;
    }
    return aplicarAjuda(&partida->ajudas, pergunta, tipo);
}

int desistirPartida(Partida *partida){
    // A desistência só pode ocorrer enquanto existe uma pergunta disponível.
    if (obterPerguntaAtual(partida) == NULL){
        return 0;
    }
    partida->estado = PARTIDA_DESISTENCIA;
    return 1;
}

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
    inicializarAjudas(&partida->ajudas);
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
