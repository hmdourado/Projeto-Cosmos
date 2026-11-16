
#ifndef CADASTRO_H
#define CADASTRO_H

#include "pergunta.h"
#include <stddef.h>

/**
 * @brief Estrutura responsável pelo gerenciamento dinâmico das perguntas.
 *
 * Armazena um vetor dinâmico de perguntas, controlando a quantidade
 * de elementos cadastrados e a capacidade de memória alocada.
 */
typedef struct {
    Pergunta *perguntas;
    size_t quantidade;
    size_t capacidade;
} Cadastro;

/**
 * @brief Inicializa um cadastro vazio.
 *
 * Define o ponteiro de perguntas como NULL e inicializa
 * a quantidade e a capacidade como zero.
 *
 * @param cadastro Ponteiro para a estrutura de cadastro.
 */
void inicializarCadastro(Cadastro *cadastro);

/**
 * @brief Adiciona uma pergunta ao cadastro.
 *
 * Verifica a validade da pergunta e a disponibilidade de espaço
 * no vetor dinâmico, realizando realocação quando necessário.
 *
 * @param cadastro Ponteiro para o cadastro.
 * @param pergunta Ponteiro para a pergunta que será adicionada.
 * @return 1 se a pergunta for adicionada com sucesso; 0 caso contrário.
 */
int adicionarPergunta(Cadastro *cadastro, const Pergunta *pergunta);

/**
 * @brief Busca uma pergunta pelo seu identificador.
 *
 * Percorre o cadastro procurando uma pergunta com o ID informado.
 *
 * @param cadastro Ponteiro para o cadastro.
 * @param id Identificador da pergunta procurada.
 * @return Ponteiro para a pergunta encontrada ou NULL caso não exista.
 */
Pergunta *buscarPerguntaPorId(Cadastro *cadastro, int id);

/**
 * @brief Atualiza uma pergunta existente no cadastro.
 *
 * Localiza a pergunta pelo ID informado e substitui seus dados
 * pelos dados da nova pergunta, preservando o identificador.
 *
 * @param cadastro Ponteiro para o cadastro.
 * @param id Identificador da pergunta que será atualizada.
 * @param novaPergunta Ponteiro para os novos dados da pergunta.
 * @return 1 se a atualização for realizada; 0 caso contrário.
 */
int atualizarPergunta(Cadastro *cadastro, int id,
                      const Pergunta *novaPergunta);

/**
 * @brief Remove uma pergunta do cadastro.
 *
 * Localiza a pergunta pelo ID informado e reorganiza o vetor
 * para preencher a posição liberada.
 *
 * @param cadastro Ponteiro para o cadastro.
 * @param id Identificador da pergunta que será removida.
 * @return 1 se a remoção for realizada; 0 caso contrário.
 */
int removerPergunta(Cadastro *cadastro, int id);

/**
 * @brief Libera a memória alocada para o cadastro.
 *
 * Libera o vetor dinâmico de perguntas e reinicializa
 * os campos da estrutura.
 *
 * @param cadastro Ponteiro para o cadastro.
 */
void liberarCadastro(Cadastro *cadastro);

#endif
