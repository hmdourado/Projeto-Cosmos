/**
 * @file pergunta.h
 * @author Henrique Dourado 
 * @brief 
 * @version 0.1
 * @date 2026-11-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef PERGUNTA_H
#define PERGUNTA_H

#define ENUNCIADO_TAM 300
#define ALTERNATIVA_TAM 150
#define QUANT_ALTERNATIVAS 4

typedef enum {
    MUITO_FACIL = 1,
    FACIL,
    MEDIO,
    DIFICIL,
    MUITO_DIFICIL,
} NivelDificuldade;

typedef struct {
    int id;
    char enunciado[ENUNCIADO_TAM];
    char alternativas[QUANT_ALTERNATIVAS][ALTERNATIVA_TAM];
    NivelDificuldade nivel;
} Pergunta;

#endif