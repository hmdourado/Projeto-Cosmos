/**
 * @file teste_vitoria.c
 * @brief Pré-visualização independente da vitória real do Projeto Cosmos.
 *
 * Carrega o cadastro, inicia uma partida pelas regras reais e responde cada
 * pergunta usando a respostaCorreta do próprio registro. Ao receber
 * RESPOSTA_VITORIA, abre a tela cinematográfica original de vitória.
 *
 * Este executável não altera main.c, menu.c, o cadastro, o binário de perguntas
 * nem o ranking oficial.
 */

#include "raylib.h"
#include "cadastro.h"
#include "arquivo.h"
#include "jogo.h"
#include "vitoria.h"
#include "fontes.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_NOME_TESTE 64

int main(void)
{
    // Inicializa a janela.
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "COSMOS - Teste da Tela de Vitoria");

    if (!IsWindowReady()) {
        fprintf(stderr, "Nao foi possivel inicializar a janela.\n");
        return 1;
    }

    SetTargetFPS(60);

    // Inicializa as fontes utilizadas pela interface.
    inicializarFontes();

    // Dados ficticios para simular uma partida concluida.
    DadosTelaVitoria dados = {
        .perguntasRespondidas = 15,
        .totalPerguntas = 15,
        .acertos = 15,
        .premioFinal = 1000000,
        .premioGarantido = 1000000,
        .ajudasUtilizadas = 2,
        .totalAjudas = 4
    };

    // Inicializa o estado visual e o video de comemoracao.
    EstadoVisualVitoria estado = {0};
    inicializarEstadoVisualVitoria(&estado);

    // Simulacao do registro no ranking.
    char nomeJogador[TAM_NOME_TESTE] = "";
    char mensagemRanking[128] = "";

    bool rankingRegistrado = false;

    while (!WindowShouldClose()) {

        // Atualiza o video de fundo.
        atualizarEstadoVisualVitoria(&estado);

        // Entrada de texto para testar o campo de nome.
        if (!rankingRegistrado) {

            int caractere = GetCharPressed();

            while (caractere > 0) {
                size_t tamanho = strlen(nomeJogador);

                if (caractere >= 32 &&
                    caractere <= 125 &&
                    tamanho + 1 < sizeof(nomeJogador)) {

                    nomeJogador[tamanho] = (char)caractere;
                    nomeJogador[tamanho + 1] = '\0';
                }

                caractere = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                size_t tamanho = strlen(nomeJogador);

                if (tamanho > 0) {
                    nomeJogador[tamanho - 1] = '\0';
                }
            }

            // Simula o salvamento do resultado.
            if (IsKeyPressed(KEY_ENTER)) {
                if (nomeJogador[0] != '\0') {
                    rankingRegistrado = true;

                    snprintf(
                        mensagemRanking,
                        sizeof(mensagemRanking),
                        "Teste registrado: %s",
                        nomeJogador
                    );
                } else {
                    estado = DEMO_FALHA;
                    snprintf(mensagem, sizeof(mensagem),
                             "A resposta %d retornou resultado inesperado (%d).",
                             numeroPergunta, resultado);
                }
            }
        }

        if (estado == DEMO_VITORIA && visualVitoriaInicializado) {
            atualizarEstadoVisualVitoria(&visualVitoria);
        }

        BeginDrawing();
            if (estado == DEMO_VITORIA && visualVitoriaInicializado) {
                /* A tela original de vitória e seu vídeo são renderizados aqui. */
                desenharTelaVitoria(
                    &visualVitoria,
                    &dadosVitoria,
                    "",
                    false,
                    ""
                );

                /* Identifica claramente que esta visualização não altera o ranking. */
                DrawRectangleRounded(
                    (Rectangle){GetScreenWidth() - 390.0f,
                                GetScreenHeight() - 54.0f, 370.0f, 34.0f},
                    0.18f, 8, (Color){4, 15, 31, 210}
                );
                DrawText("PRE-VISUALIZACAO | RANKING NAO ALTERADO",
                         GetScreenWidth() - 376,
                         GetScreenHeight() - 43,
                         10,
                         COR_CIANO_DEMO);
                DrawText("ESC: FECHAR",
                         GetScreenWidth() - 125,
                         18,
                         10,
                         COR_TEXTO_DEMO);
            } else {
                desenharTelaProgresso(&partida, respondidas, mensagem, estado);
            }
        EndDrawing();
    }

    if (visualVitoriaInicializado) {
        liberarEstadoVisualVitoria(&visualVitoria);
    }

    /* Descarrega as três fontes; o módulo fontes.c não libera a fonteLogo. */
    UnloadFont(fonteTitulo);
    UnloadFont(fontePrincipal);
    UnloadFont(fonteLogo);

    CloseWindow();
    liberarCadastro(&cadastro);

    if (estado == DEMO_FALHA) {
        fprintf(stderr, "Demonstracao interrompida: %s\n", mensagem);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
