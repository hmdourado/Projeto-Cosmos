#include "../include/menu.h"
#include "../include/raylib.h"

void executarMenu(Cadastro *cadastro){
    (void)cadastro;

    InitWindow(800, 450, "Projeto Cosmos");
    SetTargetFPS(60);

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
}