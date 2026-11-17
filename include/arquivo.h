/**
 * @file arquivo.h
 * @author Henrique Dourado
 * @brief Definições e protótipos para operações com arquivos.
 * @version 0.1
 * @date 2026-11-16
 */

#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "cadastro.h"

/**
 * @brief Carrega perguntas de um arquivo CSV.
 *
 * @param cadastro Ponteiro para o cadastro que receberá as perguntas.
 * @param caminho Caminho do arquivo CSV.
 * @return 1 se a leitura for realizada com sucesso; 0 caso contrário.
 */
int carregarCSV(Cadastro *cadastro, const char *caminho);

/**
 * @brief Carrega perguntas armazenadas em um arquivo binário.
 *
 * @param cadastro Ponteiro para o cadastro que receberá as perguntas.
 * @param caminho Caminho do arquivo binário.
 * @return 1 se a leitura for realizada com sucesso; 0 caso contrário.
 */
int salvarBinario(const Cadastro *cadastro, const char *caminho);

/**
 * @brief Exporta as perguntas do cadastro para um arquivo CSV.
 *
 * @param cadastro Ponteiro constante para o cadastro.
 * @param caminho Caminho do arquivo CSV de destino.
 * @return 1 se a exportação for realizada com sucesso; 0 caso contrário.
 */

int carregarBinario(Cadastro *cadastro, const char *caminho);

/**
 * @brief Salva as perguntas do cadastro em um arquivo binário.
 *
 * @param cadastro Ponteiro constante para o cadastro.
 * @param caminho Caminho do arquivo binário.
 * @return 1 se a gravação for realizada com sucesso; 0 caso contrário.
 */
int exportarCSV(const Cadastro *cadastro, const char *caminho);

#endif
