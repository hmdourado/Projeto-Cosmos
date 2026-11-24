/**
 * @file teste_integracao.c
 * @brief Testes do fluxo completo com um menu simulado e arquivos temporários.
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _DARWIN_C_SOURCE
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

// Reutiliza o ponto de entrada real com outro nome para testar sua coordenação.
#define main mainAplicacao
#include "../main.c"
#undef main

static int criarDiretorio(const char *caminho){
#ifdef _WIN32
    return _mkdir(caminho);
#else
    return mkdir(caminho, 0700);
#endif
}

static int removerDiretorio(const char *caminho){
#ifdef _WIN32
    return _rmdir(caminho);
#else
    return rmdir(caminho);
#endif
}

static int criarDiretorioTemporario(char *caminho, size_t tamanho){
#ifdef _WIN32
    char pastaTemporaria[MAX_PATH];
    DWORD comprimento = GetTempPathA(MAX_PATH, pastaTemporaria);
    if (comprimento == 0 || comprimento >= MAX_PATH || tamanho < MAX_PATH){
        return 0;
    }
    if (GetTempFileNameA(pastaTemporaria, "cos", 0, caminho) == 0){
        return 0;
    }
    if (!DeleteFileA(caminho) || !CreateDirectoryA(caminho, NULL)){
        return 0;
    }
    return 1;
#else
    if (snprintf(caminho, tamanho, "/tmp/cosmos-integracao-XXXXXX") < 0){
        return 0;
    }
    return mkdtemp(caminho) != NULL;
#endif
}

static int truncarArquivo(const char *caminho, long long tamanho){
#ifdef _WIN32
    FILE *arquivo = fopen(caminho, "rb+");
    if (arquivo == NULL){
        return -1;
    }
    int resultado = _chsize_s(_fileno(arquivo), tamanho) == 0 ? 0 : -1;
    if (fclose(arquivo) != 0){
        resultado = -1;
    }
    return resultado;
#else
    return truncate(caminho, (off_t)tamanho);
#endif
}

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
        assert(criarDiretorio(bloquearGravacao) == 0);
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

static void testarCamposCSV(const char *caminho){
    Cadastro original;
    Cadastro reimportado;
    inicializarCadastro(&original);
    inicializarCadastro(&reimportado);
    assert(carregarCSV(&original, "dados/perguntas.csv"));

    Pergunta *pergunta = &original.perguntas[0];
    strcpy(pergunta->enunciado, "\"Terra, Lua\"\nQual é a ordem?");
    strcpy(pergunta->alternativas[0], "Terra, Lua");
    assert(exportarCSV(&original, caminho));
    assert(carregarCSV(&reimportado, caminho));
    assert(reimportado.quantidade == original.quantidade);
    for (size_t i = 0; i < original.quantidade; i++){
        const Pergunta *esperada = &original.perguntas[i];
        const Pergunta *lida = &reimportado.perguntas[i];
        assert(lida->id == esperada->id && lida->nivel == esperada->nivel);
        assert(lida->respostaCorreta == esperada->respostaCorreta);
        assert(strcmp(lida->enunciado, esperada->enunciado) == 0);
        for (int j = 0; j < QUANT_ALTERNATIVAS; j++){
            assert(strcmp(lida->alternativas[j], esperada->alternativas[j]) == 0);
        }
    }

    // Um campo sem aspas de fechamento deve rejeitar toda a importação.
    FILE *arquivo = fopen(caminho, "a");
    assert(arquivo != NULL);
    assert(fputs("81,1,\"Campo, sem fechamento", arquivo) >= 0);
    assert(fclose(arquivo) == 0);
    assert(!carregarCSV(&reimportado, caminho));
    assert(reimportado.quantidade == original.quantidade);

    liberarCadastro(&original);
    liberarCadastro(&reimportado);
    assert(remove(caminho) == 0);
}

int main(void){
    // Isola todos os arquivos gerados, sem modificar o banco real do projeto.
    char diretorio[260];
    assert(criarDiretorioTemporario(diretorio, sizeof(diretorio)));
    char binario[256];
    char ausente[256];
    char caminhoInvalido[256];
    char csvExportado[300];
    snprintf(binario, sizeof(binario), "%s/perguntas.bin", diretorio);
    snprintf(ausente, sizeof(ausente), "%s/ausente.csv", diretorio);
    snprintf(caminhoInvalido, sizeof(caminhoInvalido),
             "%s/inexistente/perguntas.dat", diretorio);
    snprintf(csvExportado, sizeof(csvExportado), "%s/exportado.csv", diretorio);
    testarCamposCSV(csvExportado);
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
    assert(truncarArquivo(binario, (long long)(sizeof(size_t) + sizeof(Pergunta))) == 0);
    argumentos[2] = "dados/perguntas.csv";
    assert(mainAplicacao(3, argumentos) == EXIT_SUCCESS);
    assert(chamadasMenu == 3);
    verificarGravacao(binario);

    // Sem nenhuma fonte válida, não deve executar o menu nem sobrescrever o binário.
    assert(truncarArquivo(binario, 0) == 0);
    argumentos[2] = ausente;
    assert(mainAplicacao(3, argumentos) == EXIT_FAILURE);
    assert(chamadasMenu == 3);
    FILE *arquivo = fopen(binario, "rb");
    assert(arquivo != NULL);
    assert(fgetc(arquivo) == EOF);
    assert(fclose(arquivo) == 0);

    // Um caminho sem diretório impede a gravação e deve retornar falha.
    argumentos[1] = caminhoInvalido;
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
    assert(removerDiretorio(binario) == 0);
    assert(removerDiretorio(diretorio) == 0);
    puts("Testes de integração passaram.");
    return EXIT_SUCCESS;
}
