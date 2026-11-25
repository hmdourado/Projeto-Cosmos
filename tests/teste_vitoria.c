/**
 * @file teste_vitoria.c
 * @author Arthur Morais Palitot Fernandes (arthurpalitot@gmail.com | arthurmoraisfernandes@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 25-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "../include/raylib.h"
#include "../include/vitoria.h"
#include "../include/fontes.h"

#include <stdio.h>
#include <string.h>

#define TAM_NOME_TESTE 64

int main(void)
{
    // Inicializa a janela.
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "COSMOS - Teste da Tela de Vitoria");
    // Inicializa o dispositivo de áudio da Raylib.
    InitAudioDevice();

    bool audioDisponivel = IsAudioDeviceReady();

    if (!audioDisponivel) {
        TraceLog(LOG_WARNING, "Audio indisponivel no teste.");
    }
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
    inicializarEstadoVisualVitoria(
        &estado,
        audioDisponivel
    );

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
                    snprintf(
                        mensagemRanking,
                        sizeof(mensagemRanking),
                        "Digite um nome antes de continuar."
                    );
                }
            }

        } else {
            // R permite testar novamente o estado inicial.
            if (IsKeyPressed(KEY_R)) {
                rankingRegistrado = false;
                nomeJogador[0] = '\0';
                mensagemRanking[0] = '\0';
            }
        }

        BeginDrawing();

            // Renderiza diretamente a tela de vitoria.
            desenharTelaVitoria(
                &estado,
                &dados,
                nomeJogador,
                rankingRegistrado,
                mensagemRanking
            );

            // Instrucoes de teste no lado direito.
            DrawText(
                "TESTE DA TELA DE VITORIA",
                420, 30, 18, SKYBLUE
            );

            DrawText(
                "ESC: ENCERRAR TESTE",
                420, GetScreenHeight() - 55,
                12, LIGHTGRAY
            );

            DrawText(
                "R: REINICIAR REGISTRO",
                420, GetScreenHeight() - 35,
                12, LIGHTGRAY
            );

        EndDrawing();
    }

    // Libera os recursos antes de fechar a janela.
    liberarEstadoVisualVitoria(&estado);

    CloseWindow();

    return 0;
}
