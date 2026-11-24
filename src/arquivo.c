
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
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/** Lê um campo e devolve o separador que o encerrou. */
static int lerCampoCSV(FILE *arquivo, char *campo, size_t capacidade, int *separador){
    size_t tamanho = 0;
    int caractere = fgetc(arquivo);

    if (caractere == '"'){
        for (;;){
            caractere = fgetc(arquivo);
            if (caractere == EOF || caractere == '\0'){
                return 0;
            }
            if (caractere == '"'){
                caractere = fgetc(arquivo);
                if (caractere != '"'){
                    break;
                }
            }
            if (tamanho == capacidade - 1){
                return 0;
            }
            campo[tamanho++] = (char)caractere;
        }
    } else {
        while (caractere != ',' && caractere != '\n' && caractere != '\r' && caractere != EOF){
            if (tamanho == capacidade - 1 || caractere == '\0'){
                return 0;
            }
            campo[tamanho++] = (char)caractere;
            caractere = fgetc(arquivo);
        }
    }

    campo[tamanho] = '\0';
    *separador = caractere;
    return !ferror(arquivo);
}

static int lerInteiroCSV(FILE *arquivo, int *valor){
    char campo[sizeof(int) * CHAR_BIT + 2];
    int separador;
    if (!lerCampoCSV(arquivo, campo, sizeof(campo), &separador) || separador != ','){
        return 0;
    }
    char *fim;
    errno = 0;
    long numero = strtol(campo, &fim, 10);
    if (fim == campo || *fim != '\0' || errno == ERANGE || numero < INT_MIN || numero > INT_MAX){
        return 0;
    }
    *valor = (int)numero;
    return 1;
}

/**
 * @brief Interpreta um registro do arquivo CSV e preenche uma estrutura Pergunta.
 *
 * @param arquivo Arquivo posicionado no início do registro.
 * @param pergunta Ponteiro para a estrutura que receberá os dados.
 * @return 1 se a interpretação e validação forem realizadas com sucesso; 0 caso contrário.
 */
static int lerPerguntaCSV(FILE *arquivo, Pergunta *pergunta){
    // Inicializa a estrutura da pergunta, definindo seus valores iniciais.
    inicializarPergunta(pergunta);
    int nivel;
    int separador;
    // Lê o identificador, o nível de dificuldade e o enunciado.
    if (!lerInteiroCSV(arquivo, &pergunta->id) || !lerInteiroCSV(arquivo, &nivel) ||
        nivel < MUITO_FACIL || nivel > MUITO_DIFICIL ||
        !lerCampoCSV(arquivo, pergunta->enunciado, sizeof(pergunta->enunciado), &separador) ||
        separador != ','){
        return 0;
    }
    pergunta->nivel = (NivelDificuldade)nivel;

    // Percorre as quatro alternativas da pergunta.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
        if (!lerCampoCSV(arquivo, pergunta->alternativas[i], sizeof(pergunta->alternativas[i]), &separador) ||
            separador != ','){
            return 0;
        }
    }

    // Obtém o último campo, correspondente à resposta correta.
    char resposta[2];
    if (!lerCampoCSV(arquivo, resposta, sizeof(resposta), &separador)){
        return 0;
    }
    if (separador == '\r'){
        separador = fgetc(arquivo);
    }
    if (separador != '\n' && separador != EOF){
        return 0;
    }
    // Armazena o caractere da resposta correta na estrutura.
    pergunta->respostaCorreta = resposta[0];

    // Verifica se todos os dados preenchidos atendem aos critérios de validação.
    return validarPergunta(pergunta);
}

int carregarCSV(Cadastro *cadastro, const char *caminho){
    // Verifica se os parâmetros recebidos são válidos.
    if (cadastro == NULL || caminho == NULL){
        return 0;
    }
    // Abre o arquivo no modo de leitura.
    FILE *arquivo = fopen(caminho, "rb");

    // Verifica se o arquivo foi aberto corretamente.
    if (arquivo == NULL){
        return 0;
    }

    // Realiza a leitura do cabeçalho para que ele não seja interpretado como uma pergunta.
    int caractere = fgetc(arquivo);
    int sucesso = caractere != EOF;
    while (caractere != '\n' && caractere != EOF){
        caractere = fgetc(arquivo);
    }

    size_t quantidadeOriginal = cadastro->quantidade;

    // Percorre todos os registros restantes do arquivo até atingir o final.
    while (sucesso && (caractere = fgetc(arquivo)) != EOF){
        // Declara uma estrutura temporária para armazenar os dados da pergunta atual.
        Pergunta pergunta;

        // Interpreta os campos e adiciona a pergunta ao cadastro se os dados forem válidos.
        sucesso = ungetc(caractere, arquivo) != EOF &&
            lerPerguntaCSV(arquivo, &pergunta) &&
            adicionarPergunta(cadastro, &pergunta);
    }

    // Fecha o arquivo após finalizar a leitura dos registros.
    sucesso = sucesso && !ferror(arquivo);
    if (fclose(arquivo) != 0 || !sucesso){
        cadastro->quantidade = quantidadeOriginal;
        return 0;
    }

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

/** Escreve um campo textual entre aspas, duplicando aspas literais. */
static int escreverCampoCSV(FILE *arquivo, const char *campo){
    if (fputc('"', arquivo) == EOF){
        return 0;
    }
    for (; *campo != '\0'; campo++){
        if ((*campo == '"' && fputc('"', arquivo) == EOF) ||
            fputc((unsigned char)*campo, arquivo) == EOF){
            return 0;
        }
    }
    return fputs("\",", arquivo) != EOF;
}

int exportarCSV(const Cadastro *cadastro, const char *caminho){
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
        int sucesso = fprintf(arquivo, "%d,%d,", pergunta->id, pergunta->nivel) >= 0 &&
            escreverCampoCSV(arquivo, pergunta->enunciado);
        for (int j = 0; sucesso && j < QUANT_ALTERNATIVAS; j++){
            sucesso = escreverCampoCSV(arquivo, pergunta->alternativas[j]);
        }
        if (!sucesso || fprintf(arquivo, "%c\n", pergunta->respostaCorreta) < 0){

            // Fecha o arquivo em caso de falha.
            fclose(arquivo);

            return 0;
        }
    }

    // Fecha o arquivo após concluir a exportação.
    return fclose(arquivo) == 0;
}
