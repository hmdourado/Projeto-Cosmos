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
    
    SetConfigFlags(FLAG_WINDOW_UNDECORATED); // TELA SEM ADORNO SUPERIOR
   // SetConfigFlags(FLAG_FULLSCREEN_MODE); // TELA CHEIA
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(1280, 720, "Cosmos");

 
    //fontes

    //fonteCosmos = LoadFontEx("assets/fonts/Nasalization Rg.otf");

    Font fonteCosmos = LoadFontEx(
        "assets/fonts/spacerangerlaser.ttf",
        600,    // Resolução de carregamento
        NULL,
        0
    );

    SetTextureFilter(fonteCosmos.texture, TEXTURE_FILTER_BILINEAR);
    
     Font fonteMenuSecundaria = LoadFontEx(
        "assets/fonts/terminator real nfi.ttf",
        128,
        NULL,
        0
    );
    
   
    SetTextureFilter(fonteMenuSecundaria.texture, TEXTURE_FILTER_BILINEAR);

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
        if (IsKeyPressed(KEY_F11)){
            ToggleFullscreen();
        }
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
            

            DrawTextEx(
                fonteMenuSecundaria,

                "COMECAR PARTIDA >>",
                (Vector2){800,650},
                 20,       // Tamanho da fonte
                 2,
                 WHITE
            );

             DrawTextEx(
                fonteMenuSecundaria,
                "SALVAR E SAIR",
                 (Vector2){75,650},
                 15,       // Tamanho da fonte
                 2,
                 RED
            );
            DrawTextEx(
                fonteMenuSecundaria,
                "CONFIGURACOES",
                 (Vector2){350,650},
                 15,       // Tamanho da fonte
                 2,
                 WHITE
            );

        EndDrawing();
    }

    VideoClose(&fundo);
    UnloadFont(fonteCosmos);
    CloseWindow();
}
