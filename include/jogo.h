/**
 * @file jogo.h
 * @brief Estrutura e inicializacao de uma partida independente da interface.
 */
#ifndef JOGO_H
#define JOGO_H

#include "pergunta.h"
#include "cadastro.h"

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

#endif
