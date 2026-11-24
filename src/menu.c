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



Font fonteCosmos;
/**
 * @brief Executa Menu 
 * 
 * @param cadastro 
 */

void executarMenu(Cadastro *cadastro)
{
    (void)cadastro; // Evita warning enquanto não utilizamos o cadastro.

    InitWindow(1280, 720, "Cosmos");
    //fontes
    fonteCosmos = LoadFont("assets/fonts/Nasalization.ttf");
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
            DrawTextEx(
                fonteCosmos,
                "COSMOS",
                 (Vector2){ 450, 50 },
                 100,       // Tamanho da fonte
                 2,
                 WHITE
            );

            DrawText(
                "v.1.0.0 (beta)",
                50, 15,
                10,
                WHITE
            );
/*
            DrawRectangleRounded(
                (Rectangle){50, 180, 200, 55},
                0.2f,
                8,
                DARKBLUE
            );
*/
            

            DrawText(
                "COMEÇAR PARTIDA",
                925, 650,
                30,
                WHITE
            );

             DrawText(
                "SALVAR E SAIR",
                125, 650,
                25,
                RED
            );

        EndDrawing();
    }

    VideoClose(&fundo);

    CloseWindow();
}
