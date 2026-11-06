/**
 * @file pergunta.c
 * @author Henrique Dourado
 * @brief 
 * @version 0.1
 * @date 2026-11-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "../include/pergunta.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inicializarPergunta (Pergunta *p){
      if (p == NULL) {
        return;
    }

    p->id = 0;

    memset(p->enunciado, '\0', sizeof(p->enunciado));

    memset(p->alternativas, 0 , sizeof(p->alternativas));

    memset(p->nivel, 0 , sizeof(p->nivel));

}
