
/**
 * @file teste_arquivo.c
 * @author Henrique Dourado
 * @brief Testes completos das funções do módulo arquivo.
 */

#include "../include/arquivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Cria um arquivo CSV utilizado nos testes.
 *
 * @param caminho Caminho do arquivo CSV.
 * @return 1 se o arquivo for criado corretamente; 0 caso contrário.
 */
static int criarCSVTeste(const char *caminho){

    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL){
        return 0;
    }

    // Escreve o cabeçalho do CSV.
    fprintf(arquivo,
        "id,nivel,enunciado,alternativaA,alternativaB,"
        "alternativaC,alternativaD,respostaCorreta\n");

    // Escreve a primeira pergunta.
    fprintf(arquivo,
        "1,2,Qual planeta e conhecido como planeta vermelho?,"
        "Venus,Marte,Jupiter,Saturno,B\n");

    // Escreve a segunda pergunta.
    fprintf(arquivo,
        "2,3,Qual e a estrela mais proxima da Terra?,"
        "Sirius,Proxima Centauri,Sol,Vega,C\n");

    fclose(arquivo);

    return 1;
}

/**
 * @brief Compara os dados de dois cadastros.
 *
 * @param cadastro1 Primeiro cadastro.
 * @param cadastro2 Segundo cadastro.
 * @return 1 se os cadastros forem iguais; 0 caso contrário.
 */
static int compararCadastros(const Cadastro *cadastro1,
                             const Cadastro *cadastro2){

    // Verifica se os cadastros existem.
    if (cadastro1 == NULL || cadastro2 == NULL){
        return 0;
    }

    // Verifica se possuem a mesma quantidade de perguntas.
    if (cadastro1->quantidade != cadastro2->quantidade){
        return 0;
    }

    // Percorre e compara as perguntas dos dois cadastros.
    for (size_t i = 0; i < cadastro1->quantidade; i++){

        const Pergunta *p1 = &cadastro1->perguntas[i];
        const Pergunta *p2 = &cadastro2->perguntas[i];

        // Compara os dados principais da pergunta.
        if (p1->id != p2->id ||
            p1->nivel != p2->nivel ||
            p1->respostaCorreta != p2->respostaCorreta ||
            strcmp(p1->enunciado, p2->enunciado) != 0){

            return 0;
        }

        // Compara as quatro alternativas.
        for (int j = 0; j < QUANT_ALTERNATIVAS; j++){

            if (strcmp(p1->alternativas[j],
                       p2->alternativas[j]) != 0){

                return 0;
            }
        }
    }

    return 1;
}

int main(){

    int totalTestes = 0;
    int testesAprovados = 0;

    Cadastro cadastroCSV;
    Cadastro cadastroBinario;
    Cadastro cadastroExportado;
    Cadastro cadastroVazio;

    // Inicializa todos os cadastros utilizados nos testes.
    inicializarCadastro(&cadastroCSV);
    inicializarCadastro(&cadastroBinario);
    inicializarCadastro(&cadastroExportado);
    inicializarCadastro(&cadastroVazio);

    printf("\n====================================\n");
    printf("       TESTES DO MODULO ARQUIVO\n");
    printf("====================================\n");

    // Cria o arquivo CSV utilizado nos testes.
    if (!criarCSVTeste("src/output/perguntas_teste.csv")){
        printf("\nFalha ao criar CSV de teste!\n");

        return 1;
    }

    // TESTE 1: Carregar perguntas de um CSV.
    printf("\nTESTE 1 - Carregar CSV\n");

    totalTestes++;

    if (carregarCSV(&cadastroCSV,
                    "src/output/perguntas_teste.csv") &&
        cadastroCSV.quantidade == 2){

        printf("APROVADO: CSV carregado corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Falha ao carregar CSV.\n");
    }

    // TESTE 2: Salvar cadastro em arquivo binário.
    printf("\nTESTE 2 - Salvar binario\n");

    totalTestes++;

    if (salvarBinario(&cadastroCSV,
                      "src/output/perguntas_teste.bin")){

        printf("APROVADO: Arquivo binario salvo.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Falha ao salvar binario.\n");
    }

    // TESTE 3: Carregar perguntas do arquivo binário.
    printf("\nTESTE 3 - Carregar binario\n");

    totalTestes++;

    if (carregarBinario(&cadastroBinario,
                        "src/output/perguntas_teste.bin") &&
        cadastroBinario.quantidade == 2){

        printf("APROVADO: Binario carregado corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Falha ao carregar binario.\n");
    }

    // TESTE 4: Comparar cadastro original com cadastro recuperado.
    printf("\nTESTE 4 - Comparar dados recuperados\n");

    totalTestes++;

    if (compararCadastros(&cadastroCSV, &cadastroBinario)){

        printf("APROVADO: Os dados sao identicos.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Os dados sao diferentes.\n");
    }

    // TESTE 5: Exportar cadastro para CSV.
    printf("\nTESTE 5 - Exportar CSV\n");

    totalTestes++;

    if (exportarCSV(&cadastroBinario,
                    "src/output/perguntas_exportadas.csv")){

        printf("APROVADO: CSV exportado corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Falha ao exportar CSV.\n");
    }

    // TESTE 6: Carregar novamente o CSV exportado.
    printf("\nTESTE 6 - Recarregar CSV exportado\n");

    totalTestes++;

    if (carregarCSV(&cadastroExportado,
                    "src/output/perguntas_exportadas.csv") &&
        compararCadastros(&cadastroCSV, &cadastroExportado)){

        printf("APROVADO: Exportacao e leitura compativeis.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Dados exportados inconsistentes.\n");
    }

    // TESTE 7: Salvar cadastro vazio.
    printf("\nTESTE 7 - Salvar cadastro vazio\n");

    totalTestes++;

    if (salvarBinario(&cadastroVazio,
                      "src/output/cadastro_vazio.bin")){

        printf("APROVADO: Cadastro vazio salvo.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Falha ao salvar cadastro vazio.\n");
    }

    // TESTE 8: Recuperar cadastro vazio.
    printf("\nTESTE 8 - Carregar cadastro vazio\n");

    totalTestes++;

    if (carregarBinario(&cadastroExportado,
                        "src/output/cadastro_vazio.bin") &&
        cadastroExportado.quantidade == 2){

        // O cadastroExportado ja possuia duas perguntas.
        // A leitura adiciona os dados ao cadastro existente.
        printf("APROVADO: Leitura preservou o cadastro existente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Falha ao carregar cadastro vazio.\n");
    }

    // TESTE 9: Caminho inexistente na leitura binária.
    printf("\nTESTE 9 - Arquivo binario inexistente\n");

    totalTestes++;

    if (!carregarBinario(&cadastroVazio,
                         "src/output/arquivo_inexistente.bin")){

        printf("APROVADO: Falha identificada corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Arquivo inexistente foi carregado.\n");
    }

    // TESTE 10: Caminho inválido na gravação binária.
    printf("\nTESTE 10 - Caminho invalido na gravacao\n");

    totalTestes++;

    if (!salvarBinario(&cadastroCSV,
                       "src/output/pasta_inexistente/teste.bin")){

        printf("APROVADO: Falha identificada corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Gravacao inesperadamente realizada.\n");
    }

    // TESTE 11: Caminho inválido na exportação CSV.
    printf("\nTESTE 11 - Caminho invalido na exportacao\n");

    totalTestes++;

    if (!exportarCSV(&cadastroCSV,
                     "src/output/pasta_inexistente/teste.csv")){

        printf("APROVADO: Falha identificada corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: Exportacao inesperadamente realizada.\n");
    }

    // TESTE 12: Caminho inexistente na leitura CSV.
    printf("\nTESTE 12 - CSV inexistente\n");

    totalTestes++;

    if (!carregarCSV(&cadastroVazio,
                     "src/output/arquivo_inexistente.csv")){

        printf("APROVADO: Falha identificada corretamente.\n");
        testesAprovados++;

    } else {
        printf("REPROVADO: CSV inexistente foi carregado.\n");
    }

    // Exibe o resultado geral dos testes.
    printf("\n====================================\n");
    printf("          RESULTADO FINAL\n");
    printf("====================================\n");

    printf("Testes aprovados: %d/%d\n",
           testesAprovados, totalTestes);

    printf("Testes reprovados: %d/%d\n",
           totalTestes - testesAprovados, totalTestes);

    // Libera a memória dos cadastros.
    liberarCadastro(&cadastroCSV);
    liberarCadastro(&cadastroBinario);
    liberarCadastro(&cadastroExportado);
    liberarCadastro(&cadastroVazio);

    return 0;
}
