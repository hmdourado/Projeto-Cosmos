
/**
 * @file arquivo.c
 * @author Henrique Dourado
 * @brief Implementação das operações de leitura e escrita de arquivos.
 * @version 0.1
 * @date 2026-11-16
 *
 * @copyright Copyright (c) 2026
 */

#include "../include/arquivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a quantidade máxima de caracteres armazenados em uma linha do CSV.
#define TAM_LINHA_CSV 1024

/**
 * @brief Interpreta uma linha do arquivo CSV e preenche uma estrutura Pergunta.
 *
 * @param linha String contendo os dados da pergunta separados por vírgulas.
 * @param pergunta Ponteiro para a estrutura que receberá os dados.
 * @return 1 se a interpretação e validação forem realizadas com sucesso; 0 caso contrário.
 */
static int interpretarLinhaCSV(char *linha, Pergunta *pergunta){
    // Verifica se os ponteiros recebidos são válidos.
    if (linha == NULL || pergunta == NULL){
        return 0;
    }

    // Inicializa a estrutura da pergunta, definindo seus valores iniciais.
    inicializarPergunta(pergunta);

    // Remove os caracteres de quebra de linha (\n ou \r) do final da string.
    linha[strcspn(linha, "\r\n")] = '\0';

    // Obtém o primeiro campo da linha, correspondente ao identificador da pergunta.
    char *campo = strtok(linha, ",");

    // Verifica se o campo foi encontrado.
    if (campo == NULL){
        return 0;
    }

    // Converte o identificador de texto para inteiro.
    pergunta->id = atoi(campo);

    // Obtém o segundo campo, correspondente ao nível de dificuldade.
    campo = strtok(NULL, ",");

    // Verifica se o campo foi encontrado.
    if (campo == NULL){
        return 0;
    }

    // Converte o nível de dificuldade para inteiro e realiza a conversão para o tipo enumerado.
    pergunta->nivel = (NivelDificuldade)atoi(campo);

    // Obtém o terceiro campo, correspondente ao enunciado da pergunta.
    campo = strtok(NULL, ",");

    // Verifica se o campo existe e se seu conteúdo cabe no array de destino.
    if (campo == NULL || strlen(campo) >= TAM_ENUNCIADO){
        return 0;
    }

    // Copia o enunciado para a estrutura da pergunta.
    strcpy(pergunta->enunciado, campo);

    // Percorre as quatro alternativas da pergunta.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){

        // Obtém o próximo campo correspondente a uma alternativa.
        campo = strtok(NULL, ",");

        // Verifica se o campo existe e se seu conteúdo cabe no array de destino.
        if (campo == NULL || strlen(campo) >= TAM_ALTERNATIVA){
            return 0;
        }

        // Copia a alternativa para a posição correspondente no array.
        strcpy(pergunta->alternativas[i], campo);
    }

    // Obtém o último campo, correspondente à resposta correta.
    campo = strtok(NULL, ",");

    // Verifica se o campo existe e contém exatamente um caractere.
    if (campo == NULL || strlen(campo) != 1){
        return 0;
    }

    // Armazena o caractere da resposta correta na estrutura.
    pergunta->respostaCorreta = campo[0];

    // Verifica se todos os dados preenchidos atendem aos critérios de validação.
    return validarPergunta(pergunta);
}

int carregarCSV(Cadastro *cadastro, const char *caminho){
    // Verifica se os parâmetros recebidos são válidos.
    if (cadastro == NULL || cadastro == NULL){
        return 0;
    }

    // Abre o arquivo no modo de leitura.
    FILE *arquivo = fopen(caminho, "r");

    // Verifica se o arquivo foi aberto corretamente.
    if (arquivo == NULL){
        return 0;
    }

    // Declara o buffer utilizado para armazenar temporariamente cada linha do CSV.
    char linha[TAM_LINHA_CSV];

    // Realiza a leitura do cabeçalho para que ele não seja interpretado como uma pergunta.
    if (fgets(linha, sizeof(linha), arquivo) == NULL){
        fclose(arquivo);
        return 0;
    }

    // Percorre todas as linhas restantes do arquivo até atingir o final.
    while (fgets(linha, sizeof(linha), arquivo) != NULL){

        // Declara uma estrutura temporária para armazenar os dados da pergunta atual.
        Pergunta pergunta;

        // Interpreta os campos da linha e verifica se os dados são válidos.
        if (!interpretarLinhaCSV(linha, &pergunta)){
            fclose(arquivo);
            return 0;
        }

        // Adiciona a pergunta interpretada ao cadastro, utilizando o vetor dinâmico.
        if (!adicionarPergunta(cadastro, &pergunta)){
            fclose(arquivo);
            return 0;
        }
    }

    // Fecha o arquivo após finalizar a leitura dos registros.
    fclose(arquivo);

    // Retorna 1 indicando que o carregamento foi concluído.
    return 1;
}

int salvarBinario(const Cadastro *cadastro, const char *caminho){
    // Verifica se os parâmetros recebidos são válidos.
    if (cadastro == NULL || caminho == NULL ||
        (cadastro->quantidade > 0 && cadastro->perguntas == NULL)){
        return 0;
    }

    // Abre o arquivo no modo de escrita.
    FILE *arquivo = fopen(caminho, "wb");

    // Verifica se o arquivo foi aberto corretamente.
    if (arquivo == NULL){
        return 0;
    }

    if ((fwrite(&cadastro->quantidade, sizeof(size_t), 1, arquivo)) != 1){
        // Fechar arquivo
        fclose(arquivo);

        return 0;
    }

    for (size_t i = 0; i < cadastro->quantidade; i++){
        if ((fwrite(&cadastro->perguntas[i], sizeof(Pergunta), 1, arquivo)) != 1){
            // Fechar arquivo
            fclose(arquivo);

            return 0;
        }
    }
    fclose(arquivo);

    return 1;
}

int carregarBinario(Cadastro *cadastro, const char *caminho){
    // Verifica se os parâmetros recebidos são válidos.
    if (cadastro == NULL || caminho == NULL){
        return 0;
    }

    // Abre o arquivo no modo de leitura binária.
    FILE *arquivo = fopen(caminho, "rb");

    // Verifica se o arquivo foi aberto corretamente.
    if (arquivo == NULL){
        return 0;
    }

    // Variável utilizada para armazenar a quantidade de perguntas.
    size_t novaQuantidade;

    // Recupera a quantidade de perguntas armazenada no arquivo.
    if (fread(&novaQuantidade, sizeof(size_t), 1, arquivo) != 1){
        fclose(arquivo);
        return 0;
    }

    // Percorre a quantidade de perguntas armazenadas.
    for (size_t i = 0; i < novaQuantidade; i++){

        // Estrutura temporária para armazenar a pergunta recuperada.
        Pergunta pergunta;

        // Lê uma estrutura Pergunta do arquivo binário.
        if (fread(&pergunta, sizeof(Pergunta), 1, arquivo) != 1){
            fclose(arquivo);
            return 0;
        }

        // Valida os dados da pergunta recuperada.
        if (!validarPergunta(&pergunta)){
            fclose(arquivo);
            return 0;
        }

        // Adiciona a pergunta recuperada ao cadastro.
        if (!adicionarPergunta(cadastro, &pergunta)){
            fclose(arquivo);
            return 0;
        }
    }

    // Fecha o arquivo após concluir a leitura.
    fclose(arquivo);

    return 1;
}

int exportarCSV(const Cadastro *cadastro, const char *caminho){
    // Verifica se os parâmetros recebidos são válidos.
    if (cadastro == NULL || caminho == NULL ||
        (cadastro->quantidade > 0 && cadastro->perguntas == NULL)){
        return 0;
    }

    // Abre o arquivo no modo de escrita.
    FILE *arquivo = fopen(caminho, "w");

    // Verifica se o arquivo foi aberto corretamente.
    if (arquivo == NULL){
        return 0;
    }

    // Escreve o cabeçalho do arquivo CSV.
    if (fprintf(arquivo,
        "id,nivel,enunciado,alternativaA,alternativaB,"
        "alternativaC,alternativaD,respostaCorreta\n") < 0){

        // Fecha o arquivo em caso de falha.
        fclose(arquivo);

        return 0;
    }

    // Percorre todas as perguntas armazenadas no cadastro.
    for (size_t i = 0; i < cadastro->quantidade; i++){

        // Obtém a pergunta atual do vetor.
        const Pergunta *pergunta = &cadastro->perguntas[i];

        // Escreve os dados da pergunta no formato CSV.
        if (fprintf(arquivo, "%d,%d,%s,%s,%s,%s,%s,%c\n",
            pergunta->id,
            pergunta->nivel,
            pergunta->enunciado,
            pergunta->alternativas[0],
            pergunta->alternativas[1],
            pergunta->alternativas[2],
            pergunta->alternativas[3],
            pergunta->respostaCorreta) < 0){

            // Fecha o arquivo em caso de falha.
            fclose(arquivo);

            return 0;
        }
    }

    // Fecha o arquivo após concluir a exportação.
    fclose(arquivo);

    return 1;
}
