
#include "../include/video.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <direct.h>
    #define obterDiretorioAtual _getcwd
#else
    #include <unistd.h>
    #define obterDiretorioAtual getcwd
#endif

#ifdef _WIN32
    #include <io.h>
    #include <fcntl.h>
    #define ABRIR_PROCESSO _popen
    #define FECHAR_PROCESSO _pclose
#else
    #define ABRIR_PROCESSO popen
    #define FECHAR_PROCESSO pclose
#endif

#define VIDEO_LARGURA 1280
#define VIDEO_ALTURA 720
#define VIDEO_FPS 24

// Lê exatamente um frame do processo FFmpeg.
static bool LerFrame(VideoBackground *video)
{
    size_t tamanhoFrame =
        (size_t)video->largura * video->altura * 4;

    size_t totalLido = fread(
        video->buffer,
        1,
        tamanhoFrame,
        video->processo
    );

    if (totalLido != tamanhoFrame)
    {
        TraceLog(
            LOG_ERROR,
            "Frame incompleto: %zu de %zu bytes",
            totalLido,
            tamanhoFrame
        );

        return false;
    }

    return true;
}

// Inicializa o vídeo.
bool VideoInit(VideoBackground *video, const char *arquivo)
{
    
    if (video == NULL || arquivo == NULL)
        return false;

    memset(video, 0, sizeof(VideoBackground));

    video->largura = VIDEO_LARGURA;
    video->altura = VIDEO_ALTURA;
    video->fps = VIDEO_FPS;

    size_t tamanhoFrame =
        (size_t)video->largura * video->altura * 4;

    video->buffer = malloc(tamanhoFrame);

    if (video->buffer == NULL)
        return false;

    Image imagem = GenImageColor(
        video->largura,
        video->altura,
        BLACK
    );

    video->textura = LoadTextureFromImage(imagem);
    UnloadImage(imagem);

    if (video->textura.id == 0)
    {
        free(video->buffer);
        video->buffer = NULL;
        return false;
    }

    char comando[1024];
    char diretorio[512];
    if (obterDiretorioAtual(diretorio, sizeof(diretorio)) == NULL){
        TraceLog(LOG_ERROR, "Nao foi possivel obter o diretorio atual.");
        VideoClose(video);
        return false;
    }

#ifdef _WIN32
    snprintf(comando, sizeof(comando),
        "ffmpeg\\ffmpeg.exe -nostdin -hide_banner -loglevel error "
        "-i %s -an "
        "-vf scale=%d:%d,fps=%d "
        "-pix_fmt rgba -f rawvideo pipe:1 2>ffmpeg_error.log",
        arquivo,
        video->largura,
        video->altura,
        video->fps
    );
#else
    snprintf(comando, sizeof(comando),
        "ffmpeg -nostdin -hide_banner -loglevel error "
        "-i \"%s\" -an "
        "-vf \"scale=%d:%d,fps=%d\" "
        "-pix_fmt rgba -f rawvideo pipe:1 2>/dev/null",
        arquivo,
        video->largura,
        video->altura,
        video->fps
    );
#endif
    TraceLog(LOG_INFO, "Comando FFmpeg: %s", comando);
    TraceLog(LOG_INFO, "Iniciando VideoInit...");

    video->processo = ABRIR_PROCESSO(comando, "rb");

    if (video->processo == NULL)
    {
        TraceLog(LOG_ERROR, "Falha ao abrir processo FFmpeg");

        VideoClose(video);
        return false;
    }
    TraceLog(LOG_INFO, "Processo FFmpeg iniciado");

#ifdef _WIN32
    _setmode(_fileno(video->processo), _O_BINARY);
#endif

    video->finalizado = false;
    video->acumulador = 0.0f;

    // Lê o primeiro frame para verificar se o FFmpeg conseguiu decodificar.
    if (!LerFrame(video))
    {      
        TraceLog(LOG_ERROR, "FFmpeg nao conseguiu entregar o primeiro frame");
        VideoClose(video);
        return false;
    }
   TraceLog(LOG_INFO, "Primeiro frame recebido!");
    UpdateTexture(video->textura, video->buffer);

    return true;
}

// Atualiza os frames.
void VideoUpdate(VideoBackground *video)
{
    if (video == NULL || video->finalizado)
        return;

    video->acumulador += GetFrameTime();

    float intervalo = 1.0f / video->fps;

    if (video->acumulador >= intervalo)
    {
        if (!LerFrame(video))
        {
            video->finalizado = true;
            return;
        }

        UpdateTexture(video->textura, video->buffer);

        video->acumulador -= intervalo;
    }
}

// Desenha o vídeo na tela.
void VideoDraw(VideoBackground *video)
{
    if (video == NULL || video->textura.id == 0)
        return;

    DrawTexturePro(
        video->textura,
        (Rectangle){
            0, 0,
            (float)video->largura,
            (float)video->altura
        },
        (Rectangle){
            0, 0,
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        },
        (Vector2){0, 0},
        0.0f,
        WHITE
    );
}

// Libera os recursos.
void VideoClose(VideoBackground *video)
{
    if (video == NULL)
        return;

    if (video->processo != NULL)
    {
        FECHAR_PROCESSO(video->processo);
        video->processo = NULL;
    }

    if (video->textura.id != 0)
    {
        UnloadTexture(video->textura);
        video->textura.id = 0;
    }

    free(video->buffer);
    video->buffer = NULL;

    video->finalizado = true;
}
