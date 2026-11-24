/**
 * @file menu.c 
 * @author Arthur Morais Palitot Fernandes (arthurpalitot@gmail.com | arthurmoraisfernandes@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 23-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "../include/raylib.h"
#include "../include/video.h"
#include "../include/menu.h"

/**
 * @brief Executa Menu 
 * 
 * @param cadastro 
 */

void executarMenu(Cadastro *cadastro)
{
    (void)cadastro; // Evita warning enquanto não utilizamos o cadastro.

    InitWindow(1280, 720, "Meu aplicativo");

    SetTargetFPS(60);

    VideoBackground fundo;

    //verifica se o video Init retorna False e seta erro 
    if (!VideoInit(&fundo, "assets/14014441_1280_720_30fps.mp4"))
    {
        TraceLog(LOG_ERROR, "Erro ao inicializar video!");
        CloseWindow();
        return;
    }
    TraceLog(LOG_INFO, "Video inicializado com sucesso!");
    
    while (!WindowShouldClose())
    {
        // Atualização
        VideoUpdate(&fundo);

        // Renderização
        BeginDrawing();

            ClearBackground(BLACK);

            // Primeiro desenhamos o fundo
            VideoDraw(&fundo);

            // Depois desenhamos a interface
            DrawText(
                "MEU APLICATIVO",
                50, 50,
                35,
                WHITE
            );

            DrawText(
                "Bem-vindo!",
                50, 110,
                25,
                WHITE
            );

            DrawRectangleRounded(
                (Rectangle){50, 180, 200, 55},
                0.2f,
                8,
                DARKBLUE
            );

            DrawText(
                "ENTRAR",
                105, 197,
                20,
                WHITE
            );

        EndDrawing();
    }

    VideoClose(&fundo);

    CloseWindow();
}
