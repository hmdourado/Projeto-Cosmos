/**
 * @file telas.h
 * @brief Interfaces das telas da aplicação utilizando Raylib.
 */
#ifndef TELAS_H
#define TELAS_H

#include "raylib.h"
#include "pergunta.h"
#include "personagem.h"

#include <stdbool.h>

#define QUANT_ESTRELAS_TELA_JOGO 70
#define QUANT_AJUDAS_TELA 4

// Armazena os dados da partida utilizados pela interface gráfica.
typedef struct {
    const Pergunta *pergunta;
    unsigned int numeroPergunta;
    unsigned int totalPerguntas;
    int premioAtual;
    int premioGarantido;
    bool alternativasEliminadas[QUANT_ALTERNATIVAS];
    bool ajudasDisponiveis[QUANT_AJUDAS_TELA];
    const char *mensagemStatus;
} DadosTelaJogo;

// Identifica as ações disponíveis na interface da partida.
typedef enum {
    ACAO_TELA_JOGO_NENHUMA = 0,
    ACAO_TELA_JOGO_RESPOSTA_A,
    ACAO_TELA_JOGO_RESPOSTA_B,
    ACAO_TELA_JOGO_RESPOSTA_C,
    ACAO_TELA_JOGO_RESPOSTA_D,
    ACAO_TELA_JOGO_AJUDA_SUPERNOVA,
    ACAO_TELA_JOGO_AJUDA_NASA,
    ACAO_TELA_JOGO_AJUDA_OBSERVATORIO,
    ACAO_TELA_JOGO_AJUDA_ESPECIALISTA,
    ACAO_TELA_JOGO_DESISTIR
} AcaoTelaJogo;

// Identifica os efeitos visuais utilizados após as ações da partida.
typedef enum {
    FEEDBACK_NENHUM = 0,
    FEEDBACK_ACERTO,
    FEEDBACK_ERRO,
    FEEDBACK_MARCO
} FeedbackTelaJogo;

// Armazena a posição e as propriedades visuais de cada estrela animada.
typedef struct {
    Vector2 posicao;
    float velocidade;
    float tamanho;
    float fase;
} EstrelaTelaJogo;

// Mantém o estado das animações, do personagem e das interações visuais.
typedef struct {
    EstrelaTelaJogo estrelas[QUANT_ESTRELAS_TELA_JOGO];
    float tempo;
    float entrada;
    float progressoPremio;
    float hoverAlternativas[QUANT_ALTERNATIVAS];
    float hoverAjudas[QUANT_AJUDAS_TELA];
    float tempoFeedback;
    float deslocamentoPainel;
    int alternativaSelecionada;
    FeedbackTelaJogo feedbackResposta;
    PersonagemCosmos personagem;
    bool personagemDisponivel;
} EstadoVisualTelaJogo;

// Inicializa as propriedades visuais, as estrelas e o personagem da tela.
void inicializarEstadoVisualTelaJogo(EstadoVisualTelaJogo *estado);

// Libera a textura do personagem e reinicializa o estado visual.
void liberarEstadoVisualTelaJogo(EstadoVisualTelaJogo *estado);

// Atualiza as animações conforme o tempo e os dados atuais da partida.
void atualizarAnimacaoTelaJogo(
    EstadoVisualTelaJogo *estado,
    const DadosTelaJogo *dados
);

// Registra o efeito visual correspondente ao resultado da ação do jogador.
void registrarFeedbackTelaJogo(
    EstadoVisualTelaJogo *estado,
    FeedbackTelaJogo feedback
);

// Identifica as interações do jogador e retorna a ação correspondente.
AcaoTelaJogo atualizarTelaJogo(
    const DadosTelaJogo *dados,
    EstadoVisualTelaJogo *estado
);

// Desenha a interface utilizando os dados da partida e o estado das animações.
void desenharTelaJogo(
    const DadosTelaJogo *dados,
    const EstadoVisualTelaJogo *estado
);

// Declara as funções responsáveis pelas demais telas da aplicação.
void desenharTelaCadastro(void);
void desenharTelaAjuda(void);
void desenharTelaRanking(void);

#endif
