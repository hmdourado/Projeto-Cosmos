
/**
 * @file jogo.c
 * @brief Implementação da interface gráfica cinematográfica da partida utilizando Raylib.
 */
#include "../../include/telas.h"
#include "../../include/jogo.h"

#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Define as cores utilizadas na composição visual da interface.
static const Color COR_FUNDO_CLARO = { 10, 20, 42, 215 };
static const Color COR_PAINEL = { 10, 23, 47, 225 };
static const Color COR_BOTAO = { 16, 35, 65, 235 };
static const Color COR_BORDA = { 54, 105, 145, 220 };
static const Color COR_DESTAQUE = { 35, 205, 245, 255 };
static const Color COR_DOURADO = { 255, 197, 87, 255 };
static const Color COR_TEXTO = { 241, 247, 255, 255 };
static const Color COR_TEXTO_SEC = { 157, 184, 211, 255 };
static const Color COR_DESABILITADO = { 83, 103, 128, 255 };
static const Color COR_VERDE = { 65, 225, 157, 255 };
static const Color COR_VERMELHO = { 235, 76, 91, 255 };

#define MARGEM_TELA 24.0f
#define ESPACO_ELEMENTOS 14.0f
#define CAMINHO_PERSONAGEM "assets/personagens/apresentadora_cosmos.png"

// Armazena as regiões utilizadas para posicionar os elementos da interface.
typedef struct {
    Rectangle escadaPremios;
    Rectangle painelPergunta;
    Rectangle alternativas[QUANT_ALTERNATIVAS];
    Rectangle painelMensagem;
    Rectangle balaoPersonagem;
    Rectangle areaPersonagem;
    Rectangle identificacaoPersonagem;
    Rectangle botoesAjudas[QUANT_AJUDAS_TELA];
    Rectangle botaoDesistir;
    Rectangle painelPremiosCompacto;
    bool layoutCinematografico;
} LayoutTelaJogo;

// Limita um valor ao intervalo informado para preservar as dimensões dos elementos.
static float limitarValor(float valor, float minimo, float maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

// Calcula a posição dos painéis para a composição cinematográfica e para telas menores.
static LayoutTelaJogo calcularLayoutTelaJogo(void) {
    LayoutTelaJogo layout = {0};

    float largura = (float)GetScreenWidth();
    float altura = (float)GetScreenHeight();
    float margem = largura >= 1400.0f ? 30.0f : MARGEM_TELA;

    layout.layoutCinematografico = largura >= 1180.0f && altura >= 660.0f;

    if (layout.layoutCinematografico) {
        float larguraEscada = limitarValor(largura * 0.125f, 150.0f, 180.0f);
        float larguraPersonagem = limitarValor(largura * 0.25f, 300.0f, 340.0f);
        float espacamento = 18.0f;
        float xPrincipal = margem + larguraEscada + espacamento;
        float xPersonagem = largura - margem - larguraPersonagem - 12.0f;
        float larguraPrincipal = xPersonagem - espacamento - xPrincipal;

        layout.escadaPremios = (Rectangle){
            margem, 96.0f, larguraEscada, altura - 174.0f
        };

        layout.painelPergunta = (Rectangle){
            xPrincipal, 101.0f, larguraPrincipal, 151.0f
        };

        float inicioAlternativas = 267.0f;
        float alturaAlternativa = limitarValor((altura - 500.0f) / 2.0f, 92.0f, 116.0f);
        float larguraAlternativa = (larguraPrincipal - ESPACO_ELEMENTOS) / 2.0f;

        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
            int coluna = i % 2;
            int linha = i / 2;
            layout.alternativas[i] = (Rectangle){
                xPrincipal + coluna * (larguraAlternativa + ESPACO_ELEMENTOS),
                inicioAlternativas + linha * (alturaAlternativa + ESPACO_ELEMENTOS),
                larguraAlternativa,
                alturaAlternativa
            };
        }

        layout.painelMensagem = (Rectangle){
            xPrincipal, inicioAlternativas + 2.0f * alturaAlternativa + ESPACO_ELEMENTOS + 10.0f,
            larguraPrincipal, 43.0f
        };

        // Posiciona a fala abaixo da regiao do rosto para nao encobrir a apresentadora.
        layout.balaoPersonagem = (Rectangle){
            xPersonagem + 8.0f, 205.0f, larguraPersonagem - 16.0f, 58.0f
        };

        // Mantem a personagem abaixo do cabecalho e acima da identificacao inferior.
        float posicaoPersonagemY = 90.0f;
        float margemInferiorPersonagem = 100.0f;

        layout.areaPersonagem = (Rectangle){
            xPersonagem,
            posicaoPersonagemY,
            larguraPersonagem,
            altura - posicaoPersonagemY - margemInferiorPersonagem
        };

        layout.identificacaoPersonagem = (Rectangle){
            xPersonagem + 8.0f, altura - 82.0f, larguraPersonagem - 16.0f, 40.0f
        };

        float larguraControle = (larguraPrincipal - 4.0f * 8.0f) / 5.0f;
        float yControle = altura - 67.0f;

        for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
            layout.botoesAjudas[i] = (Rectangle){
                xPrincipal + i * (larguraControle + 8.0f),
                yControle,
                larguraControle,
                47.0f
            };
        }

        layout.botaoDesistir = (Rectangle){
            xPrincipal + 4.0f * (larguraControle + 8.0f),
            yControle,
            larguraControle,
            47.0f
        };
    } else {
        // Em resoluções menores, prioriza a pergunta e mantém os controles acessíveis.
        float larguraPrincipal = largura - 2.0f * margem;
        float yPergunta = 139.0f;
        float alturaPergunta = 132.0f;
        float yAlternativas = yPergunta + alturaPergunta + 14.0f;
        float alturaControle = 46.0f;
        float yControle = altura - 62.0f;
        float alturaAlternativa = limitarValor(
            (yControle - yAlternativas - 28.0f) / 2.0f, 72.0f, 120.0f
        );
        float larguraAlternativa = (larguraPrincipal - 12.0f) / 2.0f;

        layout.painelPremiosCompacto = (Rectangle){
            margem, 82.0f, larguraPrincipal, 42.0f
        };
        layout.painelPergunta = (Rectangle){
            margem, yPergunta, larguraPrincipal, alturaPergunta
        };

        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
            int coluna = i % 2;
            int linha = i / 2;
            layout.alternativas[i] = (Rectangle){
                margem + coluna * (larguraAlternativa + 12.0f),
                yAlternativas + linha * (alturaAlternativa + 14.0f),
                larguraAlternativa,
                alturaAlternativa
            };
        }

        layout.painelMensagem = (Rectangle){
            margem, yControle - 51.0f, larguraPrincipal, 38.0f
        };

        float larguraControle = (larguraPrincipal - 4.0f * 7.0f) / 5.0f;
        for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
            layout.botoesAjudas[i] = (Rectangle){
                margem + i * (larguraControle + 7.0f),
                yControle,
                larguraControle,
                alturaControle
            };
        }
        layout.botaoDesistir = (Rectangle){
            margem + 4.0f * (larguraControle + 7.0f),
            yControle,
            larguraControle,
            alturaControle
        };
    }

    return layout;
}

// Formata valores monetários utilizando o ponto como separador de milhares.
static void formatarPremio(int valor, char *destino, size_t capacidade) {
    if (destino == NULL || capacidade == 0) return;
    if (valor < 0) valor = 0;

    char numero[32];
    snprintf(numero, sizeof(numero), "%d", valor);

    size_t tamanho = strlen(numero);
    size_t posicao = 0;
    if (capacidade < 4) {
        destino[0] = '\0';
        return;
    }

    destino[posicao++] = 'R';
    destino[posicao++] = '$';
    destino[posicao++] = ' ';

    for (size_t i = 0; i < tamanho; i++) {
        if (i > 0 && (tamanho - i) % 3 == 0) {
            if (posicao + 1 >= capacidade) {
                destino[0] = '\0';
                return;
            }
            destino[posicao++] = '.';
        }
        if (posicao + 1 >= capacidade) {
            destino[0] = '\0';
            return;
        }
        destino[posicao++] = numero[i];
    }
    destino[posicao] = '\0';
}

// Desenha um texto centralizado dentro da área informada.
static void desenharTextoCentralizado(
    const char *texto, Rectangle area, float tamanho, Color cor
) {
    if (texto == NULL || area.width <= 0.0f || area.height <= 0.0f) return;

    Font fonte = GetFontDefault();
    Vector2 medida = MeasureTextEx(fonte, texto, tamanho, 1.0f);
    Vector2 posicao = {
        area.x + (area.width - medida.x) / 2.0f,
        area.y + (area.height - medida.y) / 2.0f
    };
    DrawTextEx(fonte, texto, posicao, tamanho, 1.0f, cor);
}

// Divide o texto em linhas para respeitar os limites da área disponível.
static void desenharTextoMultilinha(
    const char *texto, Rectangle area, float tamanho, Color cor
) {
    if (texto == NULL || texto[0] == '\0' || area.width <= 0.0f || area.height <= 0.0f) return;

    Font fonte = GetFontDefault();
    char linha[512] = "";
    const char *cursor = texto;
    float posicaoY = area.y;

    while (*cursor != '\0') {
        // Ignora espaços antes de identificar a próxima palavra.
        while (*cursor != '\0' && isspace((unsigned char)*cursor)) cursor++;
        if (*cursor == '\0') break;

        const char *inicioPalavra = cursor;
        while (*cursor != '\0' && !isspace((unsigned char)*cursor)) cursor++;

        size_t tamanhoPalavra = (size_t)(cursor - inicioPalavra);
        char palavra[256];
        if (tamanhoPalavra >= sizeof(palavra)) tamanhoPalavra = sizeof(palavra) - 1;
        memcpy(palavra, inicioPalavra, tamanhoPalavra);
        palavra[tamanhoPalavra] = '\0';

        char candidata[sizeof(linha) + sizeof(palavra)];
        if (linha[0] == '\0') {
            snprintf(candidata, sizeof(candidata), "%s", palavra);
        } else {
            snprintf(candidata, sizeof(candidata), "%s %s", linha, palavra);
        }

        Vector2 medida = MeasureTextEx(fonte, candidata, tamanho, 1.0f);
        if (medida.x > area.width && linha[0] != '\0') {
            if (posicaoY + tamanho > area.y + area.height) return;
            DrawTextEx(fonte, linha, (Vector2){area.x, posicaoY}, tamanho, 1.0f, cor);
            posicaoY += tamanho + 6.0f;
            snprintf(linha, sizeof(linha), "%s", palavra);
        } else {
            snprintf(linha, sizeof(linha), "%s", candidata);
        }
    }

    // Desenha a última linha quando ainda existe espaço na área.
    if (linha[0] != '\0' && posicaoY + tamanho <= area.y + area.height) {
        DrawTextEx(fonte, linha, (Vector2){area.x, posicaoY}, tamanho, 1.0f, cor);
    }
}

// Aplica uma borda dupla para dar profundidade aos painéis holográficos.
static void desenharPainelHolografico(Rectangle area, float arredondamento, Color fundo, Color borda) {
    DrawRectangleRounded(area, arredondamento, 12, fundo);
    DrawRectangleRoundedLinesEx(area, arredondamento, 12, 1.4f, borda);

    Rectangle brilho = {
        area.x + 4.0f, area.y + 4.0f,
        area.width - 8.0f, area.height - 8.0f
    };
    DrawRectangleRoundedLinesEx(brilho, arredondamento, 12, 0.7f, (Color){95, 185, 220, 55});
}

// Desenha estrelas em movimento com variação de brilho para criar profundidade.
static void desenharEstrelas(const EstadoVisualTelaJogo *estado) {
    if (estado == NULL) return;

    for (int i = 0; i < QUANT_ESTRELAS_TELA_JOGO; i++) {
        const EstrelaTelaJogo *estrela = &estado->estrelas[i];
        float brilho = 0.5f + 0.5f * sinf(estado->tempo * 1.8f + estrela->fase);
        unsigned char alfa = (unsigned char)(45.0f + brilho * 125.0f);
        Color cor = {175, 218, 255, alfa};
        DrawCircleV(estrela->posicao, estrela->tamanho, cor);
    }
}

// Desenha a identificação superior e o progresso da partida.
static void desenharCabecalho(const DadosTelaJogo *dados) {
    int largura = GetScreenWidth();
    DrawText("COSMOS", 26, 14, 27, COR_DOURADO);
    DrawText("DESAFIO COSMICO  /  CENTRAL DE COMANDO", 28, 45, 11, COR_TEXTO_SEC);

    char progresso[64];
    snprintf(progresso, sizeof(progresso), "PERGUNTA %02u / %02u",
             dados->numeroPergunta, dados->totalPerguntas);

    int larguraTexto = MeasureText(progresso, 17);
    DrawText(progresso, largura - larguraTexto - 28, 22, 17, COR_DESTAQUE);
    DrawLine(24, 69, largura - 24, 69, (Color){68, 119, 157, 170});
}

// Desenha a escada de premiação com os quinze níveis em ordem decrescente.
static void desenharEscadaPremios(
    const DadosTelaJogo *dados,
    Rectangle area
) {
    desenharPainelHolografico(area, 0.08f, (Color){7, 18, 39, 225}, COR_BORDA);
    DrawText("PREMIACAO", (int)area.x + 13, (int)area.y + 13, 15, COR_DOURADO);
    DrawText("15 NIVEIS", (int)area.x + 13, (int)area.y + 33, 10, COR_TEXTO_SEC);

    float inicioY = area.y + 56.0f;
    float espaco = 3.0f;
    float alturaLinha = (area.height - 66.0f - 14.0f * espaco) / 15.0f;
    alturaLinha = limitarValor(alturaLinha, 20.0f, 29.0f);

    // Apresenta primeiro o prêmio máximo e termina no primeiro nível da partida.
    for (int i = 0; i < TOTAL_PERGUNTAS_PARTIDA; i++) {
        int numero = TOTAL_PERGUNTAS_PARTIDA - i;
        int premio = obterPremioPergunta(numero);
        bool atual = dados->numeroPergunta == (unsigned int)numero;
        bool concluida = dados->numeroPergunta > (unsigned int)numero;
        bool marco = numero == 5 || numero == 10;

        float y = inicioY + i * (alturaLinha + espaco);
        Rectangle linha = {
            area.x + 8.0f, y, area.width - 16.0f, alturaLinha
        };

        Color fundo = (Color){13, 29, 52, 205};
        Color borda = (Color){50, 83, 119, 130};
        Color texto = COR_TEXTO_SEC;

        if (concluida) {
            fundo = (Color){13, 57, 57, 210};
            texto = (Color){126, 205, 183, 255};
        }
        if (atual) {
            fundo = (Color){12, 91, 119, 245};
            borda = COR_DESTAQUE;
            texto = WHITE;
        }
        if (marco && !atual) {
            borda = (Color){180, 139, 65, 190};
        }

        DrawRectangleRounded(linha, 0.22f, 6, fundo);
        DrawRectangleRoundedLinesEx(linha, 0.22f, 6, atual ? 1.8f : 0.8f, borda);

        DrawText(TextFormat("%02d", numero), (int)linha.x + 6,
                 (int)(linha.y + (linha.height - 12.0f) / 2.0f), 11, texto);

        char valor[40];
        formatarPremio(premio, valor, sizeof(valor));
        int tamanhoFonte = area.width < 165.0f ? 10 : 11;
        int larguraValor = MeasureText(valor, tamanhoFonte);
        DrawText(valor, (int)(linha.x + linha.width - larguraValor - 6.0f),
                 (int)(linha.y + (linha.height - 12.0f) / 2.0f), tamanhoFonte,
                 atual ? COR_DOURADO : texto);

        if (atual) {
            DrawCircle((int)linha.x + 2, (int)(linha.y + linha.height / 2.0f), 3.0f, COR_DESTAQUE);
        } else if (marco) {
            DrawCircle((int)linha.x + 2, (int)(linha.y + linha.height / 2.0f), 2.5f, COR_DOURADO);
        }
    }
}

// Desenha o painel central do enunciado e destaca o valor da pergunta atual.
static void desenharPainelPergunta(const DadosTelaJogo *dados, Rectangle area) {
    desenharPainelHolografico(area, 0.07f, (Color){8, 22, 47, 238}, COR_DESTAQUE);

    int premio = obterPremioPergunta((int)dados->numeroPergunta);
    char valor[40];
    formatarPremio(premio, valor, sizeof(valor));

    DrawText(TextFormat("PERGUNTA %02u", dados->numeroPergunta),
             (int)area.x + 22, (int)area.y + 15, 14, COR_DESTAQUE);
    int larguraValor = MeasureText(valor, 14);
    DrawText(valor, (int)(area.x + area.width - larguraValor - 22.0f),
             (int)area.y + 15, 14, COR_DOURADO);
    DrawLine((int)area.x + 20, (int)area.y + 40,
             (int)(area.x + area.width - 20), (int)area.y + 40,
             (Color){60, 112, 149, 170});

    Rectangle texto = {
        area.x + 23.0f, area.y + 53.0f,
        area.width - 46.0f, area.height - 65.0f
    };
    float fonte = GetScreenHeight() >= 700 ? 23.0f : 18.0f;
    desenharTextoMultilinha(dados->pergunta->enunciado, texto, fonte, COR_TEXTO);
}

// Desenha uma alternativa com destaque para hover, seleção e resultado da resposta.
static void desenharAlternativa(
    Rectangle area,
    int indice,
    const char *texto,
    bool eliminada,
    float intensidadeHover,
    bool selecionada,
    FeedbackTelaJogo feedback
) {
    float expansao = intensidadeHover * 2.0f;
    area.x -= expansao;
    area.y -= expansao;
    area.width += expansao * 2.0f;
    area.height += expansao * 2.0f;

    Color fundo = COR_BOTAO;
    Color borda = COR_BORDA;
    Color corTexto = COR_TEXTO;

    // Diferencia alternativas eliminadas, selecionadas e respostas confirmadas.
    if (eliminada) {
        fundo = (Color){10, 20, 37, 200};
        borda = (Color){35, 55, 76, 150};
        corTexto = COR_DESABILITADO;
    } else if (selecionada && feedback == FEEDBACK_ACERTO) {
        fundo = (Color){12, 78, 64, 245};
        borda = COR_VERDE;
    } else if (selecionada && feedback == FEEDBACK_ERRO) {
        fundo = (Color){86, 29, 42, 245};
        borda = COR_VERMELHO;
    } else if (selecionada && feedback == FEEDBACK_MARCO) {
        fundo = (Color){91, 67, 25, 245};
        borda = COR_DOURADO;
    } else if (selecionada || intensidadeHover > 0.05f) {
        fundo = (Color){18, 65, 94, 245};
        borda = COR_DESTAQUE;
    }

    desenharPainelHolografico(area, 0.12f, fundo, borda);

    float centroY = area.y + area.height / 2.0f;
    Color corLetra = eliminada ? COR_DESABILITADO : COR_DESTAQUE;
    if (selecionada && feedback == FEEDBACK_ACERTO) corLetra = COR_VERDE;
    if (selecionada && feedback == FEEDBACK_ERRO) corLetra = COR_VERMELHO;
    if (selecionada && feedback == FEEDBACK_MARCO) corLetra = COR_DOURADO;

    DrawCircle((int)area.x + 30, (int)centroY, 17.0f, corLetra);
    char letra[2] = {(char)('A' + indice), '\0'};
    desenharTextoCentralizado(letra,
        (Rectangle){area.x + 13.0f, centroY - 17.0f, 34.0f, 34.0f},
        16.0f, WHITE);

    // Substitui o conteúdo quando a alternativa tiver sido eliminada por uma ajuda.
    if (eliminada) {
        desenharTextoCentralizado("ALTERNATIVA ELIMINADA",
            (Rectangle){area.x + 58.0f, area.y, area.width - 72.0f, area.height},
            13.0f, COR_DESABILITADO);
        return;
    }

    Rectangle areaTexto = {
        area.x + 60.0f, area.y + 12.0f,
        area.width - 76.0f, area.height - 24.0f
    };
    float tamanhoFonte = GetScreenHeight() >= 700 ? 18.0f : 15.0f;
    desenharTextoMultilinha(texto, areaTexto, tamanhoFonte, corTexto);
}

// Desenha a mensagem de status dentro de uma faixa holográfica.
static void desenharMensagem(const DadosTelaJogo *dados, Rectangle area, FeedbackTelaJogo feedback) {
    Color cor = COR_TEXTO_SEC;
    if (feedback == FEEDBACK_ACERTO) cor = COR_VERDE;
    if (feedback == FEEDBACK_ERRO) cor = COR_VERMELHO;
    if (feedback == FEEDBACK_MARCO) cor = COR_DOURADO;

    desenharPainelHolografico(area, 0.18f, (Color){7, 19, 39, 205}, (Color){54, 105, 145, 150});
    const char *mensagem = dados->mensagemStatus;
    if (mensagem == NULL || mensagem[0] == '\0') {
        mensagem = "Escolha uma alternativa para continuar.";
    }
    desenharTextoCentralizado(mensagem, area, 14.0f, cor);
}

// Desenha os botões de ajuda e a opção de desistência com estados de hover.
static void desenharControles(
    const DadosTelaJogo *dados,
    const LayoutTelaJogo *layout,
    const EstadoVisualTelaJogo *estado
) {
    static const char *nomesAjudas[QUANT_AJUDAS_TELA] = {
        "SUPERNOVA", "NASA", "OBSERVATORIO", "ESPECIALISTA"
    };

    for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
        Rectangle botao = layout->botoesAjudas[i];
        bool disponivel = dados->ajudasDisponiveis[i];
        float hover = estado != NULL ? estado->hoverAjudas[i] : 0.0f;
        Color fundo = disponivel
            ? (Color){
                (unsigned char)(COR_BOTAO.r + (COR_DESTAQUE.r - COR_BOTAO.r) * hover),
                (unsigned char)(COR_BOTAO.g + (COR_DESTAQUE.g - COR_BOTAO.g) * hover),
                (unsigned char)(COR_BOTAO.b + (COR_DESTAQUE.b - COR_BOTAO.b) * hover),
                245
              }
            : (Color){12, 22, 40, 220};

        desenharPainelHolografico(botao, 0.20f, fundo,
            disponivel ? (hover > 0.1f ? COR_DESTAQUE : COR_BORDA) : COR_FUNDO_CLARO);
        desenharTextoCentralizado(nomesAjudas[i], botao,
            GetScreenWidth() < 1180 ? 10.0f : 11.0f,
            disponivel ? COR_TEXTO : COR_DESABILITADO);
    }

    // Mantém a desistência visualmente distinta das ajudas.
    Rectangle desistir = layout->botaoDesistir;
    Vector2 mouse = GetMousePosition();
    bool hoverDesistir = CheckCollisionPointRec(mouse, desistir);
    Color fundoDesistir = hoverDesistir
        ? (Color){148, 43, 57, 250}
        : (Color){91, 35, 49, 235};

    desenharPainelHolografico(desistir, 0.20f, fundoDesistir, COR_VERMELHO);
    desenharTextoCentralizado("DESISTIR", desistir, 12.0f, COR_TEXTO);
}

// Desenha a escotilha luminosa e a personagem apresentadora na lateral direita.
static void desenharAreaPersonagem(
    const DadosTelaJogo *dados,
    const EstadoVisualTelaJogo *estado,
    const LayoutTelaJogo *layout
) {
    Rectangle area = layout->areaPersonagem;
    Vector2 centro = {area.x + area.width / 2.0f, area.y + area.height * 0.57f};

    // Cria uma iluminação suave atrás da apresentadora.
    DrawCircleGradient((int)centro.x, (int)centro.y, area.width * 0.46f,
        (Color){15, 115, 175, 65}, (Color){4, 12, 30, 0});
    DrawCircleLines((int)centro.x, (int)centro.y, area.width * 0.39f,
        (Color){64, 172, 215, 75});
    DrawCircleLines((int)centro.x, (int)centro.y, area.width * 0.44f,
        (Color){64, 172, 215, 35});

    // Desenha a personagem quando a textura estiver carregada.
    if (estado != NULL && estado->personagemDisponivel) {
        desenharPersonagemCosmos(&estado->personagem, area);
    } else {
        // Apresenta uma identificação alternativa quando a imagem não estiver disponível.
        DrawCircleGradient((int)centro.x, (int)(area.y + 145.0f), 58.0f,
            (Color){35, 105, 145, 180}, (Color){9, 20, 40, 220});
        DrawCircleLines((int)centro.x, (int)(area.y + 145.0f), 58.0f, COR_DESTAQUE);
        desenharTextoCentralizado("HELENA", (Rectangle){area.x, area.y + 215.0f, area.width, 35.0f}, 20.0f, COR_TEXTO);
        desenharTextoCentralizado("APRESENTADORA COSMOS", (Rectangle){area.x, area.y + 250.0f, area.width, 24.0f}, 10.0f, COR_TEXTO_SEC);
    }

    // Apresenta a fala ou orientação atual da personagem.
    desenharPainelHolografico(layout->balaoPersonagem, 0.18f,
        (Color){7, 20, 42, 230}, (Color){49, 159, 202, 190});

    const char *fala = dados->mensagemStatus;
    if (fala == NULL || fala[0] == '\0') {
        fala = "O universo esta cheio de descobertas. Vamos comecar!";
    }
    Rectangle areaFala = {
        layout->balaoPersonagem.x + 12.0f,
        layout->balaoPersonagem.y + 8.0f,
        layout->balaoPersonagem.width - 24.0f,
        layout->balaoPersonagem.height - 16.0f
    };
    desenharTextoMultilinha(fala, areaFala, 12.0f, COR_TEXTO);

    // Identifica a personagem e sua função no desafio.
    desenharPainelHolografico(layout->identificacaoPersonagem, 0.18f,
        (Color){7, 18, 37, 225}, (Color){70, 126, 164, 180});
    desenharTextoCentralizado("COMANDANTE HELENA", layout->identificacaoPersonagem,
        13.0f, COR_DOURADO);
}

// Desenha um resumo compacto da premiação em resoluções menores.
static void desenharPremiosCompactos(const DadosTelaJogo *dados, Rectangle area) {
    char atual[40];
    char garantido[40];
    char texto[120];
    formatarPremio(dados->premioAtual, atual, sizeof(atual));
    formatarPremio(dados->premioGarantido, garantido, sizeof(garantido));
    snprintf(texto, sizeof(texto), "PREMIO ATUAL: %s     GARANTIDO: %s", atual, garantido);
    desenharPainelHolografico(area, 0.16f, COR_PAINEL, COR_BORDA);
    desenharTextoCentralizado(texto, area, 14.0f, COR_DOURADO);
}

// Inicializa as estrelas, as animações e a textura da personagem.
void inicializarEstadoVisualTelaJogo(EstadoVisualTelaJogo *estado) {
    if (estado == NULL) return;

    *estado = (EstadoVisualTelaJogo){0};
    estado->alternativaSelecionada = -1;
    estado->feedbackResposta = FEEDBACK_NENHUM;

    int largura = GetScreenWidth();
    int altura = GetScreenHeight();

    // Distribui as estrelas inicialmente por toda a área visível.
    for (int i = 0; i < QUANT_ESTRELAS_TELA_JOGO; i++) {
        estado->estrelas[i].posicao = (Vector2){
            (float)GetRandomValue(0, largura > 0 ? largura : 1280),
            (float)GetRandomValue(0, altura > 0 ? altura : 720)
        };
        estado->estrelas[i].velocidade = (float)GetRandomValue(8, 35);
        estado->estrelas[i].tamanho = (float)GetRandomValue(1, 3);
        estado->estrelas[i].fase = (float)GetRandomValue(0, 628) / 100.0f;
    }

    // Carrega a personagem fictícia utilizada na lateral da interface.
    estado->personagemDisponivel = inicializarPersonagemCosmos(
        &estado->personagem,
        CAMINHO_PERSONAGEM
    );
    estado->entrada = 0.0f;
}

// Libera a textura da personagem antes do encerramento da janela Raylib.
void liberarEstadoVisualTelaJogo(EstadoVisualTelaJogo *estado) {
    if (estado == NULL) return;
    liberarPersonagemCosmos(&estado->personagem);
    estado->personagemDisponivel = false;
}

// Atualiza as estrelas, os efeitos de hover, a premiação e a personagem.
void atualizarAnimacaoTelaJogo(
    EstadoVisualTelaJogo *estado,
    const DadosTelaJogo *dados
) {
    if (estado == NULL) return;

    float delta = GetFrameTime();
    delta = limitarValor(delta, 0.0f, 0.05f);
    estado->tempo += delta;

    // Conclui gradualmente a animação de entrada da interface.
    estado->entrada = limitarValor(estado->entrada + delta * 1.5f, 0.0f, 1.0f);

    // Atualiza o movimento vertical das estrelas e as reposiciona ao saírem da tela.
    for (int i = 0; i < QUANT_ESTRELAS_TELA_JOGO; i++) {
        EstrelaTelaJogo *estrela = &estado->estrelas[i];
        estrela->posicao.y += estrela->velocidade * delta;
        if (estrela->posicao.y > GetScreenHeight()) {
            estrela->posicao.y = 0.0f;
            estrela->posicao.x = (float)GetRandomValue(0,
                GetScreenWidth() > 0 ? GetScreenWidth() : 1);
        }
    }

    // Atualiza a interação dos botões quando existe uma pergunta ativa.
    if (dados != NULL && dados->pergunta != NULL) {
        LayoutTelaJogo layout = calcularLayoutTelaJogo();
        Vector2 mouse = GetMousePosition();

        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
            float alvo = (!dados->alternativasEliminadas[i] &&
                CheckCollisionPointRec(mouse, layout.alternativas[i])) ? 1.0f : 0.0f;
            estado->hoverAlternativas[i] +=
                (alvo - estado->hoverAlternativas[i]) * fminf(delta * 12.0f, 1.0f);
        }

        for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
            float alvo = (dados->ajudasDisponiveis[i] &&
                CheckCollisionPointRec(mouse, layout.botoesAjudas[i])) ? 1.0f : 0.0f;
            estado->hoverAjudas[i] +=
                (alvo - estado->hoverAjudas[i]) * fminf(delta * 12.0f, 1.0f);
        }

        // Suaviza o preenchimento do indicador de avanço da partida.
        float progressoAlvo = dados->totalPerguntas > 0
            ? (float)dados->numeroPergunta / (float)dados->totalPerguntas
            : 0.0f;
        progressoAlvo = limitarValor(progressoAlvo, 0.0f, 1.0f);
        estado->progressoPremio +=
            (progressoAlvo - estado->progressoPremio) * fminf(delta * 5.0f, 1.0f);
    }

    // Reduz o tempo do feedback e retorna a personagem ao estado normal ao concluí-lo.
    if (estado->tempoFeedback > 0.0f) {
        estado->tempoFeedback -= delta;
        if (estado->tempoFeedback <= 0.0f) {
            estado->tempoFeedback = 0.0f;
            estado->feedbackResposta = FEEDBACK_NENHUM;
            estado->alternativaSelecionada = -1;
        }
    }

    // Aplica uma vibração temporária aos painéis quando a resposta estiver incorreta.
    if (estado->feedbackResposta == FEEDBACK_ERRO) {
        estado->deslocamentoPainel =
            sinf(estado->tempo * 45.0f) * estado->tempoFeedback * 5.0f;
    } else {
        estado->deslocamentoPainel = 0.0f;
    }

    // Atualiza a animação contínua ou temporária da apresentadora.
    atualizarPersonagemCosmos(&estado->personagem, delta);
}

// Registra os efeitos visuais e a reação da personagem para cada resultado.
void registrarFeedbackTelaJogo(
    EstadoVisualTelaJogo *estado,
    FeedbackTelaJogo feedback
) {
    if (estado == NULL) return;

    estado->feedbackResposta = feedback;

    switch (feedback) {
        case FEEDBACK_ACERTO:
            estado->tempoFeedback = 1.0f;
            definirAcaoPersonagem(&estado->personagem, PERSONAGEM_ACERTO);
            break;
        case FEEDBACK_ERRO:
            estado->tempoFeedback = 1.2f;
            definirAcaoPersonagem(&estado->personagem, PERSONAGEM_ERRO);
            break;
        case FEEDBACK_MARCO:
            estado->tempoFeedback = 2.0f;
            definirAcaoPersonagem(&estado->personagem, PERSONAGEM_MARCO);
            break;
        default:
            estado->tempoFeedback = 0.0f;
            estado->alternativaSelecionada = -1;
            definirAcaoPersonagem(&estado->personagem, PERSONAGEM_PARADO);
            break;
    }
}

// Identifica cliques nas alternativas, ajudas e botão de desistência.
AcaoTelaJogo atualizarTelaJogo(
    const DadosTelaJogo *dados,
    EstadoVisualTelaJogo *estado
) {
    if (dados == NULL || dados->pergunta == NULL) return ACAO_TELA_JOGO_NENHUMA;

    // Aguarda a conclusão do feedback visual antes de permitir outra resposta.
    if (estado != NULL && estado->tempoFeedback > 0.0f) {
        return ACAO_TELA_JOGO_NENHUMA;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return ACAO_TELA_JOGO_NENHUMA;
    }

    LayoutTelaJogo layout = calcularLayoutTelaJogo();
    Vector2 mouse = GetMousePosition();

    // Verifica se o jogador selecionou uma alternativa ainda disponível.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        if (dados->alternativasEliminadas[i]) continue;
        if (CheckCollisionPointRec(mouse, layout.alternativas[i])) {
            if (estado != NULL) estado->alternativaSelecionada = i;
            return (AcaoTelaJogo)(ACAO_TELA_JOGO_RESPOSTA_A + i);
        }
    }

    static const AcaoTelaJogo acoesAjudas[QUANT_AJUDAS_TELA] = {
        ACAO_TELA_JOGO_AJUDA_SUPERNOVA,
        ACAO_TELA_JOGO_AJUDA_NASA,
        ACAO_TELA_JOGO_AJUDA_OBSERVATORIO,
        ACAO_TELA_JOGO_AJUDA_ESPECIALISTA
    };

    // Verifica a seleção de uma ajuda que esteja disponível.
    for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
        if (!dados->ajudasDisponiveis[i]) continue;
        if (CheckCollisionPointRec(mouse, layout.botoesAjudas[i])) {
            return acoesAjudas[i];
        }
    }

    // Verifica se o jogador selecionou a opção de desistência.
    if (CheckCollisionPointRec(mouse, layout.botaoDesistir)) {
        return ACAO_TELA_JOGO_DESISTIR;
    }

    return ACAO_TELA_JOGO_NENHUMA;
}

// Desenha os elementos da interface utilizando os dados da partida e o estado visual.
void desenharTelaJogo(
    const DadosTelaJogo *dados,
    const EstadoVisualTelaJogo *estado
) {
    int largura = GetScreenWidth();
    int altura = GetScreenHeight();

    // Aplica uma camada escura translúcida para preservar o vídeo de fundo do menu.
    DrawRectangleGradientV(0, 0, largura, altura,
        (Color){5, 10, 25, 218}, (Color){5, 13, 32, 198});

    // Desenha as estrelas animadas sobre o cenário espacial.
    desenharEstrelas(estado);

    if (dados == NULL || dados->pergunta == NULL) {
        desenharTextoCentralizado("NENHUMA PERGUNTA DISPONIVEL",
            (Rectangle){0, 0, (float)largura, (float)altura}, 24.0f, COR_TEXTO);
        return;
    }

    LayoutTelaJogo layout = calcularLayoutTelaJogo();
    float deslocamento = estado != NULL ? estado->deslocamentoPainel : 0.0f;

    // Aplica o deslocamento temporário aos painéis centrais durante o feedback de erro.
    layout.painelPergunta.x += deslocamento;
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        layout.alternativas[i].x += deslocamento;
    }

    // Desenha o cabeçalho e a escada de premiação na composição cinematográfica.
    desenharCabecalho(dados);
    if (layout.layoutCinematografico) {
        desenharEscadaPremios(dados, layout.escadaPremios);
        desenharAreaPersonagem(dados, estado, &layout);
    } else {
        desenharPremiosCompactos(dados, layout.painelPremiosCompacto);
    }

    // Desenha o enunciado com o prêmio correspondente à pergunta atual.
    desenharPainelPergunta(dados, layout.painelPergunta);

    // Desenha as quatro alternativas com animações de hover e feedback.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        float hover = estado != NULL ? estado->hoverAlternativas[i] : 0.0f;
        bool selecionada = estado != NULL && estado->alternativaSelecionada == i;
        FeedbackTelaJogo feedback = estado != NULL
            ? estado->feedbackResposta : FEEDBACK_NENHUM;

        desenharAlternativa(
            layout.alternativas[i], i,
            dados->pergunta->alternativas[i],
            dados->alternativasEliminadas[i],
            hover, selecionada, feedback
        );
    }

    // Apresenta a mensagem de orientação ou o resultado da última ação.
    desenharMensagem(dados, layout.painelMensagem,
        estado != NULL ? estado->feedbackResposta : FEEDBACK_NENHUM);

    // Finaliza a composição com as ajudas e a opção de desistência.
    desenharControles(dados, &layout, estado);

    // Aplica uma transição de entrada gradual quando a tela é inicializada.
    if (estado != NULL && estado->entrada < 1.0f) {
        unsigned char opacidade = (unsigned char)((1.0f - estado->entrada) * 255.0f);
        DrawRectangle(0, 0, largura, altura, (Color){0, 0, 0, opacidade});
    }
}
