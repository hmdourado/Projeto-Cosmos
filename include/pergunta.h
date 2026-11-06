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

//Estrutura que será utilizada para formulação de uma pergunata
typedef struct {
    int id;
    char enunciado[TAM_ENUNCIADO];
    char alternativas[QUANT_ALTERNATIVAS][TAM_ALTERNATIVA];
    char respostaCorreta;
    NivelDificuldade nivel;
} Pergunta;

 //Fim proteção de inclusão
#endif