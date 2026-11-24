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
    
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_FULLSCREEN_MODE);

    InitWindow(1280, 720, "Cosmos");
    //fontes

    //fonteCosmos = LoadFontEx("assets/fonts/Nasalization Rg.otf");

    Font fonteCosmos = LoadFontEx(
        "assets/fonts/spacerangerlaser.ttf",
        128,    // Resolução de carregamento
        NULL,
        0
    );

    SetTextureFilter(fonteCosmos.texture, TEXTURE_FILTER_BILINEAR);
    /*
     Font fonteMenuSecundaria = LoadFontEx(
        "assets/fonts/",
        128,
        NULL,
        0
    );
    */
   
    //SetTextureFilter(fonteMenuSecundaria.texture, TEXTURE_FILTER_BILINEAR);

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
                 (Vector2){415,50},
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
    UnloadFont(fonteCosmos);
    CloseWindow();
}
