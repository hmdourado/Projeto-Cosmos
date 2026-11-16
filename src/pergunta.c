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
#include <stddef.h>
#include <string.h>

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

int validarPergunta(Pergunta *p) {
  //Verificação para evitar acesssar estrutura inexistente.
  if (p == NULL){
    return 0;
  }

  //strlen() conta os caracteres de p-> enunciado e atribui o valor a 'valido'.
  size_t valido = strlen(p->enunciado);

  //Verifica se enunciado possui caracteres validos.
  if (valido == 0){
    return 0;
  } 

  //Percorre as alternativas e verifica se está valida
  for (size_t i = 0; i < QUANT_ALTERNATIVAS; i++){
    if (strlen(p->alternativas[i]) == 0){
      return 0;
    }
  }

  //Verifica se a resposta selecionada está condizente com as possibilidades.
  if (p->respostaCorreta != 'A' && 
      p->respostaCorreta != 'B' && 
      p->respostaCorreta != 'C' && 
      p->respostaCorreta != 'D'){

    return 0;
  } 

  //Verifica se o nível está no intervalo correto.
  if (p->nivel < MUITO_FACIL || p->nivel > MUITO_DIFICIL){
    return 0;
  }

  //Retorno da função após verificações.
  return 1;
}

char *obterNomeNivel(NivelDificuldade nivel){
  //Estrutura switch para transforma nível númerico recebido por enum em string.
  switch (nivel){
    case NAO_DEFINIDO:
      return "Não definido";
    
    case MUITO_FACIL:
      return "Muito fácil";

    case FACIL:
      return "Fácil";
    
    case MEDIO:
      return "Médio";

    case DIFICIL:
      return "Difícil";

    case MUITO_DIFICIL:
      return "Muito Difícil";

    default:
      return "Nível inválido";
  }
}

char obterNomeAlternativa(int indice){
  //Verifica cada condição e retorna seu valor determinado.
  if (indice >= 0 && indice <= 3){
    if (indice == 0){
      return 'A';

    } else if (indice == 1){
      return 'B';

    } else if (indice == 2){
      return 'C';

    } else if (indice == 3){
      return 'D';
    } 
  }

  return '\0';
}

void exibirPergunta(Pergunta *p){
  if (p == NULL) {
        return;
    }

    //Exibição de nível
    printf("\n========================================\n");
    printf("Nível: %s\n\n", obterNomeNivel(p->nivel));

    //Exibição do enunciado
    printf("%s\n\n", p->enunciado);

    //Exibição de alternativas
    for (size_t i = 0; i < QUANT_ALTERNATIVAS; i++) {
        printf("%c) %s\n",
               obterNomeAlternativa(i),
               p->alternativas[i]);
    }

    printf("========================================\n");
}