/**
 * @file pistas.h
 * @brief Interface para consulta ao conteudo editorial especifico das perguntas.
 */
#ifndef PISTAS_H
#define PISTAS_H

#include "ajuda.h"

/**
 * @brief Busca uma pista pelo ID e pelo enunciado exato da pergunta.
 * @param pergunta Pergunta usada na consulta.
 * @param tipo AJUDA_MISSAO para a pista ou AJUDA_TELESCOPIO para a informacao.
 * @return Texto estatico correspondente ou NULL se nao houver correspondencia.
 */
const char *obterTextoPistaEspecifica(const Pergunta *pergunta, TipoAjuda tipo);

#endif
