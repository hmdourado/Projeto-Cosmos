/**
 * @file test_ajuda.c
 * @brief Testes dos limites, efeitos e integração das quatro ajudas.
 */
#include "../include/jogo.h"
#include "../include/arquivo.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Prepara duas perguntas para testar a transição entre elas. */
static void prepararPartida(Partida *partida, const Pergunta *pergunta){
    inicializarPartida(partida);
    partida->estado = PARTIDA_EM_ANDAMENTO;
    partida->perguntas[0] = *pergunta;
    partida->perguntas[1] = *pergunta;
}

int main(void){
    Cadastro cadastro;
    inicializarCadastro(&cadastro);
    assert(carregarCSV(&cadastro, "dados/perguntas.csv"));
    srand(123);

    // Todas as perguntas originais devem ter os dois textos editoriais.
    for (size_t i = 0; i < cadastro.quantidade; i++){
        const char *pista = obterTextoAjuda(&cadastro.perguntas[i], AJUDA_MISSAO);
        const char *informacao = obterTextoAjuda(&cadastro.perguntas[i], AJUDA_TELESCOPIO);
        assert(pista != NULL && informacao != NULL);
        assert(strcmp(pista, informacao) != 0);
    }

    Partida partida;
    prepararPartida(&partida, &cadastro.perguntas[0]);
    assert(usarAjuda(NULL, AJUDA_SUPERNOVA) == AJUDA_NAO_PERMITIDA);
    assert(usarAjuda(&partida, (TipoAjuda)-1) == AJUDA_INVALIDA);
    assert(usarAjuda(&partida, TOTAL_AJUDAS) == AJUDA_INVALIDA);

    // Pistas distintas podem ser combinadas, mas cada uma só pode ser gasta uma vez.
    assert(usarAjuda(&partida, AJUDA_MISSAO) == AJUDA_SUCESSO);
    assert(usarAjuda(&partida, AJUDA_MISSAO) == AJUDA_JA_USADA);
    assert(usarAjuda(&partida, AJUDA_TELESCOPIO) == AJUDA_SUCESSO);
    assert(partida.indiceAtual == 0 && partida.premioAtual == 0);

    // A Supernova elimina exatamente duas incorretas e bloqueia suas respostas.
    assert(usarAjuda(&partida, AJUDA_SUPERNOVA) == AJUDA_SUCESSO);
    int total = 0;
    int correta = partida.perguntas[0].respostaCorreta - 'A';
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
        total += partida.ajudas.eliminadas[i];
        if (partida.ajudas.eliminadas[i]){
            assert(responderPergunta(&partida, (char)('A' + i)) == RESPOSTA_INVALIDA);
        }
    }
    assert(total == 2 && !partida.ajudas.eliminadas[correta]);
    assert(usarAjuda(&partida, AJUDA_SUPERNOVA) == AJUDA_JA_USADA);
    assert(usarAjuda(&partida, AJUDA_ESPECIALISTA) == AJUDA_SUCESSO);
    assert(!partida.ajudas.eliminadas[partida.ajudas.sugestaoEspecialista - 'A']);

    // O avanço remove os efeitos, preservando o gasto de todas as ajudas.
    assert(responderPergunta(&partida, (char)('A' + correta)) == RESPOSTA_CORRETA);
    for (int i = 0; i < TOTAL_AJUDAS; i++){
        assert(partida.ajudas.usadas[i] && !partida.ajudas.ativas[i]);
        assert(usarAjuda(&partida, (TipoAjuda)i) == AJUDA_JA_USADA);
    }
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
        assert(!partida.ajudas.eliminadas[i]);
    }
    assert(partida.ajudas.sugestaoEspecialista == '\0');
    assert(desistirPartida(&partida));
    assert(usarAjuda(&partida, AJUDA_MISSAO) == AJUDA_NAO_PERMITIDA);

    // Reiniciar devolve os usos; perguntas novas ou alteradas não recebem pistas erradas.
    prepararPartida(&partida, &cadastro.perguntas[0]);
    partida.perguntas[0].id = 999;
    assert(usarAjuda(&partida, AJUDA_MISSAO) == AJUDA_SEM_CONTEUDO);
    assert(!partida.ajudas.usadas[AJUDA_MISSAO]);
    partida.perguntas[0] = cadastro.perguntas[0];
    strcpy(partida.perguntas[0].enunciado, "Pergunta alterada");
    assert(usarAjuda(&partida, AJUDA_TELESCOPIO) == AJUDA_SEM_CONTEUDO);
    assert(!partida.ajudas.usadas[AJUDA_TELESCOPIO]);

    // Exercita as duas ordens de combinação e as probabilidades por dificuldade.
    const int chances[] = {0, 90, 80, 70, 60, 50};
    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        int acertos = 0;
        assert(obterChanceEspecialista((NivelDificuldade)nivel) == chances[nivel]);
        for (int tentativa = 0; tentativa < 10000; tentativa++){
            prepararPartida(&partida, &cadastro.perguntas[0]);
            partida.perguntas[0].nivel = (NivelDificuldade)nivel;
            if (tentativa % 2 == 0){
                assert(usarAjuda(&partida, AJUDA_SUPERNOVA) == AJUDA_SUCESSO);
            }
            assert(usarAjuda(&partida, AJUDA_ESPECIALISTA) == AJUDA_SUCESSO);
            char sugestao = partida.ajudas.sugestaoEspecialista;
            assert(sugestao >= 'A' && sugestao <= 'D');
            assert(!partida.ajudas.eliminadas[sugestao - 'A']);
            acertos += sugestao == partida.perguntas[0].respostaCorreta;
            if (tentativa % 2 != 0){
                assert(usarAjuda(&partida, AJUDA_SUPERNOVA) == AJUDA_SUCESSO);
                sugestao = partida.ajudas.sugestaoEspecialista;
                assert(sugestao == '\0' || !partida.ajudas.eliminadas[sugestao - 'A']);
            }
        }
        // Usa margem ampla para testar a configuração sem exigir uma sequência específica de rand.
        assert(acertos > (chances[nivel] - 5) * 100);
        assert(acertos < (chances[nivel] + 5) * 100);
    }

    liberarCadastro(&cadastro);
    puts("Testes das quatro ajudas passaram.");
    return 0;
}
