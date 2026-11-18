/**
 * @file teste_integracao.c
 * @brief Testes do fluxo completo com um menu simulado e arquivos temporários.
 */
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// Reutiliza o ponto de entrada real com outro nome para testar sua coordenação.
#define main mainAplicacao
#include "../main.c"
#undef main

static int chamadasMenu = 0;
static const char *bloquearGravacao = NULL;

void executarMenu(Cadastro *cadastro){
    // Simula uma alteração feita pela interface antes de encerrar.
    assert(cadastro->quantidade == 80);
    strcpy(cadastro->perguntas[0].enunciado, "Alteração feita pelo menu");
    chamadasMenu++;
    // Simula uma falha de gravação ocorrida somente depois de abrir o menu.
    if (bloquearGravacao != NULL){
        assert(remove(bloquearGravacao) == 0);
        assert(mkdir(bloquearGravacao, 0700) == 0);
    }
}

/** @brief Confere se o encerramento persistiu as alterações do menu. */
static void verificarGravacao(const char *caminho){
    Cadastro cadastro;
    inicializarCadastro(&cadastro);
    assert(carregarBinario(&cadastro, caminho));
    assert(cadastro.quantidade == 80);
    assert(strcmp(cadastro.perguntas[0].enunciado, "Alteração feita pelo menu") == 0);
    liberarCadastro(&cadastro);
}

int main(void){
    // Isola todos os arquivos gerados, sem modificar o banco real do projeto.
    char diretorio[] = "/tmp/cosmos-integracao-XXXXXX";
    assert(mkdtemp(diretorio) != NULL);
    char binario[256];
    char ausente[256];
    snprintf(binario, sizeof(binario), "%s/perguntas.bin", diretorio);
    snprintf(ausente, sizeof(ausente), "%s/ausente.csv", diretorio);
    char *argumentos[] = {"cosmos", binario, "dados/perguntas.csv", NULL};

    // Primeira execução: importa o CSV, chama o menu e cria o binário.
    assert(mainAplicacao(3, argumentos) == EXIT_SUCCESS);
    assert(chamadasMenu == 1);
    verificarGravacao(binario);

    // Demais execuções: o binário deve funcionar mesmo sem um CSV disponível.
    argumentos[2] = ausente;
    assert(mainAplicacao(3, argumentos) == EXIT_SUCCESS);
    assert(chamadasMenu == 2);
    verificarGravacao(binario);

    // Trunca o binário após um registro válido para simular uma leitura parcial.
    assert(truncate(binario, (off_t)(sizeof(size_t) + sizeof(Pergunta))) == 0);
    argumentos[2] = "dados/perguntas.csv";
    assert(mainAplicacao(3, argumentos) == EXIT_SUCCESS);
    assert(chamadasMenu == 3);
    verificarGravacao(binario);

    // Sem nenhuma fonte válida, não deve executar o menu nem sobrescrever o binário.
    assert(truncate(binario, 0) == 0);
    argumentos[2] = ausente;
    assert(mainAplicacao(3, argumentos) == EXIT_FAILURE);
    assert(chamadasMenu == 3);
    FILE *arquivo = fopen(binario, "rb");
    assert(arquivo != NULL);
    assert(fgetc(arquivo) == EOF);
    assert(fclose(arquivo) == 0);

    // Um caminho sem diretório impede a gravação e deve retornar falha.
    argumentos[1] = "/diretorio-cosmos-inexistente/perguntas.bin";
    argumentos[2] = "dados/perguntas.csv";
    assert(mainAplicacao(3, argumentos) == EXIT_FAILURE);
    assert(chamadasMenu == 3);

    // Verifica também o retorno de erro quando a gravação final falha.
    assert(remove(binario) == 0);
    argumentos[1] = binario;
    bloquearGravacao = binario;
    assert(mainAplicacao(3, argumentos) == EXIT_FAILURE);
    assert(chamadasMenu == 4);
    bloquearGravacao = NULL;

    // Remove somente os arquivos temporários criados pelo próprio teste.
    assert(rmdir(binario) == 0);
    assert(rmdir(diretorio) == 0);
    puts("Testes de integração passaram.");
    return EXIT_SUCCESS;
}
