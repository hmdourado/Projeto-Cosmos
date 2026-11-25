#include "../include/fontes.h"
#include"../include/raylib.h"
#include<stdio.h>

// Fontes privadas do módulo
static Font fonteTitulo;
static Font fontePrincipal;



static Font carregarFonte(const char *caminho, int tamanho)
{
    int caracteres[128];
    int quantidade = 0;

    // ASCII: letras, números, espaços e pontuação
    for (int i = 32; i <= 126; i++)
    {
        caracteres[quantidade++] = i;
    }

    // Acentos utilizados no português
    int acentos[] = {
        0x00E1, // á
        0x00E9, // é
        0x00ED, // í
        0x00F3, // ó
        0x00FA, // ú
        0x00E2, // â
        0x00EA, // ê
        0x00EE, // î
        0x00F4, // ô
        0x00FB, // û
        0x00E7, // ç
        0x005E, // ^
        0x00B4, // ´
        0x007E  // ~
    };

    for (int i = 0; i < sizeof(acentos) / sizeof(acentos[0]); i++)
    {
        caracteres[quantidade++] = acentos[i];
    }

    return LoadFontEx(caminho, tamanho, caracteres, quantidade);
}

void inicializarFontes(void)
{
    fonteTitulo = carregarFonte(
        "assets/fonts/Xolonium-Regular.ttf", 96
    );

    fontePrincipal = carregarFonte(
        "assets/fonts/Xolonium-Regular.ttf", 64
    );
}

Font obterFonteTitulo(void)
{
    return fonteTitulo;
}

Font obterFontePrincipal(void)
{
    return fontePrincipal;
}



void descarregarFontes(void)
{
    UnloadFont(fonteTitulo);
    UnloadFont(fontePrincipal);

}