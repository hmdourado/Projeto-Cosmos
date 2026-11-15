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
 * @brief Inicializa todos os campos de uma pergunta com valores padrão.
 *
 * Define o ID como 0, limpa o enunciado e as alternativas,
 * inicializa a resposta correta como caractere nulo e define
 * o nível de dificuldade como NAO_DEFINIDO.
 *
 * @param p Ponteiro para a estrutura Pergunta que será inicializada.
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

/**
 * @brief Valida os dados de uma pergunta.
 * @param p Ponteiro para a pergunta que será validada.
 * @return 1 se a pergunta for válida; 0 caso contrário.
 */
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


/**
 * @brief Retorna a letra correspondente ao índice de uma alternativa.
 *
 * Converte os índices de 0 a 3 nas letras A, B, C e D.
 *
 * @param indice Índice da alternativa.
 * @return Letra correspondente ou '\0' caso o índice seja inválido.
 */
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