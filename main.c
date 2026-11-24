
/**
 * @file main.c
 * @brief Implementação da inicialização, execução e encerramento do programa.
 */
#include "include/cadastro.h"
#include "include/arquivo.h"
#include "include/menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "include/raylib.h"
#include "include/video.h"

int main(int argc, char *argv[]){

    // Impede a execução com uma quantidade inválida de argumentos.
    if (argc > 3){
        fprintf(stderr, "Uso: %s [arquivo.bin [arquivo.csv]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Define os caminhos dos arquivos, utilizando os padrões quando não informados.
    const char *binario = argc > 1 ? argv[1] : "dados/perguntas.dat";
    const char *csv = argc > 2 ? argv[2] : "dados/perguntas.csv";

    // Inicializa o gerador de números aleatórios utilizado no sorteio das perguntas.
    srand((unsigned int)time(NULL));

    // Inicializa o cadastro vazio antes de tentar recuperar os dados.
    Cadastro cadastro;
    inicializarCadastro(&cadastro);

    // Prioriza o carregamento do arquivo binário para recuperar os dados persistidos.
    if (!carregarBinario(&cadastro, binario)){

        fprintf(stderr, "Binário ausente ou inválido. Tentando importar o CSV.\n");

        // Descarta possíveis registros parciais antes de iniciar uma nova importação.
        liberarCadastro(&cadastro);
        inicializarCadastro(&cadastro);

        // Interrompe a execução caso o arquivo CSV também não possa ser carregado.
        if (!carregarCSV(&cadastro, csv)){
            fprintf(stderr, "Não foi possível carregar o arquivo CSV: %s\n", csv);
            liberarCadastro(&cadastro);
            return EXIT_FAILURE;
        }

        // Persiste os dados importados para disponibilizá-los nas próximas execuções.
        if (!salvarBinario(&cadastro, binario)){
            fprintf(stderr, "Não foi possível criar o arquivo binário: %s\n", binario);
            liberarCadastro(&cadastro);
            return EXIT_FAILURE;
        }
    }

    // Entrega o cadastro carregado ao módulo responsável pela interface gráfica.
    executarMenu(&cadastro);

    // Salva as possíveis alterações realizadas durante a execução do programa.
    int salvamentoSucesso = salvarBinario(&cadastro, binario);

    // Libera a memória alocada pelo cadastro antes de encerrar o programa.
    liberarCadastro(&cadastro);

    // Retorna uma falha caso a persistência dos dados não seja concluída.
    if (!salvamentoSucesso){
        fprintf(stderr, "Não foi possível salvar os dados em: %s\n", binario);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
