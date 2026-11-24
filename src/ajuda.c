/**
 * @file ajuda.c
 * @brief Implementação dos limites de uso e efeitos das ajudas.
 */
#include "../include/ajuda.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Centraliza as chances de acerto do especialista por nível de dificuldade.
static const int CHANCES_ESPECIALISTA[MUITO_DIFICIL + 1] = {
    0, 90, 80, 70, 60, 50
};

// Fornece orientações de estratégia sem revelar diretamente a resposta.
static const char *const PISTAS_MISSAO[MUITO_DIFICIL + 1] = {
    NULL,
    "Missao: identifique a palavra-chave do enunciado e associe-a ao conceito principal.",
    "Missao: concentre-se na caracteristica central do objeto ou fenomeno citado.",
    "Missao: relacione as informacoes do enunciado antes de escolher uma alternativa.",
    "Missao: compare as propriedades descritas e procure a opcao compativel com todas elas.",
    "Missao: divida o enunciado em condicoes e descarte hipoteses que contrariem alguma delas."
};

// Fornece uma segunda orientação para apoiar a comparação das alternativas.
static const char *const PISTAS_TELESCOPIO[MUITO_DIFICIL + 1] = {
    NULL,
    "Telescopio: observe o termo central da pergunta e compare-o com as alternativas.",
    "Telescopio: procure a caracteristica que melhor corresponde ao tema apresentado.",
    "Telescopio: compare os conceitos envolvidos e elimine as alternativas incompatíveis.",
    "Telescopio: observe as diferencas entre os fenomenos ou objetos mencionados.",
    "Telescopio: avalie cada alternativa considerando todas as restricoes do enunciado."
};

// Retorna o índice da alternativa correta após validar sua letra.
static int obterIndiceRespostaCorreta(const Pergunta *pergunta) {

    if (pergunta == NULL ||
        pergunta->respostaCorreta < 'A' ||
        pergunta->respostaCorreta > 'D') {
        return -1;
    }

    return pergunta->respostaCorreta - 'A';
}

// Limpa os efeitos associados à pergunta atual sem restaurar ajudas já utilizadas.
void prepararAjudasPergunta(EstadoAjudas *ajudas) {

    if (ajudas == NULL) {
        return;
    }

    // Remove os efeitos temporários de todas as ajudas.
    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        ajudas->ativas[i] = 0;
    }

    // Restaura as alternativas para a nova pergunta.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        ajudas->eliminadas[i] = 0;
    }

    // Descarta a sugestão produzida pelo especialista na pergunta anterior.
    ajudas->sugestaoEspecialista = '\0';
}

// Inicializa o controle de uso e os efeitos de todas as ajudas da partida.
void inicializarAjudas(EstadoAjudas *ajudas) {

    if (ajudas == NULL) {
        return;
    }

    // Todas as ajudas começam disponíveis para uma utilização na partida.
    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        ajudas->usadas[i] = 0;
        ajudas->ativas[i] = 0;
    }

    prepararAjudasPergunta(ajudas);
}

// Retorna a probabilidade de o especialista indicar a alternativa correta.
int obterChanceEspecialista(NivelDificuldade nivel) {

    if (nivel < MUITO_FACIL || nivel > MUITO_DIFICIL) {
        return 0;
    }

    return CHANCES_ESPECIALISTA[nivel];
}

// Retorna uma orientação estática conforme a ajuda e a dificuldade da pergunta.
const char *obterTextoAjuda(const Pergunta *pergunta, TipoAjuda tipo) {

    // Impede consultas com ponteiros ou tipos inválidos.
    if (pergunta == NULL || tipo < AJUDA_SUPERNOVA || tipo >= TOTAL_AJUDAS) {
        return NULL;
    }

    if (pergunta->nivel < MUITO_FACIL || pergunta->nivel > MUITO_DIFICIL) {
        return NULL;
    }

    // Seleciona a orientação correspondente à ajuda textual solicitada.
    switch (tipo) {

        case AJUDA_MISSAO:
            return PISTAS_MISSAO[pergunta->nivel];

        case AJUDA_TELESCOPIO:
            return PISTAS_TELESCOPIO[pergunta->nivel];

        default:
            return NULL;
    }
}

// Aplica o efeito solicitado e consome o uso somente após sua execução bem-sucedida.
ResultadoAjuda aplicarAjuda(
    EstadoAjudas *ajudas,
    const Pergunta *pergunta,
    TipoAjuda tipo
) {

    // Valida os parâmetros antes de acessar os vetores de controle.
    if (ajudas == NULL || pergunta == NULL ||
        tipo < AJUDA_SUPERNOVA || tipo >= TOTAL_AJUDAS ||
        obterIndiceRespostaCorreta(pergunta) < 0 ||
        obterChanceEspecialista(pergunta->nivel) == 0) {
        return AJUDA_INVALIDA;
    }

    // Impede que uma ajuda seja utilizada mais de uma vez na mesma partida.
    if (ajudas->usadas[tipo]) {
        return AJUDA_JA_USADA;
    }

    // As ajudas textuais não consomem uso quando não há orientação disponível.
    if (tipo == AJUDA_MISSAO || tipo == AJUDA_TELESCOPIO) {

        if (obterTextoAjuda(pergunta, tipo) == NULL) {
            return AJUDA_SEM_CONTEUDO;
        }

    } else {

        int incorretas[QUANT_ALTERNATIVAS];
        int quantidade = 0;
        int correta = obterIndiceRespostaCorreta(pergunta);

        // Identifica somente as alternativas incorretas ainda disponíveis.
        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
            if (i != correta && !ajudas->eliminadas[i]) {
                incorretas[quantidade++] = i;
            }
        }

        if (tipo == AJUDA_SUPERNOVA) {

            // A supernova precisa encontrar duas alternativas incorretas disponíveis.
            if (quantidade < 2) {
                return AJUDA_NAO_PERMITIDA;
            }

            // Sorteia duas alternativas sem reposição e preserva a resposta correta.
            for (int i = 0; i < 2; i++) {
                int sorteada = rand() % quantidade;
                ajudas->eliminadas[incorretas[sorteada]] = 1;
                incorretas[sorteada] = incorretas[quantidade - 1];
                quantidade--;
            }

            // Invalida uma sugestão anterior caso a alternativa sugerida tenha sido eliminada.
            int indiceSugestao = ajudas->sugestaoEspecialista - 'A';
            if (indiceSugestao >= 0 && indiceSugestao < QUANT_ALTERNATIVAS &&
                ajudas->eliminadas[indiceSugestao]) {
                ajudas->sugestaoEspecialista = '\0';
            }

        } else if (tipo == AJUDA_ESPECIALISTA) {

            // O especialista pode errar conforme a dificuldade, mas não sugere opção eliminada.
            int chance = obterChanceEspecialista(pergunta->nivel);
            bool acerta = rand() % 100 < chance;
            int sugestao = correta;

            if (!acerta && quantidade > 0) {
                sugestao = incorretas[rand() % quantidade];
            }

            ajudas->sugestaoEspecialista = (char)('A' + sugestao);
        }
    }

    // Registra o uso e ativa o efeito somente depois da aplicação bem-sucedida.
    ajudas->usadas[tipo] = 1;
    ajudas->ativas[tipo] = 1;

    return AJUDA_SUCESSO;
}
