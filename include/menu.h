
/**
 * @file menu.h
 * @author Integrante 2
 * @brief Definições e protótipos da interface textual (terminal).
 * @version 0.1
 * @date 2026-11-16
 *
 * @copyright Copyright (c) 2026
 *
 */
 
#ifndef MENU_H
#define MENU_H
 
#include "cadastro.h"
 
/**
 * @brief Executa a interface textual do sistema.
 *
 * Exibe o menu principal e o submenu de gerenciamento de perguntas,
 * permitindo cadastrar, listar, pesquisar (por ID ou por dificuldade),
 * alterar e excluir perguntas. Todas as operações sobre os dados são
 * realizadas pelas funções de cadastro.h.
 *
 * A função retorna quando o usuário escolhe "Sair" ou quando a entrada
 * padrão chega ao fim (EOF), evitando loops infinitos.
 *
 * @param cadastro Ponteiro para o cadastro que será manipulado.
 */
void executarMenu(Cadastro *cadastro);
 
#endif