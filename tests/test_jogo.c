/**
 * @file test_jogo.c
 * @brief Testes de inicializacao e reinicializacao sem Raylib.
 */
#include "../include/jogo.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/** @brief Verifica o sorteio, a distribuição e as falhas de preparação. */
static void testarInicioPartida(void){
    Cadastro cadastro;
    Partida partida;
    inicializarCadastro(&cadastro);
    inicializarPartida(&partida);
    srand(42);

    // Verifica parâmetros ausentes e um cadastro sem perguntas.
    assert(iniciarPartida(NULL, &cadastro) == INICIO_PARAMETRO_INVALIDO);
    assert(iniciarPartida(&partida, NULL) == INICIO_PARAMETRO_INVALIDO);
    assert(iniciarPartida(&partida, &cadastro) == INICIO_PERGUNTAS_INSUFICIENTES);
    assert(partida.estado == PARTIDA_NAO_INICIADA);

    // Monta um banco com perguntas extras para permitir diferentes sorteios.
    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        for (int i = 0; i < 6; i++){
            Pergunta pergunta;
            inicializarPergunta(&pergunta);
            pergunta.id = nivel * 100 + i;
            pergunta.nivel = (NivelDificuldade)nivel;
            snprintf(pergunta.enunciado, sizeof(pergunta.enunciado), "Pergunta %d", pergunta.id);
            for (int j = 0; j < QUANT_ALTERNATIVAS; j++){
                snprintf(pergunta.alternativas[j], TAM_ALTERNATIVA, "Alternativa %d", j);
            }
            pergunta.respostaCorreta = 'A';
            assert(adicionarPergunta(&cadastro, &pergunta));
        }
    }

    const NivelDificuldade esperados[TOTAL_PERGUNTAS_PARTIDA] = {
        MUITO_FACIL, MUITO_FACIL, FACIL, FACIL,
        MEDIO, MEDIO, MEDIO, MEDIO, DIFICIL, DIFICIL, DIFICIL, DIFICIL,
        MUITO_DIFICIL, MUITO_DIFICIL, MUITO_DIFICIL
    };

    // Repete o sorteio para conferir as regras com diferentes seleções.
    for (int tentativa = 0; tentativa < 100; tentativa++){
        assert(iniciarPartida(&partida, &cadastro) == INICIO_SUCESSO);
        assert(partida.estado == PARTIDA_EM_ANDAMENTO);
        assert(partida.indiceAtual == 0 && partida.acertos == 0);
        assert(partida.premioAtual == 0 && partida.premioGarantido == 0);
        for (int i = 0; i < TOTAL_PERGUNTAS_PARTIDA; i++){
            assert(partida.perguntas[i].nivel == esperados[i]);
            for (int j = 0; j < i; j++){
                assert(partida.perguntas[i].id != partida.perguntas[j].id);
            }
        }
    }

    // Confere que uma falha não altera a partida já preparada.
    Partida anterior = partida;
    cadastro.perguntas[1].id = cadastro.perguntas[0].id;
    assert(iniciarPartida(&partida, &cadastro) == INICIO_CADASTRO_INVALIDO);
    assert(memcmp(&partida, &anterior, sizeof(partida)) == 0);
    cadastro.perguntas[1].id = 101;

    // Remove cada dificuldade do banco temporariamente para verificar a falta.
    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        for (size_t i = 0; i < cadastro.quantidade; i++){
            if (cadastro.perguntas[i].id / 100 == nivel){
                cadastro.perguntas[i].nivel = nivel == MEDIO ? FACIL : MEDIO;
            }
        }
        assert(iniciarPartida(&partida, &cadastro) == INICIO_PERGUNTAS_INSUFICIENTES);
        assert(memcmp(&partida, &anterior, sizeof(partida)) == 0);
        for (size_t i = 0; i < cadastro.quantidade; i++){
            cadastro.perguntas[i].nivel = (NivelDificuldade)(cadastro.perguntas[i].id / 100);
        }
    }

    // Confere que as perguntas da partida não dependem da memória do cadastro.
    liberarCadastro(&cadastro);
    assert(partida.perguntas[0].enunciado[0] != '\0');
}

/** @brief Verifica todos os pontos de erro e desistência, além da vitória. */
static void testarRegrasPartida(void){
    assert(obterPerguntaAtual(NULL) == NULL);
    assert(responderPergunta(NULL, 'A') == RESPOSTA_NAO_PERMITIDA);
    assert(desistirPartida(NULL) == 0);
    assert(obterPremioPergunta(0) == 0);
    assert(obterPremioPergunta(16) == 0);

    for (int parada = 0; parada <= TOTAL_PERGUNTAS_PARTIDA; parada++){
        Partida partida;
        inicializarPartida(&partida);
        assert(obterPerguntaAtual(&partida) == NULL);
        assert(desistirPartida(&partida) == 0);
        partida.estado = PARTIDA_EM_ANDAMENTO;
        for (int i = 0; i < TOTAL_PERGUNTAS_PARTIDA; i++){
            partida.perguntas[i].respostaCorreta = 'A';
        }

        for (int i = 0; i < parada; i++){
            // Uma entrada inválida deve preservar todos os dados.
            Partida anterior = partida;
            assert(responderPergunta(&partida, 'Z') == RESPOSTA_INVALIDA);
            assert(memcmp(&partida, &anterior, sizeof(partida)) == 0);
            ResultadoResposta esperado = i == 14 ? RESPOSTA_VITORIA : RESPOSTA_CORRETA;
            assert(responderPergunta(&partida, 'a') == esperado);
            assert(partida.acertos == i + 1);
            assert(partida.premioAtual == obterPremioPergunta(i + 1));
            int garantido = i >= 9 ? 50000 : (i >= 4 ? 5000 : 0);
            assert(partida.premioGarantido == garantido);
        }

        if (parada == TOTAL_PERGUNTAS_PARTIDA){
            assert(partida.estado == PARTIDA_VITORIA);
            assert(partida.premioAtual == 1000000);
            assert(obterPerguntaAtual(&partida) == NULL);
        } else {
            assert(partida.indiceAtual == parada);
            Partida desistente = partida;
            assert(desistirPartida(&desistente) == 1);
            assert(desistente.estado == PARTIDA_DESISTENCIA);
            assert(desistente.premioAtual == obterPremioPergunta(parada));
            assert(desistirPartida(&desistente) == 0);
            assert(responderPergunta(&desistente, 'A') == RESPOSTA_NAO_PERMITIDA);

            assert(responderPergunta(&partida, 'B') == RESPOSTA_INCORRETA);
            assert(partida.estado == PARTIDA_DERROTA);
            assert(partida.premioAtual == partida.premioGarantido);
            assert(partida.acertos == parada);
        }

        // Nenhuma ação posterior ao fim pode modificar o resultado.
        Partida encerrada = partida;
        assert(responderPergunta(&partida, 'A') == RESPOSTA_NAO_PERMITIDA);
        assert(desistirPartida(&partida) == 0);
        assert(memcmp(&partida, &encerrada, sizeof(partida)) == 0);
    }
}

int main(void) {
    Partida partida;

    /* Simula dados anteriores para verificar que nenhum campo e preservado. */
    memset(&partida, 'X', sizeof(partida));
    partida.indiceAtual = 10;
    partida.acertos = 10;
    partida.premioAtual = 100000;
    partida.premioGarantido = 50000;
    partida.estado = PARTIDA_DERROTA;

    inicializarPartida(&partida);

    assert(partida.indiceAtual == 0);
    assert(partida.acertos == 0);
    assert(partida.premioAtual == 0);
    assert(partida.premioGarantido == 0);
    assert(partida.estado == PARTIDA_NAO_INICIADA);

    for (int i = 0; i < TOTAL_PERGUNTAS_PARTIDA; i++) {
        const Pergunta *pergunta = &partida.perguntas[i];
        assert(pergunta->id == 0);
        assert(pergunta->respostaCorreta == '\0');
        assert(pergunta->nivel == NAO_DEFINIDO);
        for (size_t j = 0; j < sizeof(pergunta->enunciado); j++) {
            assert(pergunta->enunciado[j] == '\0');
        }
        for (int j = 0; j < QUANT_ALTERNATIVAS; j++) {
            for (int k = 0; k < TAM_ALTERNATIVA; k++) {
                assert(pergunta->alternativas[j][k] == '\0');
            }
        }
    }

    inicializarPartida(NULL);
    testarInicioPartida();
    testarRegrasPartida();
    puts("Testes de inicialização, sorteio e regras da partida passaram.");
    return 0;
}
