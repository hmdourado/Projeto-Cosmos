/**
 * @file pergunta.h
 * @author Henrique Dourado 
 * @brief Estruturas para perguntas que serão utilizadas no jogo.
 * @version 0.1
 * @date 2026-11-06
 * 
 * @copyright Copyright (c) 2026
 * 
 * 
 */

 //Proteção de inclusão
#ifndef PERGUNTA_H
#define PERGUNTA_H

// Defição de tamanhos e quantidades, desse modo caso seja necessário a alteração ficará mais fácil de dar a manutenção.
#define TAM_ENUNCIADO 300
#define TAM_ALTERNATIVA 150
#define QUANT_ALTERNATIVAS 4

//Estrutura de niveis de perguntas
typedef enum {
    NAO_DEFINIDO = 0,
    MUITO_FACIL,
    FACIL,
    MEDIO,
    DIFICIL,
    MUITO_DIFICIL
} NivelDificuldade;

//Estrutura que será utilizada para formulação de uma pergunta
typedef struct {
    int id;
    char enunciado[TAM_ENUNCIADO];
    char alternativas[QUANT_ALTERNATIVAS][TAM_ALTERNATIVA];
    char respostaCorreta;
    NivelDificuldade nivel;
} Pergunta;

/**
 * @brief Inicializa todos os campos de uma pergunta com valores padrão.
 *
 * Define o ID como 0, limpa o enunciado e as alternativas,
 * inicializa a resposta correta como caractere nulo e define
 * o nível de dificuldade como NAO_DEFINIDO.
 *
 * @param p Ponteiro para a estrutura Pergunta que será inicializada.
 */
void inicializarPergunta (Pergunta *p);

/**
 * @brief Valida os dados de uma pergunta.
 * @param p Ponteiro para a pergunta que será validada.
 * @return 1 se a pergunta for válida; 0 caso contrário.
 */
int validarPergunta(Pergunta *p);

/**
 * @brief Retorna o nome correspondente ao nível de dificuldade.
 *
 * Converte um valor da enumeração NivelDificuldade
 * em sua representação textual.
 *
 * @param nivel Nível de dificuldade que será convertido.
 * @return Nome do nível de dificuldade ou "Nível inválido"
 * caso o valor não corresponda a uma opção válida.
 */
char *obterNomeNivel(NivelDificuldade nivel);

/**
 * @brief Retorna a letra correspondente ao índice de uma alternativa.
 *
 * Converte os índices de 0 a 3 nas letras A, B, C e D.
 *
 * @param indice Índice da alternativa.
 * @return Letra correspondente ou '\0' caso o índice seja inválido.
 */
char obterNomeAlternativa(int indice);

/**
 * @brief Exibe os dados de uma pergunta.
 *
 * Apresenta o enunciado, as alternativas e o nível de dificuldade.
 *
 * @param p Ponteiro constante para a pergunta que será exibida.
 */
void exibirPergunta(Pergunta *p);

 //Fim proteção de inclusão
#endif