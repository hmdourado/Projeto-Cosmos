/**
 * @file vitoria.c
 * @brief Interface cinematográfica de conclusão da jornada.
 */

#include "../../include/vitoria.h"
#include "../../include/raylib.h"
#include "../../include/fontes.h"
#include "../../include/audio.h"
#include <stdio.h>
#include <string.h>

#define CAMINHO_VIDEO_VITORIA "assets/background1.mp4"
#define CAMINHO_AUDIO_VITORIA "assets/audio/musicaganhador.mp3"

static const Color COR_FUNDO = {5, 12, 29, 255};
static const Color COR_PAINEL = {7, 20, 42, 235};
static const Color COR_BORDA = {57, 112, 158, 220};
static const Color COR_CIANO = {46, 190, 240, 255};
static const Color COR_DOURADO = {255, 198, 91, 255};
static const Color COR_VERDE = {76, 222, 164, 255};
static const Color COR_TEXTO = {239, 247, 255, 255};
static const Color COR_SECUNDARIA = {153, 181, 210, 255};


/* Inicializa o vídeo exclusivo da vitória. */
void inicializarEstadoVisualVitoria(
    EstadoVisualVitoria *estado,
    bool audioDisponivel
) {
    if (estado == NULL) return;

    *estado = (EstadoVisualVitoria){0};

    // Inicializa o vídeo de comemoracao.
    estado->videoDisponivel = VideoInit(
        &estado->fundo,
        CAMINHO_VIDEO_VITORIA
    );

    if (!estado->videoDisponivel) {
        TraceLog(LOG_WARNING, "Video de comemoracao indisponivel.");
    }

    // Inicializa a trilha sonora da vitória.
    if (audioDisponivel && FileExists(CAMINHO_AUDIO_VITORIA)) {

        estado->musica = LoadMusicStream(CAMINHO_AUDIO_VITORIA);

        estado->musica.looping = true;

        SetMusicVolume(estado->musica, 0.45f);

        PlayMusicStream(estado->musica);

        estado->musicaDisponivel = true;

    } else {
        TraceLog(LOG_WARNING, "Audio de comemoracao indisponivel.");
    }
}

/* Atualiza a reprodução do vídeo. */
void atualizarEstadoVisualVitoria(EstadoVisualVitoria *estado) {
    if (estado == NULL) return;

    if (estado->videoDisponivel) {
        VideoUpdate(&estado->fundo);
    }

    if (estado->musicaDisponivel) {
        UpdateMusicStream(estado->musica);
    }
}

/* Descarrega os recursos da tela de vitória. */
void liberarEstadoVisualVitoria(EstadoVisualVitoria *estado) {
    if (estado == NULL) return;

    if (estado->videoDisponivel) {
        VideoClose(&estado->fundo);
        estado->videoDisponivel = false;
    }

    if (estado->musicaDisponivel) {
        StopMusicStream(estado->musica);
        UnloadMusicStream(estado->musica);
        estado->musicaDisponivel = false;
    }
}

/* Desenha um painel individual de estatística. */
static void desenharEstatistica(
    Rectangle area,
    const char *titulo,
    const char *valor,
    Color destaque
) {
    DrawRectangleRounded(
        area, 0.12f, 10, COR_PAINEL
    );

    DrawRectangleRoundedLinesEx(
        area, 0.12f, 10, 1.2f, COR_BORDA
    );

    DrawTextEx(
        obterFonteTitulo(),
        titulo,
        (Vector2){(int)area.x + 18, (int)area.y + 13},
        11,
        2,
        COR_SECUNDARIA
    );

    DrawTextEx(
        obterFonteTitulo(),
        valor,
        (Vector2){area.x + 18, area.y + 37},
        23,
        1.5f,
        destaque
    );
}


/* Desenha um título centralizado. */
static void desenharTituloCentralizado(
    const char *texto,
    float centroX,
    float y,
    float tamanho,
    Color cor
) {
    Vector2 medida = MeasureTextEx(
        obterFonteLogo(), texto, tamanho, 2.0f
    );

    DrawTextEx(
        obterFonteLogo(),
        texto,
        (Vector2){centroX - medida.x / 2.0f, y},
        tamanho,
        2.0f,
        cor
    );
}


/* Renderiza a página de vitória. */
/* Renderiza a página de vitória com barra lateral à esquerda. */
void desenharTelaVitoria(
    EstadoVisualVitoria *estado,
    const DadosTelaVitoria *dados,
    const char *nomeJogador,
    bool rankingRegistrado,
    const char *mensagemRanking
) {
    if (dados == NULL) return;

    int largura = GetScreenWidth();
    int altura = GetScreenHeight();

    /* =====================================================
       1. VÍDEO DE FUNDO
       ===================================================== */

    ClearBackground(COR_FUNDO);

    if (estado != NULL && estado->videoDisponivel) {
        VideoDraw(&estado->fundo);
    }

    /* Escurece levemente o cenário inteiro. */
    DrawRectangle(
        0, 0, largura, altura,
        (Color){2, 8, 22, 75}
    );

    /* =====================================================
       2. BARRA LATERAL
       ===================================================== */

    float margem = 24.0f;

    float larguraPainel = largura >= 1100
        ? 370.0f
        : largura * 0.38f;

    if (larguraPainel > 370.0f) larguraPainel = 370.0f;
    if (larguraPainel < 280.0f) larguraPainel = 280.0f;

    float alturaPainel = altura - 2.0f * margem;

    Rectangle painel = {
        margem,
        margem,
        larguraPainel,
        alturaPainel
    };

    /* Fundo translúcido da barra lateral. */
    DrawRectangleRounded(
        painel,
        0.055f,
        12,
        (Color){5, 17, 37, 238}
    );

    DrawRectangleRoundedLinesEx(
        painel,
        0.055f,
        12,
        1.5f,
        COR_CIANO
    );

    /* Brilho decorativo superior. */
    DrawLineEx(
        (Vector2){painel.x + 18, painel.y + 3},
        (Vector2){painel.x + painel.width - 18, painel.y + 3},
        2.0f,
        (Color){46, 190, 240, 130}
    );

    float centroX = painel.x + painel.width / 2.0f;

    /* =====================================================
       3. CABEÇALHO
       ===================================================== */

    desenharTituloCentralizado(
        "JORNADA CONCLUIDA",
        centroX,
        painel.y + 25.0f,
        21.0f,
        WHITE
    );

    desenharTituloCentralizado(
        "MISSAO CUMPRIDA",
        centroX,
        painel.y + 59.0f,
        10.0f,
        COR_CIANO
    );

    DrawLine(
        (int)painel.x + 20,
        (int)painel.y + 89,
        (int)(painel.x + painel.width - 20),
        (int)painel.y + 89,
        (Color){57, 112, 158, 150}
    );

    /* =====================================================
       4. ESTATÍSTICAS VERTICAIS
       ===================================================== */

    char acertos[40];
    char premio[48];
    char garantido[48];
    char ajudas[40];

    snprintf(
        acertos, sizeof(acertos),
        "%d / %u",
        dados->acertos,
        dados->totalPerguntas
    );

    snprintf(
        premio, sizeof(premio),
        "R$ %d",
        dados->premioFinal
    );

    snprintf(
        garantido, sizeof(garantido),
        "R$ %d",
        dados->premioGarantido
    );

    snprintf(
        ajudas, sizeof(ajudas),
        "%u / %u",
        dados->ajudasUtilizadas,
        dados->totalAjudas
    );

    float larguraCard = painel.width - 40.0f;
    float alturaCard = altura < 680 ? 58.0f : 68.0f;
    float espacamento = altura < 680 ? 8.0f : 10.0f;

    float inicioCards = painel.y + (altura < 680 ? 110.0f : 122.0f);

    float xCard = painel.x + 20.0f;

    float yCard = inicioCards;

    /* Acertos. */
    desenharEstatistica(
        (Rectangle){xCard, yCard, larguraCard, alturaCard},
        "RESPOSTAS CORRETAS",
        acertos,
        COR_VERDE
    );

    yCard += alturaCard + espacamento;

    /* Prêmio conquistado. */
    desenharEstatistica(
        (Rectangle){xCard, yCard, larguraCard, alturaCard},
        "PRÊMIO FINAL",
        premio,
        COR_DOURADO
    );

    yCard += alturaCard + espacamento;

    /* Prêmio garantido. */
    desenharEstatistica(
        (Rectangle){xCard, yCard, larguraCard, alturaCard},
        "PRÊMIO GARANTIDO",
        garantido,
        COR_CIANO
    );

    yCard += alturaCard + espacamento;

    /* Ajudas utilizadas. */
    desenharEstatistica(
        (Rectangle){xCard, yCard, larguraCard, alturaCard},
        "AJUDAS UTILIZADAS",
        ajudas,
        COR_TEXTO
    );

    /* =====================================================
       5. PROGRESSO DA JORNADA
       ===================================================== */

    yCard += alturaCard + 13.0f;

    DrawText(
        TextFormat(
            "DESAFIOS CONCLUÍDOS: %u / %u",
            dados->perguntasRespondidas,
            dados->totalPerguntas
        ),
        (int)xCard,
        (int)yCard,
        10,
        COR_SECUNDARIA
    );

    /* =====================================================
       6. REGISTRO NO RANKING
       ===================================================== */

    float yRanking = yCard + 34.0f;

    DrawLine(
        (int)xCard,
        (int)yRanking - 10,
        (int)(xCard + larguraCard),
        (int)yRanking - 10,
        (Color){57, 112, 158, 120}
    );

    if (!rankingRegistrado) {

        DrawText(
            "HALL DA FAMA",
            (int)xCard,
            (int)yRanking,
            13,
            COR_DOURADO
        );

        DrawText(
            "Registre sua jornada no ranking.",
            (int)xCard,
            (int)yRanking + 22,
            10,
            COR_SECUNDARIA
        );

        Rectangle campo = {
            xCard,
            yRanking + 43,
            larguraCard,
            42
        };

        DrawRectangleRounded(
            campo,
            0.12f,
            8,
            (Color){4, 15, 31, 255}
        );

        DrawRectangleRoundedLinesEx(
            campo,
            0.12f,
            8,
            1.2f,
            COR_BORDA
        );

        const char *nome =
            (nomeJogador != NULL && nomeJogador[0] != '\0')
            ? nomeJogador
            : "Digite seu nome...";

        DrawText(
            nome,
            (int)campo.x + 12,
            (int)campo.y + 13,
            14,
            nomeJogador != NULL && nomeJogador[0] != '\0'
                ? COR_TEXTO
                : COR_SECUNDARIA
        );

        DrawText(
            "ENTER: SALVAR RESULTADO",
            (int)xCard,
            (int)yRanking + 96,
            10,
            COR_CIANO
        );

    } else {

        DrawText(
            "RESULTADO REGISTRADO!",
            (int)xCard,
            (int)yRanking + 8,
            13,
            COR_VERDE
        );

        DrawText(
            mensagemRanking != NULL ? mensagemRanking : "",
            (int)xCard,
            (int)yRanking + 34,
            10,
            COR_SECUNDARIA
        );

        DrawText(
            "ENTER / M: VOLTAR AO MENU",
            (int)xCard,
            (int)yRanking + 65,
            10,
            COR_CIANO
        );
    }

    if (!rankingRegistrado &&
        mensagemRanking != NULL &&
        mensagemRanking[0] != '\0') {

        DrawText(
            mensagemRanking,
            (int)xCard,
            (int)yRanking + 116,
            9,
            RED
        );
    }
}