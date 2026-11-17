/**
 * @file ajuda.c
 * @brief Implementação dos limites de uso e efeitos das ajudas.
 */
#include "../include/ajuda.h"
#include <stddef.h>
#include <stdlib.h>

void prepararAjudasPergunta(EstadoAjudas *ajudas){
    if (ajudas == NULL){
        return;
    }

    // Remove somente os efeitos que pertencem à pergunta anterior.
    for (int i = 0; i < TOTAL_AJUDAS; i++){
        ajudas->ativas[i] = 0;
    }
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
        ajudas->eliminadas[i] = 0;
    }
    ajudas->sugestaoEspecialista = '\0';
}

void inicializarAjudas(EstadoAjudas *ajudas){
    if (ajudas == NULL){
        return;
    }

    // Cada ajuda começa disponível e pode ser utilizada uma vez por partida.
    for (int i = 0; i < TOTAL_AJUDAS; i++){
        ajudas->usadas[i] = 0;
    }
    prepararAjudasPergunta(ajudas);
}

int obterChanceEspecialista(NivelDificuldade nivel){
    const int chances[] = {0, 90, 80, 70, 60, 50};
    if (nivel < MUITO_FACIL || nivel > MUITO_DIFICIL){
        return 0;
    }
    return chances[nivel];
}

ResultadoAjuda aplicarAjuda(EstadoAjudas *ajudas, const Pergunta *pergunta, TipoAjuda tipo){
    // Valida os parâmetros antes de acessar os vetores de controle.
    if (ajudas == NULL || pergunta == NULL || tipo < 0 || tipo >= TOTAL_AJUDAS ||
        pergunta->respostaCorreta < 'A' || pergunta->respostaCorreta > 'D' ||
        obterChanceEspecialista(pergunta->nivel) == 0){
        return AJUDA_INVALIDA;
    }
    if (ajudas->usadas[tipo]){
        return AJUDA_JA_USADA;
    }

    // Pistas ausentes não consomem o uso da ajuda.
    if (tipo == AJUDA_MISSAO || tipo == AJUDA_TELESCOPIO){
        if (obterTextoAjuda(pergunta, tipo) == NULL){
            return AJUDA_SEM_CONTEUDO;
        }
    } else {
        // Seleciona somente alternativas incorretas que continuam disponíveis.
        int incorretas[QUANT_ALTERNATIVAS];
        int quantidade = 0;
        int correta = pergunta->respostaCorreta - 'A';
        for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
            if (i != correta && !ajudas->eliminadas[i]){
                incorretas[quantidade++] = i;
            }
        }

        if (tipo == AJUDA_SUPERNOVA){
            if (quantidade < 2){
                return AJUDA_NAO_PERMITIDA;
            }

            // Sorteia duas incorretas sem reposição, preservando a correta.
            for (int i = 0; i < 2; i++){
                int sorteada = rand() % quantidade;
                ajudas->eliminadas[incorretas[sorteada]] = 1;
                incorretas[sorteada] = incorretas[--quantidade];
            }

            // Uma sugestão anterior deixa de valer se a alternativa foi eliminada.
            char sugestao = ajudas->sugestaoEspecialista;
            if (sugestao >= 'A' && sugestao <= 'D' && ajudas->eliminadas[sugestao - 'A']){
                ajudas->sugestaoEspecialista = '\0';
            }
        } else {
            // O especialista pode errar, mas nunca sugere uma alternativa eliminada.
            int acerta = rand() % 100 < obterChanceEspecialista(pergunta->nivel);
            int sugestao = acerta || quantidade == 0 ? correta : incorretas[rand() % quantidade];
            ajudas->sugestaoEspecialista = (char)('A' + sugestao);
        }
    }

    // Confirma o uso apenas depois de aplicar o efeito solicitado.
    ajudas->usadas[tipo] = 1;
    ajudas->ativas[tipo] = 1;
    return AJUDA_SUCESSO;
}
