
#include "../include/raylib.h"
#include "../include/video.h"
#include "../include/menu.h"

void executarMenu(Cadastro *cadastro)
{
    (void)cadastro; // Evita warning enquanto não utilizamos o cadastro.

    InitWindow(1280, 720, "Meu aplicativo");

    SetTargetFPS(60);

    VideoBackground fundo;

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
