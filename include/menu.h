/**
 * @file menu.h
 * @brief Contrato de integração com o módulo de interface.
 */
#ifndef MENU_H
#define MENU_H

#include "cadastro.h"

/**
 * @brief Executa o menu e retorna quando o usuário encerra o programa.
 * @param cadastro Cadastro carregado, disponível para consulta e alteração.
 * @note A persistência e a liberação do cadastro pertencem ao main.
 */
void executarMenu(Cadastro *cadastro);

#endif
