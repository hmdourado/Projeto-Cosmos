/**
 * @file main.c
 * @brief Coordenação da inicialização, execução e encerramento do programa.
 */
#include "include/arquivo.h"
#include "include/menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// A compilação escolhe o menu que fornece executarMenu():
// terminal: src/menu_temporario.c; gráfico: src/menu.c (veja docs/regras.md).
// #include "include/raylib.h"
// #include "include/video.h"

int main(int argc, char *argv[]){
    // Permite informar caminhos alternativos para testes e execução.
    if (argc > 3){
        fprintf(stderr, "Uso: %s [arquivo.bin [arquivo.csv]]\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *binario = argc > 1 ? argv[1] : "dados/perguntas.dat";
    const char *csv = argc > 2 ? argv[2] : "dados/perguntas.csv";

    // Inicializa o cadastro vazio antes de tentar recuperar os dados.
    Cadastro cadastro;
    inicializarCadastro(&cadastro);
    srand((unsigned int)time(NULL));

    // Prioriza o binário e descarta registros parciais se a leitura falhar.
    if (!carregarBinario(&cadastro, binario)){
        liberarCadastro(&cadastro);
        inicializarCadastro(&cadastro);
        fprintf(stderr, "Binário ausente ou inválido. Importando CSV.\n");

        // Interrompe a execução quando nenhuma fonte pode ser carregada.
        if (!carregarCSV(&cadastro, csv)){
            fprintf(stderr, "Não foi possível carregar o CSV: %s\n", csv);
            liberarCadastro(&cadastro);
            return EXIT_FAILURE;
        }

        // Cria o binário imediatamente após concluir a importação inicial.
        if (!salvarBinario(&cadastro, binario)){
            fprintf(stderr, "Não foi possível criar o binário: %s\n", binario);
            liberarCadastro(&cadastro);
            return EXIT_FAILURE;
        }
    }

    // Entrega o cadastro carregado ao módulo responsável pela interface.
    executarMenu(&cadastro);

    // Salva as alterações e libera a memória mesmo em caso de falha na gravação.
    int salvo = salvarBinario(&cadastro, binario);
    liberarCadastro(&cadastro);
    if (!salvo){
        fprintf(stderr, "Não foi possível salvar o binário: %s\n", binario);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
