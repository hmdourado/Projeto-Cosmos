/**
 * @file pergunta.h
 * @author Henrique Dourado 
 * @brief 
 * @version 0.1
 * @date 2026-11-06
 * 
 * @copyright Copyright (c) 2026
 * 
 * Estruturas para perguntas que serão utilizadas no jogo,
 * com proteção de inclusão para casos de arquivos .c que utilizarem as estruras e declarações.
 * 
 */

 //Proteção de inclusão
#ifndef PERGUNTA_H
#define PERGUNTA_H

// Defição de tamanhos e quantidades, desse modo caso seja necessário a alteração ficará mais fácil de dar a manutenção.
#define ENUNCIADO_TAM 300
#define ALTERNATIVA_TAM 150
#define QUANT_ALTERNATIVAS 4

//Estrutura de niveis de perguntas
typedef enum {
    MUITO_FACIL = 1,
    FACIL,
    MEDIO,
    DIFICIL,
    MUITO_DIFICIL,
} NivelDificuldade;

//Estrutura que será utilizada para formulação de uma pergunata
typedef struct {
    int id;
    char enunciado[ENUNCIADO_TAM];
    char alternativas[QUANT_ALTERNATIVAS][ALTERNATIVA_TAM];
    NivelDificuldade nivel;
} Pergunta;

 //Fim proteção de inclusão
#endif