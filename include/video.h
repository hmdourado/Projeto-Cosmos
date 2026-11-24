
#ifndef VIDEO_H
#define VIDEO_H

#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    Texture2D textura;
    FILE *processo;

    unsigned char *buffer;

    int largura;
    int altura;
    int fps;

    float acumulador;

    bool finalizado;
} VideoBackground;

bool VideoInit(VideoBackground *video, const char *arquivo);

void VideoUpdate(VideoBackground *video);

void VideoDraw(VideoBackground *video);

void VideoClose(VideoBackground *video);

#endif
