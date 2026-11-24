/**
 * @file ajuda.c
 * @brief Implementacao das regras e dos efeitos das ajudas da partida.
 */
#include "../include/ajuda.h"
#include "../include/pistas.h"

#include <stdbool.h>
#include <stdlib.h>

/* Chance percentual de o especialista indicar a resposta correta por nivel. */
static const int CHANCES_ESPECIALISTA[MUITO_DIFICIL + 1] = {
    0, 90, 80, 70, 60, 50
};

/**
 * @brief Converte a letra da resposta correta em indice de alternativa.
 * @param pergunta Pergunta que sera validada.
 * @return Indice entre 0 e QUANT_ALTERNATIVAS - 1 ou -1 se invalida.
 */
static int obterIndiceRespostaCorreta(const Pergunta *pergunta) {
    if (pergunta == NULL ||
        pergunta->respostaCorreta < 'A' ||
        pergunta->respostaCorreta >= 'A' + QUANT_ALTERNATIVAS) {
        return -1;
    }

    return pergunta->respostaCorreta - 'A';
}

/**
 * @brief Limpa os efeitos temporarios da pergunta atual.
 * @param ajudas Estado das ajudas da partida.
 *
 * Os usos consumidos sao preservados; somente efeitos visuais e temporarios
 * sao reiniciados para a proxima pergunta.
 */
void prepararAjudasPergunta(EstadoAjudas *ajudas) {
    if (ajudas == NULL) {
        return;
    }

    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        ajudas->ativas[i] = 0;
    }

    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        ajudas->eliminadas[i] = 0;
    }

    ajudas->sugestaoEspecialista = '\0';
}

/**
 * @brief Inicializa o estado das ajudas para uma nova partida.
 * @param ajudas Estado que sera inicializado.
 */
void inicializarAjudas(EstadoAjudas *ajudas) {
    if (ajudas == NULL) {
        return;
    }

    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        ajudas->usadas[i] = 0;
        ajudas->ativas[i] = 0;
    }

    prepararAjudasPergunta(ajudas);
}

/**
 * @brief Retorna a chance percentual de acerto do especialista.
 * @param nivel Nivel de dificuldade da pergunta.
 * @return Chance entre 50 e 90 ou zero para nivel invalido.
 */
int obterChanceEspecialista(NivelDificuldade nivel) {
    if (nivel < MUITO_FACIL || nivel > MUITO_DIFICIL) {
        return 0;
    }

    return CHANCES_ESPECIALISTA[nivel];
}

/**
 * @brief Retorna a pista especifica associada a uma pergunta.
 * @param pergunta Pergunta atual.
 * @param tipo Ajuda textual solicitada (Missao ou Telescopio).
 * @return Texto especifico ou NULL se nao houver correspondencia.
 *
 * A consulta valida tanto o ID quanto o enunciado. Assim, uma pergunta
 * alterada ou um ID reutilizado nao recebe uma pista de outro registro.
 */
const char *obterTextoAjuda(const Pergunta *pergunta, TipoAjuda tipo) {
    if (pergunta == NULL ||
        (tipo != AJUDA_MISSAO && tipo != AJUDA_TELESCOPIO)) {
        return NULL;
    }

    if (obterChanceEspecialista(pergunta->nivel) == 0) {
        return NULL;
    }

    return obterTextoPistaEspecifica(pergunta, tipo);
}

/**
 * @brief Aplica o efeito de uma ajuda e registra seu uso em caso de sucesso.
 * @param ajudas Estado de uso e efeitos das ajudas.
 * @param pergunta Pergunta atualmente apresentada.
 * @param tipo Tipo de ajuda solicitado.
 * @return Resultado da operacao, conforme ResultadoAjuda.
 *
 * A ajuda Missao apresenta uma orientacao especifica. A ajuda Telescopio
 * apresenta uma informacao complementar. Supernova elimina duas alternativas
 * incorretas sem reposicao. Especialista sugere uma alternativa de acordo com
 * a chance definida para o nivel da pergunta.
 */
ResultadoAjuda aplicarAjuda(
    EstadoAjudas *ajudas,
    const Pergunta *pergunta,
    TipoAjuda tipo
) {
    if (ajudas == NULL || pergunta == NULL ||
        tipo < AJUDA_SUPERNOVA || tipo >= TOTAL_AJUDAS ||
        obterIndiceRespostaCorreta(pergunta) < 0 ||
        obterChanceEspecialista(pergunta->nivel) == 0) {
        return AJUDA_INVALIDA;
    }

    if (ajudas->usadas[tipo]) {
        return AJUDA_JA_USADA;
    }

    /* Ajudas textuais so sao consumidas quando existe conteudo especifico. */
    if (tipo == AJUDA_MISSAO || tipo == AJUDA_TELESCOPIO) {
        if (obterTextoAjuda(pergunta, tipo) == NULL) {
            return AJUDA_SEM_CONTEUDO;
        }
    } else {
        int incorretas[QUANT_ALTERNATIVAS];
        int quantidadeIncorretas = 0;
        const int indiceCorreto = obterIndiceRespostaCorreta(pergunta);

        /* Monta a lista de alternativas incorretas ainda disponiveis. */
        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
            if (i != indiceCorreto && !ajudas->eliminadas[i]) {
                incorretas[quantidadeIncorretas++] = i;
            }
        }

        if (tipo == AJUDA_SUPERNOVA) {
            if (quantidadeIncorretas < 2) {
                return AJUDA_NAO_PERMITIDA;
            }

            /* Sorteia duas alternativas distintas, sem eliminar a correta. */
            for (int i = 0; i < 2; i++) {
                const int sorteio = rand() % quantidadeIncorretas;
                const int indiceEliminado = incorretas[sorteio];

                ajudas->eliminadas[indiceEliminado] = 1;

                /* Remove o item sorteado da lista sem deslocar o vetor. */
                incorretas[sorteio] = incorretas[quantidadeIncorretas - 1];
                quantidadeIncorretas--;
            }

            /* Uma sugestao anterior deixa de valer se a opcao foi eliminada. */
            if (ajudas->sugestaoEspecialista >= 'A' &&
                ajudas->sugestaoEspecialista < 'A' + QUANT_ALTERNATIVAS) {
                const int indiceSugestao = ajudas->sugestaoEspecialista - 'A';
                if (ajudas->eliminadas[indiceSugestao]) {
                    ajudas->sugestaoEspecialista = '\0';
                }
            }
        } else if (tipo == AJUDA_ESPECIALISTA) {
            const int chance = obterChanceEspecialista(pergunta->nivel);
            const bool acertou = (rand() % 100) < chance;
            int indiceSugerido = indiceCorreto;

            /* Se errar, escolhe uma incorreta que ainda nao foi eliminada. */
            if (!acertou && quantidadeIncorretas > 0) {
                indiceSugerido = incorretas[rand() % quantidadeIncorretas];
            }

            ajudas->sugestaoEspecialista = (char)('A' + indiceSugerido);
        } else {
            return AJUDA_INVALIDA;
        }
    }

    /* Consome e ativa a ajuda somente depois de concluir o efeito. */
    ajudas->usadas[tipo] = 1;
    ajudas->ativas[tipo] = 1;

    return AJUDA_SUCESSO;
}
