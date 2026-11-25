/**
 * @file menu.c
 * @author Arthur Morais Palitot Fernandes
 * @brief Menu principal, CRUD grafico do banco e navegacao do Cosmos com Raylib.
 * @version 1.1.0
 * @date 24-11-2026
 *
 * @copyright Copyright (c) 2026
 */

#include "../include/raylib.h"
#include "../include/video.h"
#include "../include/menu.h"
#include "../include/audio.h"
#include "../include/cadastro.h"
#include "../include/arquivo.h"
#include "../include/jogo.h"
#include "../include/ajuda.h"
#include "../include/telas.h"
#include "../include/ranking.h"
#include "../include/fontes.h"
#include "../include/vitoria.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define CAMINHO_VIDEO       "assets/14014441_1280_720_30fps.mp4"
#define CAMINHO_FONTE       "assets/fonts/spacerangerlaser.ttf"
#define CAMINHO_BINARIO     "dados/perguntas.dat"
#define CAMINHO_CSV_EXPORT  "dados/perguntas_exportadas.csv"
#define CAMINHO_RANKING     RANKING_ARQUIVO
#define TAM_MENSAGEM_MENU   192
#define TAM_MENSAGEM_FORM   160

/* Paleta principal do Cosmos. */
static const Color COR_FUNDO       = { 4, 10, 24, 255 };
static const Color COR_PAINEL      = { 7, 19, 40, 232 };
static const Color COR_PAINEL_ALT  = { 11, 29, 56, 238 };
static const Color COR_BORDA       = { 57, 112, 158, 210 };
static const Color COR_AZUL        = { 46, 190, 240, 255 };
static const Color COR_CIANO       = { 111, 225, 255, 255 };
static const Color COR_DOURADO     = { 255, 198, 91, 255 };
static const Color COR_TEXTO       = { 239, 247, 255, 255 };
static const Color COR_TEXTO_SEC   = { 153, 181, 210, 255 };
static const Color COR_VERDE       = { 76, 222, 164, 255 };
static const Color COR_VERMELHO    = { 239, 91, 104, 255 };

/* Telas acessiveis pelo menu principal. */
typedef enum {
    MENU_INICIO = 0,
    MENU_BANCO,
    MENU_RANKING,
    MENU_AJUDA,
    MENU_CONFIGURACOES
} TelaMenu;

typedef struct {
    bool ativo;
    bool edicao;
    int idOriginal;
    int campoAtivo; /* 0 = enunciado; 1..4 = alternativas A..D. */
    Pergunta pergunta;
} FormularioPergunta;


/**
 * @brief Retorna o nome legivel de um nivel de dificuldade.
 * 
 * @param nivel 
 * @return const char* :  nivel definido 
 */
static const char *obterNomeNivelMenu(NivelDificuldade nivel) {
    switch (nivel) {
        case MUITO_FACIL: return "MUITO FACIL";
        case FACIL: return "FACIL";
        case MEDIO: return "MEDIO";
        case DIFICIL: return "DIFICIL";
        case MUITO_DIFICIL: return "MUITO DIFICIL";
        default: return "NAO DEFINIDO";
    }
}

/**
 * @brief Painel com borda suave, brilho superior e transparencia.
 * 
 * @param area 
 * @param fundo 
 * @param borda 
 */
static void desenharPainel(Rectangle area, Color fundo, Color borda) {
    DrawRectangleRounded(area, 0.075f, 12, fundo);
    DrawRectangleRoundedLinesEx(area, 0.075f, 12, 1.4f, borda);
    DrawLineEx(
        (Vector2){area.x + 18.0f, area.y + 2.0f},
        (Vector2){area.x + area.width - 18.0f, area.y + 2.0f},
        1.0f,
        (Color){COR_CIANO.r, COR_CIANO.g, COR_CIANO.b, 75}
    );
}


/**
 * @brief Desenha botao (em dourado) com os estados hover, normal e pressionado
 * 
 * @param area 
 * @param titulo 
 * @param descricao 
 * @param atalho 
 * @param destaque 
 * @return true  
 * @return false 
 */
static bool desenharBotaoPremium(
    Rectangle area,
    const char *titulo,
    const char *descricao,
    const char *atalho,
    bool destaque
) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, area);
    bool pressionado = hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    Color fundo = destaque
        ? (hover ? (Color){18, 76, 112, 248} : (Color){10, 49, 82, 242})
        : (hover ? (Color){19, 48, 78, 246} : (Color){8, 25, 49, 232});
    Color borda = destaque ? COR_AZUL : COR_BORDA;

    if (pressionado) area.y += 2.0f;

    DrawRectangleRounded(area, 0.12f, 10, fundo);
    DrawRectangleRoundedLinesEx(
        area,
        0.12f,
        10,
        hover ? 2.2f : 1.1f,
        hover ? COR_CIANO : borda
    );
    DrawRectangleRounded(
        (Rectangle){area.x + 1.0f, area.y + 1.0f, 4.0f, area.height - 2.0f},
        0.5f,
        5,
        destaque ? COR_DOURADO : COR_AZUL
    );

    int fonteTitulo = area.height < 44.0f ? 15 : 20;
    int yTitulo = area.height < 44.0f ? 9 : 12;
    DrawTextEx(obterFonteTitulo(),titulo, (Vector2){(int)area.x + 20, (int)area.y + yTitulo}, fonteTitulo,2, COR_TEXTO);
    if (descricao != NULL) {
        DrawTextEx(obterFonteTitulo(), descricao, (Vector2){(int)area.x + 20, (int)area.y + 39}, 12,1, COR_TEXTO_SEC);
    }
    if (atalho != NULL) {
        DrawText(
                 atalho,
                 (int)(area.x + area.width - MeasureText(atalho, 11) - 15),
                (int)area.y + 15,11, 
                hover ? COR_CIANO : COR_TEXTO_SEC);

    }

    return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

/**
 * @brief Desenha uma faixa de cabecalho padronizada para as telas internas.
 * 
 * @param secao 
 * @param subtitulo 
 */
static void desenharCabecalhoTela(const char *secao, const char *subtitulo) {
    float largura = (float)GetScreenWidth();
    DrawTextEx(obterFonteLogo(),"COSMOS", (Vector2){ 42, 28,}, 28, 2, COR_TEXTO);
    DrawText("CENTRAL DE COMANDO", 44, 61, 10, COR_CIANO);
    DrawText(secao, 42, 112, 30, COR_TEXTO);
    DrawText(subtitulo, 44, 150, 14, COR_TEXTO_SEC);
    DrawLine(42, 180, (int)largura - 42, 180, (Color){57, 112, 158, 115});
}

/**
 * @brief Desenha estrelas discretas quando o video de fundo nao estiver disponivel.
 * 
 */
static void desenharFundoAlternativo(void) {
    int largura = GetScreenWidth();
    int altura = GetScreenHeight();

    ClearBackground(COR_FUNDO);
    if (largura <= 0 || altura <= 0) return;

    for (int i = 0; i < 95; i++) {
        int x = (i * 137 + 41) % largura;
        int y = (i * 83 + 19) % altura;
        unsigned char alpha = (unsigned char)(75 + (i % 5) * 25);
        DrawCircle((float)x, (float)y, (float)(i % 3 + 1) * 0.7f,
                   (Color){150, 198, 235, alpha});
    }
}


/**
 * @brief Desenha somente o quadro atual do video, sem filtros ou overlays no menu
 * 
 * @param fundo 
 * @param videoDisponivel 
 */
static void desenharFundoMenu(VideoBackground *fundo, bool videoDisponivel) {
    if (videoDisponivel) VideoDraw(fundo);
    else desenharFundoAlternativo();
}


/**
 * @brief Transfere o estado da partida para a estrutura consumida pela tela Raylib.
 * 
 * @param partida 
 * @param dados 
 * @param mensagem 
 */
static void atualizarDadosTelaJogo(
    const Partida *partida,
    DadosTelaJogo *dados,
    const char *mensagem
) {
    if (partida == NULL || dados == NULL) return;

    *dados = (DadosTelaJogo){0};
    dados->pergunta = obterPerguntaAtual(partida);
    if (dados->pergunta == NULL) return;

    dados->numeroPergunta = (unsigned int)partida->indiceAtual + 1;
    dados->totalPerguntas = TOTAL_PERGUNTAS_PARTIDA;
    dados->premioAtual = partida->premioAtual;
    dados->premioGarantido = partida->premioGarantido;
    dados->mensagemStatus = mensagem;

    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        dados->ajudasDisponiveis[i] = !partida->ajudas.usadas[i];
    }
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        dados->alternativasEliminadas[i] = partida->ajudas.eliminadas[i] != 0;
    }
}

/**
 * @brief Converte uma acao visual no tipo de ajuda reconhecido pelo modulo de regras.
 * 
 * @param acao 
 * @param tipo 
 * @return true 
 * @return false 
 */
static bool obterTipoAjudaMenu(AcaoTelaJogo acao, TipoAjuda *tipo) {
    if (tipo == NULL) return false;

    switch (acao) {
        case ACAO_TELA_JOGO_AJUDA_SUPERNOVA: *tipo = AJUDA_SUPERNOVA; return true;
        case ACAO_TELA_JOGO_AJUDA_NASA: *tipo = AJUDA_MISSAO; return true;
        case ACAO_TELA_JOGO_AJUDA_OBSERVATORIO: *tipo = AJUDA_TELESCOPIO; return true;
        case ACAO_TELA_JOGO_AJUDA_ESPECIALISTA: *tipo = AJUDA_ESPECIALISTA; return true;
        default: return false;
    }
}

/**
 * @brief Converte a opcao selecionada na letra esperada por responderPergunta()
 * 
 * @param acao AcaoTelaJogo
 * @return char : opcao para ser param da funcao responderPergunta()
 */
static char obterLetraRespostaMenu(AcaoTelaJogo acao) {
    switch (acao) {
        case ACAO_TELA_JOGO_RESPOSTA_A: return 'A';
        case ACAO_TELA_JOGO_RESPOSTA_B: return 'B';
        case ACAO_TELA_JOGO_RESPOSTA_C: return 'C';
        case ACAO_TELA_JOGO_RESPOSTA_D: return 'D';
        default: return '\0';
    }
}


/**
 * @brief Atualiza os efeitos sonoros apenas quando o cursor entra em um controle.
 * 
 * @param audio 
 * @param audioDisponivel 
 * @param estado 
 * @param hoverAlternativaAnterior 
 * @param hoverAjudaAnterior 
 */
static void atualizarSonsHoverJogo(
    AudioJogo *audio,
    bool audioDisponivel,
    const EstadoVisualTelaJogo *estado,
    bool hoverAlternativaAnterior[QUANT_ALTERNATIVAS],
    bool hoverAjudaAnterior[QUANT_AJUDAS_TELA]
) {
    if (!audioDisponivel || estado == NULL) return;

    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        bool hover = estado->hoverAlternativas[i] >= 0.75f;
        if (hover && !hoverAlternativaAnterior[i]) reproduzirEfeitoAudio(audio, EFEITO_HOVER);
        hoverAlternativaAnterior[i] = hover;
    }
    for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
        bool hover = estado->hoverAjudas[i] >= 0.75f;
        if (hover && !hoverAjudaAnterior[i]) reproduzirEfeitoAudio(audio, EFEITO_HOVER);
        hoverAjudaAnterior[i] = hover;
    }
}

/**
 * @brief  Apresenta o resultado final e a entrada do nome para registrar no ranking.
 * 
 * @param partida 
 * @param vitoria 
 * @param desistencia 
 * @param nome 
 * @param rankingRegistrado 
 * @param mensagemRanking 
 */
static void desenharResultadoPartida(
    const Partida *partida,
    bool vitoria,
    bool desistencia,
    const char *nome,
    bool rankingRegistrado,
    const char *mensagemRanking
) {
    float largura = (float)GetScreenWidth();
    float altura = (float)GetScreenHeight();
    Rectangle painel = {largura / 2.0f - 285.0f, altura / 2.0f - 205.0f, 570.0f, 410.0f};

    DrawRectangle(0, 0, (int)largura, (int)altura, (Color){0, 0, 0, 205});
    desenharPainel(painel, (Color){7, 20, 42, 250}, COR_AZUL);

    const char *titulo = vitoria ? "JORNADA CONCLUIDA" :
                         desistencia ? "JORNADA ENCERRADA" : "FIM DE JOGO";
    Color corTitulo = vitoria ? COR_VERDE : desistencia ? COR_DOURADO : COR_VERMELHO;

    DrawText(titulo,
             (int)(painel.x + (painel.width - MeasureText(titulo, 27)) / 2.0f),
             (int)painel.y + 34, 27, corTitulo);
    DrawText(TextFormat("ACERTOS: %d / %d", partida->acertos, TOTAL_PERGUNTAS_PARTIDA),
             (int)painel.x + 55, (int)painel.y + 94, 19, COR_TEXTO);
    DrawText(TextFormat("PREMIO FINAL: R$ %d", partida->premioAtual),
             (int)painel.x + 55, (int)painel.y + 128, 19, COR_DOURADO);

    if (!rankingRegistrado) {
        DrawText("REGISTRE SEU RESULTADO NO RANKING", (int)painel.x + 55,
                 (int)painel.y + 190, 13, COR_CIANO);
        DrawRectangleRounded(
            (Rectangle){painel.x + 55, painel.y + 220, painel.width - 110, 48},
            0.12f, 8, (Color){5, 14, 31, 255}
        );
        DrawRectangleRoundedLinesEx(
            (Rectangle){painel.x + 55, painel.y + 220, painel.width - 110, 48},
            0.12f, 8, 1.2f, COR_BORDA
        );
        DrawText(nome[0] ? nome : "Digite seu nome...",
                 (int)painel.x + 70, (int)painel.y + 234, 18,
                 nome[0] ? COR_TEXTO : COR_TEXTO_SEC);
        DrawText("ENTER: SALVAR RESULTADO", (int)painel.x + 55,
                 (int)painel.y + 286, 12, COR_TEXTO_SEC);
        if (mensagemRanking != NULL && mensagemRanking[0] != '\0') {
            DrawText(mensagemRanking, (int)painel.x + 55,
                     (int)painel.y + 320, 12, COR_VERMELHO);
        }
    } else {
        DrawText(mensagemRanking, (int)painel.x + 55,
                 (int)painel.y + 232, 15, COR_VERDE);
        DrawText("M / ENTER: VOLTAR AO MENU", (int)painel.x + 55,
                 (int)painel.y + 286, 12, COR_TEXTO_SEC);
    }
}

/**
 * @brief Executa uma partida real usando os modulos de regras jogo.c e a interface cinematografica. 
 * 
 * @param cadastro 
 * @param audio 
 * @param audioDisponivel 
 * @param fundo 
 * @param videoDisponivel 
 */
static void executarPartidaRaylib(
    Cadastro *cadastro,
    AudioJogo *audio,
    bool audioDisponivel,
    VideoBackground *fundo,
    bool videoDisponivel
) {

    EstadoVisualVitoria estadoVitoria = {0};
    DadosTelaVitoria dadosVitoria = {0};

    bool telaVitoriaInicializada = false;

    if (cadastro == NULL || cadastro->quantidade == 0) {
        TraceLog(LOG_WARNING, "Naão há perguntas cadastradas para iniciar a partida.");
        return;
    }

    Partida partida;
    inicializarPartida(&partida);
    ResultadoInicio inicio = iniciarPartida(&partida, cadastro);
    
    if (inicio != INICIO_SUCESSO) {
        TraceLog(LOG_WARNING, "Não foi possivel iniciar a partida. Codigo: %d", inicio);
        return;
    }
    // Carrega o filtro visual utilizado no fundo da partida.
    Shader shaderCinza = {0};
    bool shaderCinzaDisponivel = false;

    if (FileExists("assets/shaders/escala_cinza.fs")) {
        shaderCinza = LoadShader(
            NULL,
            "assets/shaders/escala_cinza.fs"
        );

        shaderCinzaDisponivel = shaderCinza.id > 0;
    }

    if (!shaderCinzaDisponivel) {
        TraceLog(LOG_WARNING, "Shader de escala de cinza indisponivel.");
    }
    DadosTelaJogo dadosTela = {0};
    atualizarDadosTelaJogo(&partida, &dadosTela, "Selecione uma alternativa.");
    EstadoVisualTelaJogo estadoVisual = {0};
    inicializarEstadoVisualTelaJogo(&estadoVisual);

    bool partidaEncerrada = false;
    bool vitoria = false;
    bool desistencia = false;
    bool aguardandoProximaPergunta = false;
    bool rankingRegistrado = false;
    double instanteProximaPergunta = 0.0;
    double fimMensagem = 0.0;
    Pergunta perguntaEmFeedback = {0};
    char mensagemAjuda[256] = "";
    char nomeJogador[RANKING_TAM_NOME] = "";
    char mensagemRanking[96] = "";
    bool hoverAlternativaAnterior[QUANT_ALTERNATIVAS] = {false};
    bool hoverAjudaAnterior[QUANT_AJUDAS_TELA] = {false};

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F1)) break;

        if (videoDisponivel) VideoUpdate(fundo);
        if (audioDisponivel) atualizarAudioJogo(audio);
        atualizarAnimacaoTelaJogo(&estadoVisual, &dadosTela);

        if (partidaEncerrada && vitoria && telaVitoriaInicializada) {
             atualizarEstadoVisualVitoria(&estadoVitoria);
        }

        if (aguardandoProximaPergunta && GetTime() >= instanteProximaPergunta) {
            atualizarDadosTelaJogo(&partida, &dadosTela, "Nova pergunta. Boa sorte!");
            registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_NENHUM);
            aguardandoProximaPergunta = false;
            fimMensagem = GetTime() + 1.4;
            for (int i = 0; i < QUANT_ALTERNATIVAS; i++) hoverAlternativaAnterior[i] = false;
            for (int i = 0; i < QUANT_AJUDAS_TELA; i++) hoverAjudaAnterior[i] = false;
            if (audioDisponivel) reproduzirEfeitoAudio(audio, EFEITO_TRANSICAO);
        }

        if (!partidaEncerrada && !aguardandoProximaPergunta) {
            if (fimMensagem > 0.0 && GetTime() >= fimMensagem) {
                dadosTela.mensagemStatus = "Selecione uma alternativa.";
                mensagemAjuda[0] = '\0';
                fimMensagem = 0.0;
            }

            atualizarSonsHoverJogo(audio, audioDisponivel, &estadoVisual,
                                   hoverAlternativaAnterior, hoverAjudaAnterior);
            AcaoTelaJogo acao = atualizarTelaJogo(&dadosTela, &estadoVisual);
            if (audioDisponivel && acao != ACAO_TELA_JOGO_NENHUMA) {
                reproduzirEfeitoAudio(audio, EFEITO_SELECAO);
            }

            char resposta = obterLetraRespostaMenu(acao);
            if (resposta != '\0') {
                int numeroRespondida = partida.indiceAtual + 1;
                if (dadosTela.pergunta != NULL) perguntaEmFeedback = *dadosTela.pergunta;
                estadoVisual.alternativaSelecionada = resposta - 'A';
                ResultadoResposta resultado = responderPergunta(&partida, resposta);

                switch (resultado) {
                    case RESPOSTA_CORRETA: {
                        bool marco = numeroRespondida == 5 || numeroRespondida == 10;
                        registrarFeedbackTelaJogo(&estadoVisual,
                            marco ? FEEDBACK_MARCO : FEEDBACK_ACERTO);
                        if (audioDisponivel) {
                            reproduzirEfeitoAudio(audio,
                                marco ? EFEITO_MARCO_SEGURANCA : EFEITO_RESPOSTA_CORRETA);
                        }
                        dadosTela.pergunta = &perguntaEmFeedback;
                        dadosTela.numeroPergunta = (unsigned int)numeroRespondida;
                        dadosTela.totalPerguntas = TOTAL_PERGUNTAS_PARTIDA;
                        dadosTela.premioAtual = partida.premioAtual;
                        dadosTela.premioGarantido = partida.premioGarantido;
                        dadosTela.mensagemStatus = marco
                            ? "Marco de seguranca conquistado!" : "Resposta correta!";
                        aguardandoProximaPergunta = true;
                        instanteProximaPergunta = GetTime() + (marco ? 1.9 : 1.2);
                        break;
                    }
                    case RESPOSTA_VITORIA:
                        registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_ACERTO);
                        definirAcaoPersonagem(&estadoVisual.personagem, PERSONAGEM_VITORIA);
                        if (audioDisponivel) reproduzirEfeitoAudio(audio, EFEITO_VITORIA);
                        // Prepara as estatisticas da jornada.
                        dadosVitoria.perguntasRespondidas = TOTAL_PERGUNTAS_PARTIDA;
                        dadosVitoria.totalPerguntas = TOTAL_PERGUNTAS_PARTIDA;

                        dadosVitoria.acertos = partida.acertos;
                        dadosVitoria.premioFinal = partida.premioAtual;
                        dadosVitoria.premioGarantido = partida.premioGarantido;

                        dadosVitoria.totalAjudas = TOTAL_AJUDAS;
                        dadosVitoria.ajudasUtilizadas = 0;

                        // Conta as ajudas utilizadas durante a partida.
                        for (int i = 0; i < TOTAL_AJUDAS; i++) {
                            if (partida.ajudas.usadas[i]) {
                                dadosVitoria.ajudasUtilizadas++;
                            }
                        }

                        // Inicializa o video de comemoracao apenas uma vez.
                        inicializarEstadoVisualVitoria(
                            &estadoVitoria,
                            audioDisponivel
                        );
                        telaVitoriaInicializada = true;
                        dadosTela.premioAtual = partida.premioAtual;
                        dadosTela.premioGarantido = partida.premioGarantido;
                        dadosTela.mensagemStatus = "Todas as perguntas foram respondidas!";
                        partidaEncerrada = true;
                        vitoria = true;
                        
                        break;
                    case RESPOSTA_INCORRETA:
                        registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_ERRO);
                        if (audioDisponivel) reproduzirEfeitoAudio(audio, EFEITO_RESPOSTA_INCORRETA);
                        dadosTela.premioAtual = partida.premioAtual;
                        dadosTela.premioGarantido = partida.premioGarantido;
                        dadosTela.mensagemStatus = "Resposta incorreta!";
                        partidaEncerrada = true;
                        break;
                    case RESPOSTA_INVALIDA:
                        dadosTela.mensagemStatus = "Alternativa invalida.";
                        fimMensagem = GetTime() + 1.5;
                        break;
                    case RESPOSTA_NAO_PERMITIDA:
                        TraceLog(LOG_WARNING, "Resposta nao permitida no estado atual.");
                        break;
                }
            } else if (acao == ACAO_TELA_JOGO_DESISTIR) {
                if (desistirPartida(&partida)) {
                    dadosTela.premioAtual = partida.premioAtual;
                    dadosTela.premioGarantido = partida.premioGarantido;
                    dadosTela.mensagemStatus = "Partida encerrada por desistencia.";
                    partidaEncerrada = true;
                    desistencia = true;
                }
            } else {
                TipoAjuda tipoAjuda;
                if (obterTipoAjudaMenu(acao, &tipoAjuda)) {
                    const Pergunta *pergunta = obterPerguntaAtual(&partida);
                    ResultadoAjuda resultadoAjuda = aplicarAjuda(&partida.ajudas, pergunta, tipoAjuda);
                    if (resultadoAjuda == AJUDA_SUCESSO && pergunta != NULL) {
                        for (int i = 0; i < TOTAL_AJUDAS; i++) {
                            dadosTela.ajudasDisponiveis[i] = !partida.ajudas.usadas[i];
                        }
                        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
                            dadosTela.alternativasEliminadas[i] = partida.ajudas.eliminadas[i] != 0;
                        }
                        switch (tipoAjuda) {
                            case AJUDA_SUPERNOVA:
                                snprintf(mensagemAjuda, sizeof(mensagemAjuda),
                                    "Supernova: duas alternativas incorretas eliminadas.");
                                break;
                            case AJUDA_MISSAO:
                            case AJUDA_TELESCOPIO: {
                                const char *pista = obterTextoAjuda(pergunta, tipoAjuda);
                                snprintf(mensagemAjuda, sizeof(mensagemAjuda), "%s",
                                    pista != NULL ? pista : "Pista indisponivel.");
                                break;
                            }
                            case AJUDA_ESPECIALISTA:
                                snprintf(mensagemAjuda, sizeof(mensagemAjuda),
                                    "Especialista sugere %c (%d%% de chance).",
                                    partida.ajudas.sugestaoEspecialista,
                                    obterChanceEspecialista(pergunta->nivel));
                                break;
                            default:
                                snprintf(mensagemAjuda, sizeof(mensagemAjuda), "Ajuda aplicada.");
                                break;
                        }
                        dadosTela.mensagemStatus = mensagemAjuda;
                        fimMensagem = GetTime() + 5.5;
                    } else {
                        const char *erro = "Nao foi possivel aplicar a ajuda.";
                        switch (resultadoAjuda) {
                            case AJUDA_JA_USADA: erro = "Esta ajuda ja foi utilizada nesta partida."; break;
                            case AJUDA_SEM_CONTEUDO: erro = "Nao ha conteudo para esta pergunta."; break;
                            case AJUDA_NAO_PERMITIDA: erro = "Esta ajuda nao pode ser aplicada agora."; break;
                            case AJUDA_INVALIDA: erro = "Nao foi possivel validar esta ajuda."; break;
                            case AJUDA_SUCESSO: break;
                        }
                        snprintf(mensagemAjuda, sizeof(mensagemAjuda), "%s", erro);
                        dadosTela.mensagemStatus = mensagemAjuda;
                        fimMensagem = GetTime() + 2.5;
                    }
                }
            }
        } else if (partidaEncerrada && !rankingRegistrado) {
            int caractere = GetCharPressed();
            while (caractere > 0) {
                size_t tamanho = strlen(nomeJogador);
                if (caractere >= 32 && caractere <= 125 && tamanho + 1 < sizeof(nomeJogador)) {
                    nomeJogador[tamanho] = (char)caractere;
                    nomeJogador[tamanho + 1] = '\0';
                }
                caractere = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                size_t tamanho = strlen(nomeJogador);
                if (tamanho > 0) nomeJogador[tamanho - 1] = '\0';
            }
            if (IsKeyPressed(KEY_ENTER)) {
                if (nomeRankingValido(nomeJogador) &&
                    registrarResultadoRanking(CAMINHO_RANKING, nomeJogador, &partida)) {
                    rankingRegistrado = true;
                    snprintf(mensagemRanking, sizeof(mensagemRanking),
                             "Resultado registrado para %s.", nomeJogador);
                } else {
                    snprintf(mensagemRanking, sizeof(mensagemRanking),
                             "Nome invalido ou nao foi possivel salvar o ranking.");
                }
            }
        } else if (partidaEncerrada && rankingRegistrado &&
                   (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_M))) {
            break;
        }

        BeginDrawing();

        if (partidaEncerrada && vitoria) {

            // Pagina exclusiva de comemoracao.
            desenharTelaVitoria(
                &estadoVitoria,
                &dadosVitoria,
                nomeJogador,
                rankingRegistrado,
                mensagemRanking
            );

        } else {

            // Interface normal da partida.
            ClearBackground(BLACK);

            if (videoDisponivel) {
                VideoDraw(fundo);
            } else {
                desenharFundoAlternativo();
            }

            DrawRectangle(
                0, 0,
                GetScreenWidth(),
                GetScreenHeight(),
                (Color){2, 8, 22, 105}
            );

            desenharTelaJogo(&dadosTela, &estadoVisual);

            // Mantem a tela antiga para derrota e desistencia.
            if (partidaEncerrada) {
                desenharResultadoPartida(
                    &partida,
                    vitoria,
                    desistencia,
                    nomeJogador,
                    rankingRegistrado,
                    mensagemRanking
                );
            }
        }

        DrawText(
            "F1  VOLTAR AO MENU",
            28,
            GetScreenHeight() - 28,
            11,
            (Color){190, 210, 230, 190}
        );

    EndDrawing();
    }
    if (telaVitoriaInicializada) {
        liberarEstadoVisualVitoria(&estadoVitoria);
    }
    liberarEstadoVisualTelaJogo(&estadoVisual);
}


/*
                    FILTROS
*/

/**
 * @brief 
 * 
 * @param cadastro 
 * @param filtroNivel 
 * @param inicio 
 * @return int : indice do proximo registro que atende ao filtro informado
 */
static int localizarPerguntaFiltrada(
    const Cadastro *cadastro,
    int filtroNivel,
    int inicio
) {
    if (cadastro == NULL || cadastro->perguntas == NULL) return -1;

    int encontradas = 0;
    for (size_t i = 0; i < cadastro->quantidade; i++) {
        if (filtroNivel == 0 || cadastro->perguntas[i].nivel == (NivelDificuldade)filtroNivel) {
            if (encontradas == inicio) return (int)i;
            encontradas++;
        }
    }
    return -1;
}


/**
 * @brief Conta quantas perguntas pertencem ao filtro atual.
 * 
 * @param cadastro 
 * @param filtroNivel 
 * @return int : qntd perguntas do filtro atual
 */
static int contarPerguntasFiltradas(const Cadastro *cadastro, int filtroNivel) {
    if (cadastro == NULL || cadastro->perguntas == NULL) return 0;

    int total = 0;
    for (size_t i = 0; i < cadastro->quantidade; i++) {
        if (filtroNivel == 0 || cadastro->perguntas[i].nivel == (NivelDificuldade)filtroNivel) total++;
    }
    return total;
}

/*

                    UTF-8
*/
/**
 * @brief Remove o ultimo caractere do UTF-8 sem deixar bytes de continuacao soltos
 * 
 * @param texto 
 */
static void removerUltimoCaractereUtf8(char *texto) {
    if (texto == NULL) return;
    size_t tamanho = strlen(texto);
    if (tamanho == 0) return;

    size_t indice = tamanho - 1;
    while (indice > 0 && (((unsigned char)texto[indice] & 0xC0u) == 0x80u)) indice--;
    texto[indice] = '\0';
}



/**
 * @brief  Acrescenta um ponto de codigo Unicode ao campo, respeitando o limite do vetor.
 * 
 * @param destino 
 * @param capacidade 
 * @param codigo 
 */
static void acrescentarCodigoUtf8(char *destino, size_t capacidade, int codigo) {
    if (destino == NULL || capacidade == 0 || codigo < 32 || codigo > 0x10FFFF ||
        (codigo >= 0xD800 && codigo <= 0xDFFF)) return;

    char bytes[4];
    size_t quantidade = 0;
    if (codigo <= 0x7F) {
        bytes[0] = (char)codigo;
        quantidade = 1;
    } else if (codigo <= 0x7FF) {
        bytes[0] = (char)(0xC0 | (codigo >> 6));
        bytes[1] = (char)(0x80 | (codigo & 0x3F));
        quantidade = 2;
    } else if (codigo <= 0xFFFF) {
        bytes[0] = (char)(0xE0 | (codigo >> 12));
        bytes[1] = (char)(0x80 | ((codigo >> 6) & 0x3F));
        bytes[2] = (char)(0x80 | (codigo & 0x3F));
        quantidade = 3;
    } else {
        bytes[0] = (char)(0xF0 | (codigo >> 18));
        bytes[1] = (char)(0x80 | ((codigo >> 12) & 0x3F));
        bytes[2] = (char)(0x80 | ((codigo >> 6) & 0x3F));
        bytes[3] = (char)(0x80 | (codigo & 0x3F));
        quantidade = 4;
    }

    size_t atual = strlen(destino);
    if (atual + quantidade < capacidade) {
        memcpy(destino + atual, bytes, quantidade);
        destino[atual + quantidade] = '\0';
    }
}


/**
 * @brief Retorna o campo de texto que esta ativo no formulario. 
 * 
 * @param formulario 
 * @param capacidade 
 * @return char* 
 */
static char *obterCampoFormulario(FormularioPergunta *formulario, size_t *capacidade) {
    if (formulario == NULL || capacidade == NULL) return NULL;
    if (formulario->campoAtivo == 0) {
        *capacidade = sizeof(formulario->pergunta.enunciado);
        return formulario->pergunta.enunciado;
    }
    if (formulario->campoAtivo >= 1 && formulario->campoAtivo <= QUANT_ALTERNATIVAS) {
        *capacidade = sizeof(formulario->pergunta.alternativas[0]);
        return formulario->pergunta.alternativas[formulario->campoAtivo - 1];
    }
    return NULL;
}

/**
 * @brief  Cria um ID positivo ainda nao utilizado no cadastro. 
 * 
 * @param cadastro 
 * @return int 
 */
static int gerarNovoIdPergunta(const Cadastro *cadastro) {
    if (cadastro == NULL) return 0;

    int candidato = 1;
    for (;;) {
        bool encontrado = false;
        for (size_t i = 0; i < cadastro->quantidade; i++) {
            if (cadastro->perguntas[i].id == candidato) {
                encontrado = true;
                break;
            }
        }
        if (!encontrado) return candidato;
        if (candidato == INT_MAX) return 0;
        candidato++;
    }
}

/**
 * @brief Confirma se uma string possui ao menos um caractere que nao seja espaco.
 * 
 * @param texto 
 * @return true 
 * @return false 
 */
static bool textoPossuiConteudo(const char *texto) {
    if (texto == NULL) return false;
    for (size_t i = 0; texto[i] != '\0'; i++) {
        if (texto[i] != ' ' && texto[i] != '\t' && texto[i] != '\n' && texto[i] != '\r') return true;
    }
    return false;
}

/**
 * @brief Valida os campos do formulario antes de chamar o modulo de cadastro.
 * 
 * @param pergunta 
 * @return const char* : mensagem referente ao erro/oq falta para chamar cadastro.
 */
static const char *validarFormularioPergunta(const Pergunta *pergunta) {
    if (pergunta == NULL) return "Dados da pergunta invalidos.";
    if (!textoPossuiConteudo(pergunta->enunciado)) return "Informe o enunciado da pergunta.";

    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        if (!textoPossuiConteudo(pergunta->alternativas[i])) {
            return TextFormat("Preencha a alternativa %c.", 'A' + i);
        }
    }
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        for (int j = i + 1; j < QUANT_ALTERNATIVAS; j++) {
            if (strcmp(pergunta->alternativas[i], pergunta->alternativas[j]) == 0) {
                return "As quatro alternativas devem ser diferentes.";
            }
        }
    }
    if (pergunta->respostaCorreta < 'A' || pergunta->respostaCorreta > 'D') {
        return "Selecione a resposta correta.";
    }
    if (pergunta->nivel < MUITO_FACIL || pergunta->nivel > MUITO_DIFICIL) {
        return "Selecione o nivel de dificuldade.";
    }
    return NULL;
}

/* Inicializa o formulario para uma nova pergunta. */
static void abrirFormularioNovaPergunta(FormularioPergunta *formulario, const Cadastro *cadastro) {
    if (formulario == NULL) return;
    *formulario = (FormularioPergunta){0};
    formulario->ativo = true;
    formulario->edicao = false;
    formulario->campoAtivo = 0;
    formulario->pergunta.id = gerarNovoIdPergunta(cadastro);
    formulario->pergunta.respostaCorreta = 'A';
    formulario->pergunta.nivel = FACIL;
}

/* Inicializa o formulario com os dados da pergunta selecionada. */
static bool abrirFormularioEdicao(FormularioPergunta *formulario, Cadastro *cadastro, int id) {
    if (formulario == NULL || cadastro == NULL || id <= 0) return false;
    Pergunta *existente = buscarPerguntaPorId(cadastro, id);
    if (existente == NULL) return false;

    *formulario = (FormularioPergunta){0};
    formulario->ativo = true;
    formulario->edicao = true;
    formulario->idOriginal = id;
    formulario->campoAtivo = 0;
    formulario->pergunta = *existente;
    return true;
}

/* Salva de forma transacional: em falha de arquivo, tenta restaurar o cadastro anterior. */
static bool salvarFormularioPergunta(
    Cadastro *cadastro,
    FormularioPergunta *formulario,
    char *mensagem,
    size_t capacidadeMensagem
) {
    if (cadastro == NULL || formulario == NULL) return false;

    const char *erro = validarFormularioPergunta(&formulario->pergunta);
    if (erro != NULL) {
        snprintf(mensagem, capacidadeMensagem, "%s", erro);
        return false;
    }

    Pergunta original = {0};
    int id = formulario->pergunta.id;
    if (formulario->edicao) {
        Pergunta *atual = buscarPerguntaPorId(cadastro, formulario->idOriginal);
        if (atual == NULL) {
            snprintf(mensagem, capacidadeMensagem, "A pergunta selecionada nao existe mais.");
            return false;
        }
        original = *atual;
        if (!atualizarPergunta(cadastro, formulario->idOriginal, &formulario->pergunta)) {
            snprintf(mensagem, capacidadeMensagem, "Nao foi possivel atualizar a pergunta.");
            return false;
        }
        id = formulario->idOriginal;
    } else {
        id = gerarNovoIdPergunta(cadastro);
        if (id <= 0) {
            snprintf(mensagem, capacidadeMensagem, "Nao foi possivel gerar um ID livre.");
            return false;
        }
        formulario->pergunta.id = id;
        if (buscarPerguntaPorId(cadastro, id) != NULL ||
            !adicionarPergunta(cadastro, &formulario->pergunta)) {
            snprintf(mensagem, capacidadeMensagem, "Nao foi possivel inserir a pergunta.");
            return false;
        }
    }

    if (!salvarBinario(cadastro, CAMINHO_BINARIO)) {
        if (formulario->edicao) {
            (void)atualizarPergunta(cadastro, id, &original);
        } else {
            (void)removerPergunta(cadastro, id);
        }
        snprintf(mensagem, capacidadeMensagem,
                 "Falha ao salvar o binario. A alteracao foi desfeita em memoria.");
        return false;
    }

    snprintf(mensagem, capacidadeMensagem,
             formulario->edicao ? "Pergunta %d atualizada e salva." : "Pergunta %d inserida e salva.", id);
    return true;
}

/* Exclui a pergunta selecionada e restaura-a se a persistencia falhar. */
static bool excluirPerguntaPersistente(
    Cadastro *cadastro,
    int id,
    char *mensagem,
    size_t capacidadeMensagem
) {
    if (cadastro == NULL || id <= 0) return false;
    Pergunta *encontrada = buscarPerguntaPorId(cadastro, id);
    if (encontrada == NULL) {
        snprintf(mensagem, capacidadeMensagem, "A pergunta selecionada nao foi encontrada.");
        return false;
    }

    Pergunta removida = *encontrada;
    if (!removerPergunta(cadastro, id)) {
        snprintf(mensagem, capacidadeMensagem, "Nao foi possivel excluir a pergunta.");
        return false;
    }
    if (!salvarBinario(cadastro, CAMINHO_BINARIO)) {
        (void)adicionarPergunta(cadastro, &removida);
        snprintf(mensagem, capacidadeMensagem,
                 "Falha ao salvar o binario. A pergunta foi restaurada em memoria.");
        return false;
    }
    snprintf(mensagem, capacidadeMensagem, "Pergunta %d excluida e banco salvo.", id);
    return true;
}

/* Desenha campo de texto com recorte e destaque de foco. */
static void desenharCampoFormulario(const char *rotulo, const char *valor,
                                    Rectangle area, bool ativo) {
    DrawText(rotulo, (int)area.x, (int)area.y - 19, 11, ativo ? COR_CIANO : COR_TEXTO_SEC);
    DrawRectangleRounded(area, 0.10f, 7,
                         ativo ? (Color){7, 31, 54, 255} : (Color){4, 15, 31, 255});
    DrawRectangleRoundedLinesEx(area, 0.10f, 7, ativo ? 1.8f : 0.9f,
                                 ativo ? COR_CIANO : COR_BORDA);

    const char *texto = (valor != NULL && valor[0] != '\0') ? valor : "Digite aqui...";
    Color cor = (valor != NULL && valor[0] != '\0') ? COR_TEXTO : (Color){113, 139, 165, 210};
    int larguraTexto = MeasureText(texto, 14);
    float xTexto = area.x + 10.0f;
    if (ativo && larguraTexto > area.width - 20.0f) {
        xTexto = area.x + area.width - (float)larguraTexto - 10.0f;
    }
    BeginScissorMode((int)area.x + 4, (int)area.y + 2,
                     (int)area.width - 8, (int)area.height - 4);
    DrawText(texto, (int)xTexto, (int)area.y + (int)(area.height - 14.0f) / 2, 14, cor);
    if (ativo && valor != NULL && valor[0] != '\0') {
        int cursorX = (int)xTexto + MeasureText(valor, 14) + 2;
        if (cursorX < (int)(area.x + area.width - 5.0f)) {
            DrawLine(cursorX, (int)area.y + 10, cursorX, (int)(area.y + area.height - 10), COR_CIANO);
        }
    }
    EndScissorMode();
}

/* Processa teclado para os campos ativos do formulario. */
static void processarEntradaFormulario(FormularioPergunta *formulario) {
    if (formulario == NULL || !formulario->ativo) return;

    if (IsKeyPressed(KEY_TAB)) {
        formulario->campoAtivo = (formulario->campoAtivo + 1) % (QUANT_ALTERNATIVAS + 1);
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        size_t capacidade = 0;
        char *campo = obterCampoFormulario(formulario, &capacidade);
        (void)capacidade;
        if (campo != NULL) removerUltimoCaractereUtf8(campo);
    }

    int caractere = GetCharPressed();
    while (caractere > 0) {
        size_t capacidade = 0;
        char *campo = obterCampoFormulario(formulario, &capacidade);
        if (campo != NULL) acrescentarCodigoUtf8(campo, capacidade, caractere);
        caractere = GetCharPressed();
    }
}

/* Renderiza e controla o modal de inclusao/edicao. */
static void desenharFormularioPergunta(
    FormularioPergunta *formulario,
    Cadastro *cadastro,
    int *idSelecionado,
    char *mensagemMenu,
    size_t capacidadeMensagem,
    double *instanteMensagem
) {
    if (formulario == NULL || !formulario->ativo) return;

    float larguraTela = (float)GetScreenWidth();
    float alturaTela = (float)GetScreenHeight();
    float larguraModal = larguraTela < 940.0f ? larguraTela - 36.0f : 900.0f;
    float alturaModal = alturaTela < 700.0f ? alturaTela - 28.0f : 650.0f;
    Rectangle modal = {(larguraTela - larguraModal) / 2.0f,
                       (alturaTela - alturaModal) / 2.0f,
                       larguraModal, alturaModal};

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 205});
    desenharPainel(modal, (Color){5, 17, 37, 255}, COR_AZUL);
    DrawText(formulario->edicao ? "EDITAR PERGUNTA" : "NOVA PERGUNTA",
             (int)modal.x + 28, (int)modal.y + 22, 25, COR_TEXTO);
    DrawText(TextFormat("ID: %03d  |  %s", formulario->pergunta.id,
                        formulario->edicao ? "EDICAO DO REGISTRO" : "NOVO REGISTRO"),
             (int)modal.x + 30, (int)modal.y + 57, 11, COR_DOURADO);

    float esquerda = modal.x + 30.0f;
    float larguraCampo = modal.width - 60.0f;
    Rectangle campoEnunciado = {esquerda, modal.y + 111.0f, larguraCampo, 44.0f};
    desenharCampoFormulario("ENUNCIADO DA PERGUNTA", formulario->pergunta.enunciado,
                            campoEnunciado, formulario->campoAtivo == 0);

    float larguraAlternativa = (larguraCampo - 22.0f) / 2.0f;
    float alturaAlternativa = 42.0f;
    float xColuna2 = esquerda + larguraAlternativa + 22.0f;
    Rectangle campos[QUANT_ALTERNATIVAS] = {
        {esquerda, modal.y + 207.0f, larguraAlternativa, alturaAlternativa},
        {xColuna2, modal.y + 207.0f, larguraAlternativa, alturaAlternativa},
        {esquerda, modal.y + 280.0f, larguraAlternativa, alturaAlternativa},
        {xColuna2, modal.y + 280.0f, larguraAlternativa, alturaAlternativa}
    };
    const char *rotulos[QUANT_ALTERNATIVAS] = {
        "ALTERNATIVA A", "ALTERNATIVA B", "ALTERNATIVA C", "ALTERNATIVA D"
    };
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        desenharCampoFormulario(rotulos[i], formulario->pergunta.alternativas[i],
                                campos[i], formulario->campoAtivo == i + 1);
    }

    DrawText("RESPOSTA CORRETA", (int)esquerda, (int)modal.y + 352, 11, COR_TEXTO_SEC);
    float larguraResposta = 58.0f;
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        Rectangle botao = {esquerda + i * (larguraResposta + 10.0f),
                           modal.y + 372.0f, larguraResposta, 34.0f};
        bool selecionado = formulario->pergunta.respostaCorreta == (char)('A' + i);
        if (desenharBotaoPremium(botao, (const char *[]) {"A", "B", "C", "D"}[i],
                                NULL, NULL, selecionado)) {
            formulario->pergunta.respostaCorreta = (char)('A' + i);
        }
    }

    DrawText("NIVEL DE DIFICULDADE", (int)esquerda, (int)modal.y + 426, 11, COR_TEXTO_SEC);
    const char *nomesNiveis[5] = {"MUITO FACIL", "FACIL", "MEDIO", "DIFICIL", "MUITO DIFICIL"};
    NivelDificuldade niveis[5] = {MUITO_FACIL, FACIL, MEDIO, DIFICIL, MUITO_DIFICIL};
    float gapNivel = 8.0f;
    float larguraNivel = (larguraCampo - 4.0f * gapNivel) / 5.0f;
    for (int i = 0; i < 5; i++) {
        Rectangle botao = {esquerda + i * (larguraNivel + gapNivel),
                           modal.y + 446.0f, larguraNivel, 34.0f};
        bool selecionado = formulario->pergunta.nivel == niveis[i];
        if (desenharBotaoPremium(botao, nomesNiveis[i], NULL, NULL, selecionado)) {
            formulario->pergunta.nivel = niveis[i];
        }
    }

    DrawText("TAB: PROXIMO CAMPO  |  BACKSPACE: APAGAR  |  USE O MOUSE PARA SELECIONAR",
             (int)esquerda, (int)modal.y + 500, 9, COR_TEXTO_SEC);

    Rectangle botaoSalvar = {modal.x + modal.width - 292.0f, modal.y + modal.height - 62.0f,
                              150.0f, 42.0f};
    Rectangle botaoCancelar = {modal.x + modal.width - 128.0f, modal.y + modal.height - 62.0f,
                                98.0f, 42.0f};
    if (desenharBotaoPremium(botaoSalvar, "SALVAR", NULL, NULL, true)) {
        char mensagem[TAM_MENSAGEM_FORM] = "";
        if (salvarFormularioPergunta(cadastro, formulario, mensagem, sizeof(mensagem))) {
            if (idSelecionado != NULL) *idSelecionado = formulario->pergunta.id;
            snprintf(mensagemMenu, capacidadeMensagem, "%s", mensagem);
            formulario->ativo = false;
            if (instanteMensagem != NULL) *instanteMensagem = GetTime() + 4.0;
        } else {
            snprintf(mensagemMenu, capacidadeMensagem, "%s", mensagem);
            if (instanteMensagem != NULL) *instanteMensagem = GetTime() + 4.0;
        }
    }
    if (desenharBotaoPremium(botaoCancelar, "CANCELAR", NULL, NULL, false) ||
        IsKeyPressed(KEY_ESCAPE)) {
        formulario->ativo = false;
    }
}

/* Prototipo: a funcao de truncamento e implementada logo apos os modais. */
static void desenharTextoTruncado(
    const char *texto,
    int x,
    int y,
    int tamanhoFonte,
    int larguraMaxima,
    Color cor
);

/* Modal de confirmacao da exclusao de uma pergunta. */
static bool desenharConfirmacaoExclusao(
    Cadastro *cadastro,
    int id,
    char *mensagemMenu,
    size_t capacidadeMensagem,
    double *instanteMensagem
) {
    float largura = (float)GetScreenWidth();
    float altura = (float)GetScreenHeight();
    Rectangle modal = {largura / 2.0f - 270.0f, altura / 2.0f - 125.0f, 540.0f, 250.0f};
    Pergunta *pergunta = buscarPerguntaPorId(cadastro, id);

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 205});
    desenharPainel(modal, (Color){8, 18, 37, 255}, COR_VERMELHO);
    DrawText("CONFIRMAR EXCLUSAO", (int)modal.x + 30, (int)modal.y + 28, 22, COR_VERMELHO);
    DrawText(TextFormat("A pergunta ID %03d sera removida permanentemente.", id),
             (int)modal.x + 30, (int)modal.y + 78, 13, COR_TEXTO);
    if (pergunta != NULL) {
        desenharTextoTruncado(pergunta->enunciado, (int)modal.x + 30,
                              (int)modal.y + 110, 12, (int)modal.width - 60, COR_TEXTO_SEC);
    }
    DrawText("Essa operacao sera gravada no arquivo binario.",
             (int)modal.x + 30, (int)modal.y + 150, 11, COR_TEXTO_SEC);

    Rectangle confirmar = {modal.x + 245.0f, modal.y + 185.0f, 130.0f, 42.0f};
    Rectangle cancelar = {modal.x + 390.0f, modal.y + 185.0f, 112.0f, 42.0f};
    if (desenharBotaoPremium(confirmar, "EXCLUIR", NULL, NULL, true)) {
        char mensagem[TAM_MENSAGEM_FORM] = "";
        (void)excluirPerguntaPersistente(cadastro, id, mensagem, sizeof(mensagem));
        snprintf(mensagemMenu, capacidadeMensagem, "%s", mensagem);
        if (instanteMensagem != NULL) *instanteMensagem = GetTime() + 4.0;
        return true;
    }
    if (desenharBotaoPremium(cancelar, "CANCELAR", NULL, NULL, false) ||
        IsKeyPressed(KEY_ESCAPE)) return true;
    return false;
}

/* Desenha uma linha de texto sem ultrapassar a largura disponivel. */
static void desenharTextoTruncado(
    const char *texto,
    int x,
    int y,
    int tamanhoFonte,
    int larguraMaxima,
    Color cor
) {
    if (texto == NULL || larguraMaxima <= 0) return;

    char trecho[TAM_ENUNCIADO + 1];
    char candidato[TAM_ENUNCIADO + 1];
    snprintf(trecho, sizeof(trecho), "%s", texto);
    if (MeasureText(trecho, tamanhoFonte) > larguraMaxima) {
        while (strlen(trecho) > 0) {
            removerUltimoCaractereUtf8(trecho);
            snprintf(candidato, sizeof(candidato), "%s...", trecho);
            if (MeasureText(candidato, tamanhoFonte) <= larguraMaxima) {
                snprintf(trecho, sizeof(trecho), "%s", candidato);
                break;
            }
        }
        if (trecho[0] == '\0') snprintf(trecho, sizeof(trecho), "...");
    }
    DrawText(trecho, x, y, tamanhoFonte, cor);
}

/* Tela de navegacao do banco: lista continua, filtros e selecao de registros. */
static void desenharTelaBanco(
    const Cadastro *cadastro,
    int filtroNivel,
    float *deslocamentoScroll,
    const char *mensagem,
    int *idSelecionado,
    bool permitirEntrada
) {
    const float alturaLinha = 43.0f;
    const float largura = (float)GetScreenWidth();
    const float altura = (float)GetScreenHeight();
    desenharCabecalhoTela("BANCO DE PERGUNTAS",
        "Consulte, filtre, cadastre, edite e remova questões do acervo.");

    Rectangle painel = {42.0f, 198.0f, largura - 84.0f, altura - 288.0f};
    if (painel.height < 300.0f) painel.height = 300.0f;
    desenharPainel(painel, COR_PAINEL, COR_BORDA);

    const char *filtros[6] = {
        "TODAS", "MUITO FACIL", "FACIL", "MEDIO", "DIFICIL", "MUITO DIFICIL"
    };
    float larguraFiltro = (largura - 100.0f) / 6.0f;
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < 6; i++) {
        Rectangle area = {50.0f + i * larguraFiltro, 208.0f, larguraFiltro - 7.0f, 31.0f};
        bool ativo = filtroNivel == i;
        bool hover = CheckCollisionPointRec(mouse, area);
        DrawRectangleRounded(area, 0.18f, 8,
            ativo ? (Color){18, 87, 125, 245} :
            (hover ? COR_PAINEL_ALT : (Color){5, 17, 35, 220}));
        DrawRectangleRoundedLinesEx(area, 0.18f, 8, ativo ? 1.6f : 0.8f,
                                     ativo ? COR_CIANO : COR_BORDA);
        int fonte = largura < 1050.0f ? 9 : 11;
        DrawText(filtros[i],
            (int)(area.x + (area.width - MeasureText(filtros[i], fonte)) / 2.0f),
            (int)area.y + 10, fonte, ativo ? WHITE : COR_TEXTO_SEC);
    }

    DrawText("ID", 64, 257, 11, COR_CIANO);
    DrawText("NIVEL", 125, 257, 11, COR_CIANO);
    DrawText("ENUNCIADO", 270, 257, 11, COR_CIANO);
    DrawText("STATUS", (int)largura - 160, 257, 11, COR_CIANO);
    DrawLine(60, 278, (int)largura - 60, 278, (Color){57, 112, 158, 130});

    Rectangle areaLista = {60.0f, 285.0f, largura - 120.0f,
        painel.y + painel.height - 54.0f - 285.0f};
    if (areaLista.height < 100.0f) areaLista.height = 100.0f;
    int totalFiltrado = contarPerguntasFiltradas(cadastro, filtroNivel);
    float alturaConteudo = (float)totalFiltrado * alturaLinha;
    float scrollMaximo = alturaConteudo > areaLista.height
        ? alturaConteudo - areaLista.height : 0.0f;
    if (deslocamentoScroll == NULL) return;

    if (permitirEntrada) {
        float movimentoRoda = GetMouseWheelMove();
        if (CheckCollisionPointRec(mouse, areaLista)) {
            *deslocamentoScroll -= movimentoRoda * alturaLinha * 2.4f;
        }
        if (IsKeyPressed(KEY_UP)) *deslocamentoScroll -= alturaLinha;
        if (IsKeyPressed(KEY_DOWN)) *deslocamentoScroll += alturaLinha;
        if (IsKeyPressed(KEY_PAGE_UP)) *deslocamentoScroll -= areaLista.height * 0.85f;
        if (IsKeyPressed(KEY_PAGE_DOWN)) *deslocamentoScroll += areaLista.height * 0.85f;
    }
    if (*deslocamentoScroll < 0.0f) *deslocamentoScroll = 0.0f;
    if (*deslocamentoScroll > scrollMaximo) *deslocamentoScroll = scrollMaximo;

    if (totalFiltrado == 0) {
        DrawText("NENHUMA PERGUNTA ENCONTRADA PARA ESTE FILTRO.",
                 (int)areaLista.x + 18, (int)areaLista.y + 24, 14, COR_TEXTO_SEC);
    } else {
        int primeiraLinha = (int)(*deslocamentoScroll / alturaLinha);
        float deslocamentoLinha = *deslocamentoScroll - primeiraLinha * alturaLinha;
        int linhasVisiveis = (int)(areaLista.height / alturaLinha) + 2;
        int ultimaLinha = primeiraLinha + linhasVisiveis;
        if (ultimaLinha > totalFiltrado) ultimaLinha = totalFiltrado;

        for (int linha = primeiraLinha; linha < ultimaLinha; linha++) {
            int indice = localizarPerguntaFiltrada(cadastro, filtroNivel, linha);
            if (indice < 0) break;
            const Pergunta *pergunta = &cadastro->perguntas[indice];
            float y = areaLista.y + (linha - primeiraLinha) * alturaLinha - deslocamentoLinha;
            if (y + alturaLinha < areaLista.y || y > areaLista.y + areaLista.height) continue;

            Rectangle linhaArea = {areaLista.x, y + 2.0f, areaLista.width - 16.0f, 36.0f};
            bool selecionada = idSelecionado != NULL && *idSelecionado == pergunta->id;
            Color corLinha = selecionada ? (Color){16, 65, 91, 235} :
                (linha % 2 == 0 ? (Color){12, 30, 55, 170} : (Color){5, 20, 42, 150});
            DrawRectangleRounded(linhaArea, 0.12f, 6, corLinha);
            if (selecionada) DrawRectangleRoundedLinesEx(linhaArea, 0.12f, 6, 1.3f, COR_CIANO);

            if (permitirEntrada && CheckCollisionPointRec(mouse, linhaArea) &&
                IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && idSelecionado != NULL) {
                *idSelecionado = pergunta->id;
            }

            DrawText(TextFormat("%03d", pergunta->id), 66, (int)y + 12, 13, COR_DOURADO);
            desenharTextoTruncado(obterNomeNivelMenu(pergunta->nivel), 125,
                                  (int)y + 13, 11, 132, COR_CIANO);
            desenharTextoTruncado(pergunta->enunciado, 270, (int)y + 12,
                                  12, (int)(largura - 475.0f), COR_TEXTO);
            DrawText(selecionada ? "SELECIONADA" : "CLIQUE PARA SELECIONAR",
                     (int)largura - 230, (int)y + 13, 8,
                     selecionada ? COR_VERDE : (Color){115, 145, 173, 190});
        }
    }

    Rectangle trilho = {areaLista.x + areaLista.width - 8.0f,
                        areaLista.y, 5.0f, areaLista.height};
    DrawRectangleRounded(trilho, 0.5f, 6, (Color){27, 48, 71, 220});
    if (scrollMaximo > 0.0f) {
        float alturaIndicador = areaLista.height * (areaLista.height / alturaConteudo);
        if (alturaIndicador < 28.0f) alturaIndicador = 28.0f;
        if (alturaIndicador > areaLista.height) alturaIndicador = areaLista.height;
        float posicaoIndicador = (areaLista.height - alturaIndicador) *
                                  (*deslocamentoScroll / scrollMaximo);
        DrawRectangleRounded((Rectangle){trilho.x, trilho.y + posicaoIndicador,
            trilho.width, alturaIndicador}, 0.5f, 6, COR_AZUL);
    } else {
        DrawRectangleRounded(trilho, 0.5f, 6, COR_AZUL);
    }

    int primeiraExibida = totalFiltrado == 0 ? 0 : (int)(*deslocamentoScroll / alturaLinha) + 1;
    int ultimaExibida = totalFiltrado == 0 ? 0 :
        (int)((*deslocamentoScroll + areaLista.height) / alturaLinha);
    if (ultimaExibida > totalFiltrado) ultimaExibida = totalFiltrado;
    DrawText(TextFormat("%d PERGUNTAS  |  EXIBINDO %d-%d",
        totalFiltrado, primeiraExibida, ultimaExibida), 62,
        (int)(painel.y + painel.height - 34.0f), 11, COR_TEXTO_SEC);
    if (mensagem != NULL && mensagem[0] != '\0') {
        DrawText(mensagem, 62, (int)(painel.y + painel.height - 17.0f), 10, COR_DOURADO);
    }
    DrawText("ROLE A RODA DO MOUSE OU USE AS SETAS / PAGE UP / PAGE DOWN",
        (int)(largura - 515.0f), (int)(painel.y + painel.height - 34.0f), 9, COR_TEXTO_SEC);
    (void)altura;
}

/* Tela de ranking baseada na persistencia do modulo ranking.c. */
static void desenharPainelRankingMenu(void) {
    float largura = (float)GetScreenWidth();
    desenharCabecalhoTela("HALL DA FAMA", "Os melhores resultados registrados nas jornadas do Cosmos.");
    Rectangle painel = {42, 198, largura - 84, 420};
    desenharPainel(painel, COR_PAINEL, COR_BORDA);

    ResultadoRanking resultados[RANKING_LIMITE] = {0};
    int quantidade = consultarRanking(CAMINHO_RANKING, resultados);
    DrawText("POS", 68, 225, 11, COR_CIANO);
    DrawText("ASTRONAUTA", 145, 225, 11, COR_CIANO);
    DrawText("PREMIO", (int)(largura * 0.57f), 225, 11, COR_CIANO);
    DrawText("ACERTOS", (int)(largura * 0.76f), 225, 11, COR_CIANO);
    DrawLine(62, 247, (int)largura - 62, 247, (Color){57, 112, 158, 130});

    if (quantidade == 0) {
        DrawText("AINDA NAO HA RESULTADOS REGISTRADOS.", 90, 300, 18, COR_TEXTO);
        DrawText("Conclua uma partida para aparecer no ranking.", 90, 333, 13, COR_TEXTO_SEC);
    } else if (quantidade < 0) {
        DrawText("NAO FOI POSSIVEL LER O ARQUIVO DE RANKING.", 90, 300, 16, COR_VERMELHO);
    } else {
        for (int i = 0; i < quantidade; i++) {
            float y = 267.0f + i * 32.0f;
            Color cor = i == 0 ? COR_DOURADO : COR_TEXTO;
            DrawText(TextFormat("%02d", i + 1), 70, (int)y, 14, cor);
            DrawText(resultados[i].nome, 145, (int)y, 14, COR_TEXTO);
            DrawText(TextFormat("R$ %d", resultados[i].premio),
                     (int)(largura * 0.57f), (int)y, 13, COR_DOURADO);
            DrawText(TextFormat("%d / %d", resultados[i].acertos, TOTAL_PERGUNTAS_PARTIDA),
                     (int)(largura * 0.76f), (int)y, 13, COR_TEXTO_SEC);
        }
    }
}

/* Tela de ajuda com orientacoes de uso e descricao das quatro ajudas. */
static void desenharTelaAjudaMenu(void) {
    float largura = (float)GetScreenWidth();
    desenharCabecalhoTela("CENTRAL DE AJUDA", "Conheca os recursos disponiveis durante uma partida.");
    const char *titulos[4] = {"SUPERNOVA", "MISSAO NASA", "TELESCÓPIO", "ESPECIALISTA"};
    const char *descricoes[4] = {
        "Elimina duas alternativas incorretas, preservando a resposta certa.",
        "Apresenta uma pista especifica relacionada ao enunciado atual.",
        "Exibe uma informacao complementar sobre o assunto da pergunta.",
        "Sugere uma alternativa com chance de acerto conforme a dificuldade."
    };
    for (int i = 0; i < 4; i++) {
        int coluna = i % 2;
        int linha = i / 2;
        Rectangle area = {48.0f + coluna * ((largura - 110.0f) / 2.0f),
            215.0f + linha * 145.0f, (largura - 120.0f) / 2.0f, 120.0f};
        desenharPainel(area, COR_PAINEL, COR_BORDA);
        DrawText(TextFormat("0%d", i + 1), (int)area.x + 20, (int)area.y + 18, 22, COR_DOURADO);
        DrawText(titulos[i], (int)area.x + 67, (int)area.y + 20, 16, COR_CIANO);
        DrawText(descricoes[i], (int)area.x + 67, (int)area.y + 52, 12, COR_TEXTO);
        DrawText("USO UNICO POR PARTIDA", (int)area.x + 67,
                 (int)area.y + 88, 10, COR_TEXTO_SEC);
    }
    DrawText("DICA: as ajudas nao utilizadas permanecem disponiveis ate o fim da partida.",
             52, 540, 13, COR_TEXTO_SEC);
}

/* Tela de configuracoes: volume da ambiencia e dos efeitos sonoros. */
static void desenharTelaConfiguracoes(
    bool audioDisponivel,
    bool audioAtivo,
    float volumeAmbiente,
    float volumeEfeitos
) {
    float largura = (float)GetScreenWidth();
    desenharCabecalhoTela("CONFIGURAÇÕES", "Ajuste a experiência sonora da sua jornada.");
    Rectangle painel = {70, 220, largura - 140, 315};
    desenharPainel(painel, COR_PAINEL, COR_BORDA);
    DrawText("AUDIO DO JOGO", 105, 250, 17, COR_TEXTO);
    DrawText(audioDisponivel ? (audioAtivo ? "ATIVADO" : "SILENCIADO") : "INDISPONIVEL",
        (int)largura - 230, 252, 13,
        audioDisponivel && audioAtivo ? COR_VERDE : COR_TEXTO_SEC);
    DrawText("AMBIENCIA", 105, 325, 13, COR_CIANO);
    DrawText(TextFormat("%d%%", (int)(volumeAmbiente * 100.0f)),
        (int)largura - 210, 325, 13, COR_TEXTO);
    DrawRectangleRounded((Rectangle){105, 352, largura - 330, 8}, 0.5f, 8,
                         (Color){24, 49, 76, 255});
    DrawRectangleRounded((Rectangle){105, 352, (largura - 330) * volumeAmbiente, 8},
                         0.5f, 8, COR_AZUL);
    DrawText("EFEITOS SONOROS", 105, 405, 13, COR_CIANO);
    DrawText(TextFormat("%d%%", (int)(volumeEfeitos * 100.0f)),
        (int)largura - 210, 405, 13, COR_TEXTO);
    DrawRectangleRounded((Rectangle){105, 432, largura - 330, 8}, 0.5f, 8,
                         (Color){24, 49, 76, 255});
    DrawRectangleRounded((Rectangle){105, 432, (largura - 330) * volumeEfeitos, 8},
                         0.5f, 8, COR_DOURADO);
    DrawText("Use os botoes + e - para ajustar os volumes.", 105, 485, 12, COR_TEXTO_SEC);
}



// Executa o menu principal, as telas auxiliares e o gerenciamento dos recursos.
void executarMenu(Cadastro *cadastro) {
    if (cadastro == NULL) {
        TraceLog(LOG_ERROR, "Cadastro invalido ao abrir o menu.");
        return;
    }

   SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_FULLSCREEN_MODE );
    
    InitWindow(1280, 720, "COSMOS");

    Image icone = LoadImage("assets/images/icone.png");

    SetWindowIcon(icone);

    UnloadImage(icone);
    inicializarFontes();
    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Nao foi possivel inicializar a janela principal.");
        return;
    }
    SetTargetFPS(60);

    AudioJogo audio = {0};
    bool audioDisponivel = inicializarAudioJogo(&audio);
    bool audioAtivo = audioDisponivel;
    float volumeAmbiente = 0.22f;
    float volumeEfeitos = 0.70f;
    if (audioDisponivel) {
        definirVolumeAmbienteAudio(&audio, volumeAmbiente);
        definirVolumeEfeitosAudio(&audio, volumeEfeitos);
        iniciarAmbienteAudio(&audio);
    } else {
        TraceLog(LOG_WARNING, "Sistema de audio indisponivel.");
    }



    Font fonteCosmos = GetFontDefault();
    bool fonteCarregada = false;
    if (FileExists(CAMINHO_FONTE)) {
        Font temporaria = LoadFontEx(CAMINHO_FONTE, 128, NULL, 0);
        if (IsFontValid(temporaria)) {
            fonteCosmos = temporaria;
            fonteCarregada = true;
            SetTextureFilter(fonteCosmos.texture, TEXTURE_FILTER_BILINEAR);
        } else {
            TraceLog(LOG_WARNING, "Nao foi possivel carregar a fonte personalizada.");
        }
    } else {
        TraceLog(LOG_WARNING, "Fonte personalizada nao encontrada; usando fonte padrao.");
    }

    VideoBackground fundo = {0};
    bool videoDisponivel = VideoInit(&fundo, CAMINHO_VIDEO);
    if (!videoDisponivel) TraceLog(LOG_WARNING, "Video indisponivel; usando fundo espacial alternativo.");

    TelaMenu telaAtual = MENU_INICIO;
    bool executando = true;
    bool confirmarSaida = false;
    bool confirmarExclusao = false;
    int filtroNivel = 0;
    int idSelecionado = 0;
    int idParaExcluir = 0;
    float scrollBanco = 0.0f;
    FormularioPergunta formulario = {0};
    char mensagemMenu[TAM_MENSAGEM_MENU] = "Bem-vindo ao centro de comando Cosmos.";
    double instanteMensagem = 0.0;

    while (executando && !WindowShouldClose()) {
        float largura = (float)GetScreenWidth();
        float altura = (float)GetScreenHeight();
        Vector2 mouse = GetMousePosition();

        if (videoDisponivel) VideoUpdate(&fundo);
        if (audioDisponivel) atualizarAudioJogo(&audio);
        if (instanteMensagem > 0.0 && GetTime() >= instanteMensagem) {
            mensagemMenu[0] = '\0';
            instanteMensagem = 0.0;
        }
        

        if (telaAtual != MENU_INICIO && IsKeyPressed(KEY_ESCAPE) &&
            !formulario.ativo && !confirmarExclusao) {
            telaAtual = MENU_INICIO;
            confirmarSaida = false;
        }

        BeginDrawing();
            ClearBackground(BLACK);
            desenharFundoMenu(&fundo, videoDisponivel);

            if (telaAtual == MENU_INICIO) {
                DrawTextEx(fonteCosmos, "COSMOS", (Vector2){58.0f, 70.0f},
                           largura < 1100 ? 68.0f : 92.0f, 2.0f, COR_TEXTO);
                DrawLine(66, 200, 390, 200, (Color){46, 190, 240, 170});
                DrawTextEx(obterFontePrincipal(),"EXPLORE O UNIVERSO.\nTESTE SEU CONHECIMENTO.",(Vector2){66,229},21,2, COR_TEXTO);
                DrawTextEx(obterFontePrincipal(), "Quinze desafios. Cinco niveis. Uma jornada até um milhao.", (Vector2){66,294},12,2, COR_TEXTO_SEC);
               
                Rectangle painelInfo = {66, altura - 160.0f, 390, 92};
                desenharPainel(painelInfo, (Color){5, 19, 40, 190}, COR_BORDA);
                DrawText("15", 86, (int)painelInfo.y + 17, 28, COR_DOURADO);
                DrawTextEx(obterFontePrincipal(),"PERGUNTAS", (Vector2){ 88, (int)painelInfo.y + 51}, 9,2, COR_TEXTO_SEC);
                DrawText("80+", 210, (int)painelInfo.y + 17, 28, COR_CIANO);
                DrawTextEx(obterFontePrincipal(),"QUESTÕES", (Vector2){212, (int)painelInfo.y + 51},9,2, COR_TEXTO_SEC);
                DrawText("04", 337, (int)painelInfo.y + 17, 28, COR_VERDE);
                DrawTextEx(obterFontePrincipal(),"AJUDAS", (Vector2){339, (int)painelInfo.y + 51},9,2, COR_TEXTO_SEC);

                float x = largura - 440.0f;
                float y = altura * 0.19f;
                float w = 365.0f;
                float h = 64.0f;
                float gap = 13.0f;
                if (desenharBotaoPremium((Rectangle){x, y, w, h},
                    "INICIAR PARTIDA", "Comece uma nova jornada de 15 perguntas.", "01  /  PLAY", true)) {
                    executarPartidaRaylib(cadastro, &audio, audioDisponivel, &fundo, videoDisponivel);
                    snprintf(mensagemMenu, sizeof(mensagemMenu), "Você retornou ao centro de comando.");
                    instanteMensagem = GetTime() + 2.5;
                }
                y += h + gap;
                if (desenharBotaoPremium((Rectangle){x, y, w, h},
                    "BANCO DE PERGUNTAS", "Consulte e gerencie o acervo de questões.", "02  /  DATA", false)) {
                    telaAtual = MENU_BANCO;
                    scrollBanco = 0.0f;
                    idSelecionado = 0;
                }
                y += h + gap;
                if (desenharBotaoPremium((Rectangle){x, y, w, h},
                    "HALL DA FAMA", "Confira os melhores resultados registrados.", "03  /  RANK", false)) {
                    telaAtual = MENU_RANKING;
                }
                y += h + gap;
                if (desenharBotaoPremium((Rectangle){x, y, w, h},
                    "CENTRAL DE AJUDA", "Entenda as quatro ferramentas da partida.", "04  /  HELP", false)) {
                    telaAtual = MENU_AJUDA;
                }
                y += h + gap;
                if (desenharBotaoPremium((Rectangle){x, y, w, h},
                    "CONFIGURAÇÕES", "Ajuste a ambiencia e os efeitos sonoros.", "05  /  SET", false)) {
                    telaAtual = MENU_CONFIGURACOES;
                }
                y += h + gap;
                if (desenharBotaoPremium((Rectangle){x, y, w, h},
                    "SALVAR E SAIR", "Grave o banco atualizado e encerre o Cosmos.", "06  /  EXIT", false)) {
                    confirmarSaida = true;
                }

                DrawText("UTFPR  |  ENGENHARIA DE COMPUTAÇÃO  |  V.1.0.0",
                         66, (int)altura - 34, 10, (Color){185, 205, 226, 190});

                DrawText("SOBRE NÓS",
                         (int)largura - 100, (int)altura - 27, 10, COR_CIANO);
                DrawText("WASD / MOUSE  -  NAVEGACAO", (int)largura - 442,
                         (int)altura - 27, 9, (Color){185, 205, 226, 190});
                if (mensagemMenu[0] != '\0') {
                    DrawText(mensagemMenu, 66, (int)altura - 54, 11, COR_DOURADO);
                }

                if (confirmarSaida) {
                    Rectangle modal = {largura / 2.0f - 220.0f,
                                       altura / 2.0f - 100.0f, 440, 200};
                    DrawRectangle(0, 0, (int)largura, (int)altura, (Color){0, 0, 0, 180});
                    desenharPainel(modal, (Color){7, 20, 42, 250}, COR_DOURADO);
                    DrawText("SALVAR E ENCERRAR?", (int)modal.x + 35,
                             (int)modal.y + 30, 20, COR_TEXTO);
                    DrawText("O banco de perguntas será gravado em arquivo binario.",
                             (int)modal.x + 35, (int)modal.y + 68, 12, COR_TEXTO_SEC);
                    Rectangle confirmar = {modal.x + 35, modal.y + 118, 165, 48};
                    Rectangle cancelar = {modal.x + 240, modal.y + 118, 165, 48};
                    if (desenharBotaoPremium(confirmar, "CONFIRMAR", NULL, NULL, true)) {
                        if (salvarBinario(cadastro, CAMINHO_BINARIO)) {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Banco salvo com sucesso.");
                            executando = false;
                        } else {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Falha ao salvar o arquivo binario.");
                            instanteMensagem = GetTime() + 3.0;
                            confirmarSaida = false;
                        }
                    }
                    if (desenharBotaoPremium(cancelar, "CANCELAR", NULL, NULL, false)) {
                        confirmarSaida = false;
                    }
                }
            } else if (telaAtual == MENU_BANCO) {
                bool modalAberto = formulario.ativo || confirmarExclusao;
                desenharTelaBanco(cadastro, filtroNivel, &scrollBanco, mensagemMenu,
                                  &idSelecionado, !modalAberto);

                if (!modalAberto) {
                    float larguraFiltro = (largura - 100.0f) / 6.0f;
                    for (int i = 0; i < 6; i++) {
                        Rectangle area = {50.0f + i * larguraFiltro, 208.0f,
                                          larguraFiltro - 7.0f, 31.0f};
                        if (CheckCollisionPointRec(mouse, area) &&
                            IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                            filtroNivel = i;
                            scrollBanco = 0.0f;
                            idSelecionado = 0;
                        }
                    }

                    float yBotoes = altura - 65.0f;
                    if (desenharBotaoPremium((Rectangle){35, yBotoes, 90, 38},
                                             "VOLTAR", NULL, NULL, false) ||
                        IsKeyPressed(KEY_BACKSPACE)) {
                        telaAtual = MENU_INICIO;
                        mensagemMenu[0] = '\0';
                    }
                    if (desenharBotaoPremium((Rectangle){132, yBotoes, 112, 38},
                                             "ADICIONAR", NULL, NULL, true)) {
                        abrirFormularioNovaPergunta(&formulario, cadastro);
                        if (formulario.pergunta.id <= 0) {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Nao ha IDs disponiveis para novas perguntas.");
                            formulario.ativo = false;
                        }
                    }
                    if (desenharBotaoPremium((Rectangle){251, yBotoes, 100, 38},
                                             "EDITAR", NULL, NULL, false)) {
                        if (!abrirFormularioEdicao(&formulario, cadastro, idSelecionado)) {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Selecione uma pergunta para editar.");
                            instanteMensagem = GetTime() + 3.0;
                        }
                    }
                    if (desenharBotaoPremium((Rectangle){358, yBotoes, 105, 38},
                                             "EXCLUIR", NULL, NULL, false)) {
                        if (buscarPerguntaPorId(cadastro, idSelecionado) != NULL) {
                            confirmarExclusao = true;
                            idParaExcluir = idSelecionado;
                        } else {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Selecione uma pergunta para excluir.");
                            instanteMensagem = GetTime() + 3.0;
                        }
                    }
                    if (desenharBotaoPremium((Rectangle){470, yBotoes, 125, 38},
                                             "SALVAR", NULL, NULL, false)) {
                        if (salvarBinario(cadastro, CAMINHO_BINARIO)) {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Banco salvo em %s.", CAMINHO_BINARIO);
                        } else {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Falha ao salvar o banco de perguntas.");
                        }
                        instanteMensagem = GetTime() + 3.0;
                    }
                    if (desenharBotaoPremium((Rectangle){602, yBotoes, 150, 38},
                                             "EXPORTAR CSV", NULL, NULL, false)) {
                        if (exportarCSV(cadastro, CAMINHO_CSV_EXPORT)) {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "CSV exportado para %s.", CAMINHO_CSV_EXPORT);
                        } else {
                            snprintf(mensagemMenu, sizeof(mensagemMenu), "Falha ao exportar o CSV.");
                        }
                        instanteMensagem = GetTime() + 3.0;
                    }
                    DrawText("SELECIONE UMA LINHA PARA EDITAR OU EXCLUIR",
                             775, (int)altura - 51, 9, COR_TEXTO_SEC);
                }

                if (formulario.ativo) {
                    processarEntradaFormulario(&formulario);
                    /* Cliques nas caixas mudam o campo que recebe a digitacao. */
                    float larguraModal = largura < 940.0f ? largura - 36.0f : 900.0f;
                    float alturaModal = altura < 700.0f ? altura - 28.0f : 650.0f;
                    float xModal = (largura - larguraModal) / 2.0f;
                    float yModal = (altura - alturaModal) / 2.0f;
                    float esquerda = xModal + 30.0f;
                    float larguraCampo = larguraModal - 60.0f;
                    Rectangle campoEnunciado = {esquerda, yModal + 111.0f, larguraCampo, 44.0f};
                    float larguraAlt = (larguraCampo - 22.0f) / 2.0f;
                    float x2 = esquerda + larguraAlt + 22.0f;
                    Rectangle campos[QUANT_ALTERNATIVAS] = {
                        {esquerda, yModal + 207.0f, larguraAlt, 42.0f},
                        {x2, yModal + 207.0f, larguraAlt, 42.0f},
                        {esquerda, yModal + 280.0f, larguraAlt, 42.0f},
                        {x2, yModal + 280.0f, larguraAlt, 42.0f}
                    };
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                        if (CheckCollisionPointRec(mouse, campoEnunciado)) formulario.campoAtivo = 0;
                        for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
                            if (CheckCollisionPointRec(mouse, campos[i])) formulario.campoAtivo = i + 1;
                        }
                    }
                    desenharFormularioPergunta(&formulario, cadastro, &idSelecionado,
                        mensagemMenu, sizeof(mensagemMenu), &instanteMensagem);
                } else if (confirmarExclusao) {
                    if (desenharConfirmacaoExclusao(cadastro, idParaExcluir,
                        mensagemMenu, sizeof(mensagemMenu), &instanteMensagem)) {
                        confirmarExclusao = false;
                        if (buscarPerguntaPorId(cadastro, idParaExcluir) == NULL) idSelecionado = 0;
                        idParaExcluir = 0;
                    }
                }
            } else if (telaAtual == MENU_RANKING) {
                desenharPainelRankingMenu();
                if (desenharBotaoPremium((Rectangle){42, altura - 65.0f, 120, 38},
                                         "VOLTAR", NULL, NULL, false) ||
                    IsKeyPressed(KEY_BACKSPACE)) telaAtual = MENU_INICIO;
            } else if (telaAtual == MENU_AJUDA) {
                desenharTelaAjudaMenu();
                if (desenharBotaoPremium((Rectangle){42, altura - 65.0f, 120, 38},
                                         "VOLTAR", NULL, NULL, false) ||
                    IsKeyPressed(KEY_BACKSPACE)) telaAtual = MENU_INICIO;
            } else if (telaAtual == MENU_CONFIGURACOES) {
                desenharTelaConfiguracoes(audioDisponivel, audioAtivo,
                                          volumeAmbiente, volumeEfeitos);
                Rectangle alternarAudio = {105, 280, 160, 40};
                Rectangle menosAmbiente = {largura - 285, 340, 55, 34};
                Rectangle maisAmbiente = {largura - 215, 340, 55, 34};
                Rectangle menosEfeitos = {largura - 285, 420, 55, 34};
                Rectangle maisEfeitos = {largura - 215, 420, 55, 34};
                if (desenharBotaoPremium(alternarAudio,
                    audioAtivo ? "SILENCIAR" : "ATIVAR AUDIO", NULL, NULL, true) && audioDisponivel) {
                    audioAtivo = !audioAtivo;
                    definirVolumeAmbienteAudio(&audio, audioAtivo ? volumeAmbiente : 0.0f);
                    definirVolumeEfeitosAudio(&audio, audioAtivo ? volumeEfeitos : 0.0f);
                }
                if (desenharBotaoPremium(menosAmbiente, "-", NULL, NULL, false)) {
                    volumeAmbiente -= 0.05f;
                    if (volumeAmbiente < 0.0f) volumeAmbiente = 0.0f;
                    if (audioDisponivel && audioAtivo) definirVolumeAmbienteAudio(&audio, volumeAmbiente);
                }
                if (desenharBotaoPremium(maisAmbiente, "+", NULL, NULL, false)) {
                    volumeAmbiente += 0.05f;
                    if (volumeAmbiente > 1.0f) volumeAmbiente = 1.0f;
                    if (audioDisponivel && audioAtivo) definirVolumeAmbienteAudio(&audio, volumeAmbiente);
                }
                if (desenharBotaoPremium(menosEfeitos, "-", NULL, NULL, false)) {
                    volumeEfeitos -= 0.05f;
                    if (volumeEfeitos < 0.0f) volumeEfeitos = 0.0f;
                    if (audioDisponivel && audioAtivo) definirVolumeEfeitosAudio(&audio, volumeEfeitos);
                }
                if (desenharBotaoPremium(maisEfeitos, "+", NULL, NULL, false)) {
                    volumeEfeitos += 0.05f;
                    if (volumeEfeitos > 1.0f) volumeEfeitos = 1.0f;
                    if (audioDisponivel && audioAtivo) definirVolumeEfeitosAudio(&audio, volumeEfeitos);
                }
                if (desenharBotaoPremium((Rectangle){42, altura - 65.0f, 120, 38},
                                         "VOLTAR", NULL, NULL, false) ||
                    IsKeyPressed(KEY_BACKSPACE)) telaAtual = MENU_INICIO;
            }
        EndDrawing();
    }

    if (videoDisponivel) VideoClose(&fundo);
    if (fonteCarregada) UnloadFont(fonteCosmos);
    liberarAudioJogo(&audio);
    CloseWindow();
}
