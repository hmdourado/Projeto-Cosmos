/**
 * @file preview_tela_jogo.c
 * @brief Implementação da pré-visualização cinematográfica e interativa da partida.
 */
#include "../include/telas.h"
#include "../include/jogo.h"
#include "../include/cadastro.h"
#include "../include/arquivo.h"
#include "../include/audio.h"
#include "../include/video.h"
#include "../include/personagem.h"
#include "../include/ajuda.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Transfere para a interface a disponibilidade das ajudas e as eliminações atuais.
static void atualizarEstadoAjudasTela(
    const Partida *partida,
    DadosTelaJogo *dados
){
    if (partida == NULL || dados == NULL) {
        return;
    }

    // Uma ajuda permanece disponível enquanto ainda não tiver sido utilizada.
    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        dados->ajudasDisponiveis[i] = !partida->ajudas.usadas[i];
    }

    // Reflete na tela as alternativas eliminadas pela Supernova.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        dados->alternativasEliminadas[i] = partida->ajudas.eliminadas[i] != 0;
    }
}

// Converte a ação recebida da interface para o tipo reconhecido pelo módulo de ajuda.
static bool obterTipoAjuda(AcaoTelaJogo acao, TipoAjuda *tipo){
    if (tipo == NULL) {
        return false;
    }

    switch (acao) {
        case ACAO_TELA_JOGO_AJUDA_SUPERNOVA:
            *tipo = AJUDA_SUPERNOVA;
            return true;

        case ACAO_TELA_JOGO_AJUDA_NASA:
            *tipo = AJUDA_MISSAO;
            return true;

        case ACAO_TELA_JOGO_AJUDA_OBSERVATORIO:
            *tipo = AJUDA_TELESCOPIO;
            return true;

        case ACAO_TELA_JOGO_AJUDA_ESPECIALISTA:
            *tipo = AJUDA_ESPECIALISTA;
            return true;

        default:
            return false;
    }
}

// Atualiza os dados apresentados pela interface com as informações atuais da partida.
static void atualizarDadosTela(
    const Partida *partida,
    DadosTelaJogo *dados,
    const char *mensagem
){
    if (partida == NULL || dados == NULL) {
        return;
    }

    // Inicializa os dados visuais antes de preencher as informações da partida.
    *dados = (DadosTelaJogo){0};

    // Obtém a pergunta correspondente ao estado atual da partida.
    dados->pergunta = obterPerguntaAtual(partida);

    if (dados->pergunta == NULL) {
        return;
    }

    // Transfere o progresso, a premiação e a mensagem para a interface.
    dados->numeroPergunta = (unsigned int)partida->indiceAtual + 1;
    dados->totalPerguntas = TOTAL_PERGUNTAS_PARTIDA;
    dados->premioAtual = partida->premioAtual;
    dados->premioGarantido = partida->premioGarantido;
    dados->mensagemStatus = mensagem;

    // Mantém os botões e as alternativas sincronizados com o estado da partida.
    atualizarEstadoAjudasTela(partida, dados);
}

// Converte a ação da interface na letra utilizada pelo módulo de jogo.
static char obterLetraResposta(AcaoTelaJogo acao){
    switch (acao) {
        case ACAO_TELA_JOGO_RESPOSTA_A:
            return 'A';
        case ACAO_TELA_JOGO_RESPOSTA_B:
            return 'B';
        case ACAO_TELA_JOGO_RESPOSTA_C:
            return 'C';
        case ACAO_TELA_JOGO_RESPOSTA_D:
            return 'D';
        default:
            return '\0';
    }
}

// Desenha o painel apresentado quando a partida é encerrada.
static void desenharResultadoFinal(
    const Partida *partida,
    bool vitoria,
    bool desistencia
){
    if (partida == NULL) {
        return;
    }

    float largura = (float)GetScreenWidth();
    float altura = (float)GetScreenHeight();

    // Sobrepõe a interface para destacar o encerramento da partida.
    DrawRectangle(
        0,
        0,
        (int)largura,
        (int)altura,
        (Color){0, 0, 0, 205}
    );

    Rectangle painel = {
        largura / 2.0f - 250.0f,
        altura / 2.0f - 160.0f,
        500.0f,
        320.0f
    };

    // Apresenta o painel holográfico de resultado.
    DrawRectangleRounded(
        painel,
        0.08f,
        10,
        (Color){10, 23, 47, 245}
    );
    DrawRectangleRoundedLinesEx(
        painel,
        0.08f,
        10,
        2.0f,
        (Color){54, 105, 145, 255}
    );

    const char *titulo;
    Color corTitulo;

    // Define a mensagem conforme o motivo do encerramento.
    if (vitoria) {
        titulo = "PARABENS! VOCE VENCEU!";
        corTitulo = (Color){65, 225, 157, 255};
    } else if (desistencia) {
        titulo = "VOCE DESISTIU";
        corTitulo = (Color){255, 197, 87, 255};
    } else {
        titulo = "FIM DE JOGO";
        corTitulo = (Color){235, 76, 91, 255};
    }

    int larguraTitulo = MeasureText(titulo, 25);
    DrawText(
        titulo,
        (int)(painel.x + (painel.width - larguraTitulo) / 2.0f),
        (int)painel.y + 55,
        25,
        corTitulo
    );

    DrawText(
        TextFormat("Acertos: %d / %d", partida->acertos, TOTAL_PERGUNTAS_PARTIDA),
        (int)painel.x + 35,
        (int)painel.y + 125,
        22,
        WHITE
    );

    DrawText(
        TextFormat("Premio final: R$ %d", partida->premioAtual),
        (int)painel.x + 35,
        (int)painel.y + 170,
        22,
        (Color){255, 197, 87, 255}
    );

    DrawText(
        "Feche a janela para encerrar a visualizacao.",
        (int)painel.x + 35,
        (int)painel.y + 245,
        15,
        (Color){157, 184, 211, 255}
    );
}

// Atualiza os efeitos sonoros de hover somente quando o cursor entra em um botão.
static void atualizarSonsHover(
    AudioJogo *audio,
    bool audioDisponivel,
    const EstadoVisualTelaJogo *estado,
    bool hoverAlternativaAnterior[QUANT_ALTERNATIVAS],
    bool hoverAjudaAnterior[QUANT_AJUDAS_TELA]
){
    if (!audioDisponivel || estado == NULL) {
        return;
    }

    // Evita repetir o som enquanto o cursor permanece sobre a mesma alternativa.
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
        bool hoverAtual = estado->hoverAlternativas[i] >= 0.75f;

        if (hoverAtual && !hoverAlternativaAnterior[i]) {
            reproduzirEfeitoAudio(audio, EFEITO_HOVER);
        }

        hoverAlternativaAnterior[i] = hoverAtual;
    }

    // Aplica o mesmo comportamento aos botões de ajuda disponíveis.
    for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
        bool hoverAtual = estado->hoverAjudas[i] >= 0.75f;

        if (hoverAtual && !hoverAjudaAnterior[i]) {
            reproduzirEfeitoAudio(audio, EFEITO_HOVER);
        }

        hoverAjudaAnterior[i] = hoverAtual;
    }
}

int main(int argc, char *argv[]){
    // Impede a execução com uma quantidade inválida de argumentos.
    if (argc > 3) {
        fprintf(stderr, "Uso: %s [arquivo.bin [arquivo.csv]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Define os caminhos dos arquivos utilizados na pré-visualização.
    const char *binario = argc > 1 ? argv[1] : "dados/perguntas.dat";
    const char *csv = argc > 2 ? argv[2] : "dados/perguntas.csv";

    // Inicializa o gerador aleatório utilizado na seleção das perguntas.
    srand((unsigned int)time(NULL));

    // Inicializa o cadastro antes de carregar os dados persistidos.
    Cadastro cadastro;
    inicializarCadastro(&cadastro);

    // Prioriza o arquivo binário para recuperar as perguntas cadastradas.
    if (!carregarBinario(&cadastro, binario)) {
        fprintf(stderr, "Binario ausente ou invalido. Tentando importar CSV.\n");

        // Descarta possíveis registros parciais antes de importar o CSV.
        liberarCadastro(&cadastro);
        inicializarCadastro(&cadastro);

        // Interrompe o teste caso nenhuma fonte de perguntas esteja disponível.
        if (!carregarCSV(&cadastro, csv)) {
            fprintf(stderr, "Nao foi possivel carregar o CSV: %s\n", csv);
            liberarCadastro(&cadastro);
            return EXIT_FAILURE;
        }
    }

    // Inicializa e prepara a partida com as quinze perguntas sorteadas.
    Partida partida;
    inicializarPartida(&partida);

    ResultadoInicio resultadoInicio = iniciarPartida(&partida, &cadastro);
    if (resultadoInicio != INICIO_SUCESSO) {
        fprintf(
            stderr,
            "Nao foi possivel iniciar a partida. Codigo: %d\n",
            resultadoInicio
        );
        liberarCadastro(&cadastro);
        return EXIT_FAILURE;
    }

    // Prepara os dados iniciais apresentados pela interface cinematográfica.
    DadosTelaJogo dadosTela = {0};
    atualizarDadosTela(&partida, &dadosTela, "Selecione uma alternativa.");

    // Cria a janela antes de carregar texturas e inicializar os recursos de áudio.
    InitWindow(1280, 720, "Cosmos - Pre-visualizacao cinematografica");
    if (!IsWindowReady()) {
        fprintf(stderr, "Nao foi possivel inicializar a janela Raylib.\n");
        liberarCadastro(&cadastro);
        return EXIT_FAILURE;
    }
    SetTargetFPS(60);

    // Inicializa as estrelas e carrega a apresentadora dentro do estado visual.
    EstadoVisualTelaJogo estadoVisual = {0};
    inicializarEstadoVisualTelaJogo(&estadoVisual);

    // Inicializa o sistema de áudio depois da criação da janela Raylib.
    AudioJogo audio = {0};
    bool audioDisponivel = inicializarAudioJogo(&audio);

    // Inicia a ambiência e configura os volumes quando o áudio estiver disponível.
    if (audioDisponivel) {
        definirVolumeAmbienteAudio(&audio, 0.22f);
        definirVolumeEfeitosAudio(&audio, 0.70f);
        iniciarAmbienteAudio(&audio);
    } else {
        TraceLog(LOG_WARNING, "Audio indisponivel na pre-visualizacao.");
    }

    // Inicializa o vídeo espacial como fundo opcional da pré-visualização.
    VideoBackground fundo = {0};
    bool videoDisponivel = VideoInit(
        &fundo,
        "assets/14014441_1280_720_30fps.mp4"
    );

    if (!videoDisponivel) {
        TraceLog(LOG_WARNING, "Video de fundo indisponivel. Utilizando o fundo grafico da tela.");
    }

    bool partidaEncerrada = false;
    bool vitoria = false;
    bool desistencia = false;
    char mensagemAjuda[256] = "";

    bool aguardandoProximaPergunta = false;
    double instanteProximaPergunta = 0.0;
    double fimMensagem = 0.0;
    Pergunta perguntaEmFeedback = {0};

    // Armazena o estado anterior do hover para evitar efeitos sonoros repetidos.
    bool hoverAlternativaAnterior[QUANT_ALTERNATIVAS] = {false};
    bool hoverAjudaAnterior[QUANT_AJUDAS_TELA] = {false};

    // Mantém a interface ativa enquanto a janela estiver aberta.
    while (!WindowShouldClose()) {
        // Atualiza os recursos contínuos de vídeo e áudio a cada quadro.
        if (videoDisponivel) {
            VideoUpdate(&fundo);
        }
        if (audioDisponivel) {
            atualizarAudioJogo(&audio);
        }

        // Atualiza as estrelas, os efeitos de hover e a animação da apresentadora.
        atualizarAnimacaoTelaJogo(&estadoVisual, &dadosTela);

        // Troca para a próxima pergunta somente após exibir o feedback da resposta.
        if (aguardandoProximaPergunta && GetTime() >= instanteProximaPergunta) {
            atualizarDadosTela(
                &partida,
                &dadosTela,
                "Nova pergunta. Boa sorte!"
            );

            registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_NENHUM);
            aguardandoProximaPergunta = false;
            fimMensagem = GetTime() + 1.4;

            // Reinicia os estados de hover antes de apresentar a nova pergunta.
            for (int i = 0; i < QUANT_ALTERNATIVAS; i++) {
                hoverAlternativaAnterior[i] = false;
            }
            for (int i = 0; i < QUANT_AJUDAS_TELA; i++) {
                hoverAjudaAnterior[i] = false;
            }

            // Reproduz o efeito de transição no momento em que a nova pergunta aparece.
            if (audioDisponivel) {
                reproduzirEfeitoAudio(&audio, EFEITO_TRANSICAO);
            }
        }

        // Processa as interações somente enquanto a partida estiver ativa e sem transição.
        if (!partidaEncerrada && !aguardandoProximaPergunta) {
            // Restaura a mensagem padrão após o período de feedback textual.
            if (fimMensagem > 0.0 && GetTime() >= fimMensagem) {
                dadosTela.mensagemStatus = "Selecione uma alternativa.";
                mensagemAjuda[0] = '\0';
                fimMensagem = 0.0;
            }

            // Reproduz o som quando o cursor entra em uma alternativa ou ajuda.
            atualizarSonsHover(
                &audio,
                audioDisponivel,
                &estadoVisual,
                hoverAlternativaAnterior,
                hoverAjudaAnterior
            );

            // Identifica a ação realizada pelo jogador na interface.
            AcaoTelaJogo acao = atualizarTelaJogo(&dadosTela, &estadoVisual);

            // Reproduz o efeito de seleção uma única vez por ação identificada.
            if (audioDisponivel && acao != ACAO_TELA_JOGO_NENHUMA) {
                reproduzirEfeitoAudio(&audio, EFEITO_SELECAO);
            }

            // Encaminha a alternativa selecionada para a lógica da partida.
            char resposta = obterLetraResposta(acao);
            if (resposta != '\0') {
                int numeroPerguntaRespondida = partida.indiceAtual + 1;

                // Preserva uma cópia da pergunta para exibir o feedback antes da próxima.
                if (dadosTela.pergunta != NULL) {
                    perguntaEmFeedback = *dadosTela.pergunta;
                }

                ResultadoResposta resultado = responderPergunta(&partida, resposta);

                switch (resultado) {
                    case RESPOSTA_CORRETA: {
                        bool marcoSeguranca =
                            numeroPerguntaRespondida == 5 ||
                            numeroPerguntaRespondida == 10;

                        // Registra a reação visual e sonora correspondente ao acerto.
                        if (marcoSeguranca) {
                            registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_MARCO);
                            if (audioDisponivel) {
                                reproduzirEfeitoAudio(&audio, EFEITO_MARCO_SEGURANCA);
                            }
                        } else {
                            registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_ACERTO);
                            if (audioDisponivel) {
                                reproduzirEfeitoAudio(&audio, EFEITO_RESPOSTA_CORRETA);
                            }
                        }

                        // Mantém a pergunta respondida visível enquanto o feedback é apresentado.
                        dadosTela.pergunta = &perguntaEmFeedback;
                        dadosTela.numeroPergunta = (unsigned int)numeroPerguntaRespondida;
                        dadosTela.totalPerguntas = TOTAL_PERGUNTAS_PARTIDA;
                        dadosTela.premioAtual = partida.premioAtual;
                        dadosTela.premioGarantido = partida.premioGarantido;
                        dadosTela.mensagemStatus = marcoSeguranca
                            ? "Marco de seguranca conquistado!"
                            : "Resposta correta!";

                        aguardandoProximaPergunta = true;
                        instanteProximaPergunta = GetTime() + (marcoSeguranca ? 1.9 : 1.2);

                        TraceLog(
                            LOG_INFO,
                            "Resposta correta na pergunta %d.",
                            numeroPerguntaRespondida
                        );
                        break;
                    }

                    case RESPOSTA_VITORIA:
                        // Registra a comemoração da apresentadora e reproduz a fanfarra final.
                        registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_ACERTO);
                        definirAcaoPersonagem(&estadoVisual.personagem, PERSONAGEM_VITORIA);

                        if (audioDisponivel) {
                            reproduzirEfeitoAudio(&audio, EFEITO_VITORIA);
                        }

                        dadosTela.premioAtual = partida.premioAtual;
                        dadosTela.premioGarantido = partida.premioGarantido;
                        dadosTela.mensagemStatus = "Todas as perguntas foram respondidas!";
                        partidaEncerrada = true;
                        vitoria = true;

                        TraceLog(LOG_INFO, "Partida concluida com vitoria.");
                        break;

                    case RESPOSTA_INCORRETA:
                        // Registra a reação de erro e atualiza a premiação final.
                        registrarFeedbackTelaJogo(&estadoVisual, FEEDBACK_ERRO);
                        if (audioDisponivel) {
                            reproduzirEfeitoAudio(&audio, EFEITO_RESPOSTA_INCORRETA);
                        }

                        dadosTela.premioAtual = partida.premioAtual;
                        dadosTela.premioGarantido = partida.premioGarantido;
                        dadosTela.mensagemStatus = "Resposta incorreta!";
                        partidaEncerrada = true;

                        TraceLog(LOG_INFO, "Resposta incorreta. Partida encerrada.");
                        break;

                    case RESPOSTA_INVALIDA:
                        // Informa que a tentativa não corresponde a uma alternativa válida.
                        dadosTela.mensagemStatus = "Alternativa invalida.";
                        fimMensagem = GetTime() + 1.5;
                        break;

                    case RESPOSTA_NAO_PERMITIDA:
                        TraceLog(LOG_WARNING, "Resposta nao permitida no estado atual.");
                        break;
                }
            }
            // Processa a opção de desistência.
            else if (acao == ACAO_TELA_JOGO_DESISTIR) {
                if (desistirPartida(&partida)) {
                    dadosTela.premioAtual = partida.premioAtual;
                    dadosTela.premioGarantido = partida.premioGarantido;
                    dadosTela.mensagemStatus = "Partida encerrada por desistencia.";
                    partidaEncerrada = true;
                    desistencia = true;

                    TraceLog(LOG_INFO, "Partida encerrada por desistencia.");
                }
            }
            // Encaminha a ação selecionada para o módulo responsável pelas regras das ajudas.
            else {
                TipoAjuda tipoAjuda;

                if (obterTipoAjuda(acao, &tipoAjuda)) {
                    const Pergunta *pergunta = obterPerguntaAtual(&partida);
                    ResultadoAjuda resultadoAjuda = aplicarAjuda(
                        &partida.ajudas,
                        pergunta,
                        tipoAjuda
                    );

                    if (resultadoAjuda == AJUDA_SUCESSO && pergunta != NULL) {
                        // Atualiza imediatamente os botões e as alternativas após a aplicação.
                        atualizarEstadoAjudasTela(&partida, &dadosTela);

                        switch (tipoAjuda) {
                            case AJUDA_SUPERNOVA:
                                snprintf(
                                    mensagemAjuda,
                                    sizeof(mensagemAjuda),
                                    "Supernova: duas alternativas incorretas foram eliminadas."
                                );
                                break;

                            case AJUDA_MISSAO:
                            case AJUDA_TELESCOPIO: {
                                const char *pista = obterTextoAjuda(pergunta, tipoAjuda);
                                snprintf(
                                    mensagemAjuda,
                                    sizeof(mensagemAjuda),
                                    "%s",
                                    pista != NULL ? pista : "Pista indisponivel."
                                );
                                break;
                            }

                            case AJUDA_ESPECIALISTA:
                                snprintf(
                                    mensagemAjuda,
                                    sizeof(mensagemAjuda),
                                    "Especialista sugere a alternativa %c (%d%% de chance de acerto).",
                                    partida.ajudas.sugestaoEspecialista,
                                    obterChanceEspecialista(pergunta->nivel)
                                );
                                break;

                            default:
                                snprintf(
                                    mensagemAjuda,
                                    sizeof(mensagemAjuda),
                                    "Ajuda aplicada com sucesso."
                                );
                                break;
                        }

                        dadosTela.mensagemStatus = mensagemAjuda;
                        fimMensagem = GetTime() + 5.5;

                        TraceLog(LOG_INFO, "Ajuda %d aplicada com sucesso.", tipoAjuda);
                    } else {
                        // Explica por que a ajuda não pôde ser aplicada sem consumir outro uso.
                        const char *erro = "Nao foi possivel aplicar a ajuda.";

                        switch (resultadoAjuda) {
                            case AJUDA_JA_USADA:
                                erro = "Esta ajuda ja foi utilizada nesta partida.";
                                break;
                            case AJUDA_SEM_CONTEUDO:
                                erro = "Esta ajuda nao possui conteudo para esta pergunta.";
                                break;
                            case AJUDA_NAO_PERMITIDA:
                                erro = "Esta ajuda nao pode ser aplicada neste momento.";
                                break;
                            case AJUDA_INVALIDA:
                                erro = "Nao foi possivel validar esta ajuda.";
                                break;
                            case AJUDA_SUCESSO:
                                break;
                        }

                        snprintf(mensagemAjuda, sizeof(mensagemAjuda), "%s", erro);
                        dadosTela.mensagemStatus = mensagemAjuda;
                        fimMensagem = GetTime() + 2.5;

                        TraceLog(
                            LOG_WARNING,
                            "Nao foi possivel aplicar a ajuda. Codigo: %d",
                            resultadoAjuda
                        );
                    }
                }
            }
        }

        // Desenha primeiro o vídeo para que a interface cinematográfica fique sobre ele.
        BeginDrawing();
            ClearBackground(BLACK);
            if (videoDisponivel) {
                VideoDraw(&fundo);
            }

            desenharTelaJogo(&dadosTela, &estadoVisual);

            if (partidaEncerrada) {
                desenharResultadoFinal(&partida, vitoria, desistencia);
            }
        EndDrawing();
    }

    // Libera o processo de vídeo quando ele tiver sido inicializado corretamente.
    if (videoDisponivel) {
        VideoClose(&fundo);
    }

    // Libera a textura da apresentadora antes de fechar a janela Raylib.
    liberarEstadoVisualTelaJogo(&estadoVisual);

    // Libera os recursos sonoros antes do encerramento da janela.
    liberarAudioJogo(&audio);

    // Fecha a janela após liberar os recursos gráficos e sonoros.
    CloseWindow();

    // Libera a memória utilizada pelo cadastro.
    liberarCadastro(&cadastro);

    return EXIT_SUCCESS;
}
