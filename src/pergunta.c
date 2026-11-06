/**
 * @file pergunta.c
 * @author Henrique Dourado
 * @brief 
 * @version 0.1
 * @date 2026-11-06
 * 
 * @copyright Copyright (c) 2026
 * 
 * Funções responsáveis por operações básicas sobre uma pergunta
 * 
 */

 //Importação de estrtura de uma pergunta
#include "../include/pergunta.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Inicializar os campos de uma pergunta.
 * 
 * @param p 
 */
void inicializarPergunta (Pergunta *p){
    //Verificação para evitar acesssar estrutura inexistente.
      if (p == NULL) {
        return;
    }

    //ID começa em 0 indicando que não foi atribuido.
    p->id = 0;

    //Utilização da função memset() para iniciar structs e vetores referentes as perguntas.
    memset(p->enunciado, '\0', sizeof(p->enunciado));
    memset(p->alternativas, 0 , sizeof(p->alternativas));

    //Caractere nulo, pois a pergunta ainda não possui resposta cadastrada.
    p->respostaCorreta = '\0';

    //Nivel iniciando  como não definido ainda.
    p->nivel = NAO_DEFINIDO;
}
