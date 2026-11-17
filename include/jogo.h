/**
 * @file jogo.h
 * @brief Estrutura e inicializacao de uma partida independente da interface.
 */
#ifndef JOGO_H
#define JOGO_H

#include "pergunta.h"
#include "cadastro.h"
#include "ajuda.h"

#define TOTAL_PERGUNTAS_PARTIDA 15

/** @brief Situacoes possiveis de uma partida. */
typedef enum {
    PARTIDA_NAO_INICIADA = 0,
    PARTIDA_EM_ANDAMENTO,
    PARTIDA_VITORIA,
    PARTIDA_DERROTA,
    PARTIDA_DESISTENCIA
} EstadoPartida;

/** @brief Dados de uma partida, sem dependencia de terminal ou Raylib. */
typedef struct {
    Pergunta perguntas[TOTAL_PERGUNTAS_PARTIDA];
    int indiceAtual; /**< Indice de 0 a 14, valido durante a partida. */
    int acertos;
    int premioAtual; /**< Valor inteiro em reais. */
    int premioGarantido; /**< Valor inteiro protegido pelo ultimo marco. */
    EstadoPartida estado;
    EstadoAjudas ajudas;
} Partida;

/**
 * @brief Prepara ou reinicializa uma partida, sem sortear perguntas.
 * @param partida Partida que sera inicializada. NULL e ignorado.
 * @note Apaga todos os dados de uma partida anterior.
 */
void inicializarPartida(Partida *partida);

/** @brief Resultados da preparação de uma partida. */
typedef enum {
    INICIO_SUCESSO = 0,
    INICIO_PARAMETRO_INVALIDO,
    INICIO_CADASTRO_INVALIDO,
    INICIO_PERGUNTAS_INSUFICIENTES
} ResultadoInicio;

/**
 * @brief Seleciona as 15 perguntas e inicia uma partida.
 *
 * Sorteia sem repetir registros e sem modificar o cadastro recebido.
 * A distribuição por dificuldade é 2, 2, 4, 4 e 3, nessa ordem.
 *
 * @param partida Ponteiro para a partida que receberá as perguntas.
 * @param cadastro Cadastro já carregado pela aplicação, com IDs únicos e positivos.
 * @return Resultado da preparação. Em caso de falha, preserva a partida.
 * @note A aplicação deve chamar srand uma única vez ao iniciar o programa.
 */
ResultadoInicio iniciarPartida(Partida *partida, const Cadastro *cadastro);

/** @brief Resultado de uma tentativa de resposta. */
typedef enum {
    RESPOSTA_INVALIDA = 0,
    RESPOSTA_CORRETA,
    RESPOSTA_INCORRETA,
    RESPOSTA_VITORIA,
    RESPOSTA_NAO_PERMITIDA
} ResultadoResposta;

/** @brief Retorna a pergunta atual, ou NULL se a partida não estiver em andamento. */
const Pergunta *obterPerguntaAtual(const Partida *partida);

/**
 * @brief Consulta o prêmio da pergunta indicada.
 * @param numero Número da pergunta, de 1 a 15.
 * @return Valor em reais, ou zero para um número inválido.
 */
int obterPremioPergunta(int numero);

/**
 * @brief Valida A-D (maiúscula ou minúscula), aplica o prêmio e avança após acerto.
 * @param partida Partida em andamento.
 * @param resposta Letra escolhida pelo jogador.
 * @return Resultado da ação. Entradas inválidas não alteram a partida.
 */
ResultadoResposta responderPergunta(Partida *partida, char resposta);

/**
 * @brief Encerra a partida preservando o prêmio atual.
 * @return 1 se a desistência foi registrada; 0 se a ação não for permitida.
 */
int desistirPartida(Partida *partida);

/** @brief Usa uma ajuda na pergunta atual, sem avançar ou alterar o prêmio. */
ResultadoAjuda usarAjuda(Partida *partida, TipoAjuda tipo);

#endif
