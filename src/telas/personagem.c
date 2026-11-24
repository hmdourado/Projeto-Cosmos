/**
 * @file personagem.c
 * @brief Implementacao do personagem ficticio e de suas animacoes na Raylib.
 */
#include "../../include/personagem.h"

#include <math.h>
#include <stddef.h>

// Define as duracoes dos efeitos temporarios para cada acao do personagem.
static float obterDuracaoAcao(AcaoPersonagem acao) {
    switch (acao) {
        case PERSONAGEM_ACERTO:
            return 1.15f;
        case PERSONAGEM_ERRO:
            return 0.95f;
        case PERSONAGEM_MARCO:
            return 1.8f;
        case PERSONAGEM_VITORIA:
            return 3.0f;
        case PERSONAGEM_PENSANDO:
        case PERSONAGEM_PARADO:
        default:
            return 0.0f;
    }
}

// Inicializa a estrutura e carrega a imagem com fundo transparente.
bool inicializarPersonagemCosmos(PersonagemCosmos *personagem, const char *caminho) {
    if (personagem == NULL || caminho == NULL || caminho[0] == '\0') {
        return false;
    }

    *personagem = (PersonagemCosmos){0};

    if (!FileExists(caminho)) {
        TraceLog(LOG_WARNING, "Imagem do personagem nao encontrada: %s", caminho);
        return false;
    }

    personagem->textura = LoadTexture(caminho);

    if (personagem->textura.id == 0) {
        TraceLog(LOG_WARNING, "Nao foi possivel carregar a imagem do personagem.");
        return false;
    }

    SetTextureFilter(personagem->textura, TEXTURE_FILTER_BILINEAR);

    personagem->carregado = true;
    personagem->acao = PERSONAGEM_PARADO;
    personagem->tempo = 0.0f;
    personagem->tempoAcao = 0.0f;
    personagem->duracaoAcao = 0.0f;

    return true;
}

// Define a acao solicitada e reinicia o tempo do efeito correspondente.
void definirAcaoPersonagem(PersonagemCosmos *personagem, AcaoPersonagem acao) {
    if (personagem == NULL || !personagem->carregado) {
        return;
    }

    personagem->acao = acao;
    personagem->tempoAcao = 0.0f;
    personagem->duracaoAcao = obterDuracaoAcao(acao);
}

// Atualiza o movimento continuo e encerra automaticamente as acoes temporarias.
void atualizarPersonagemCosmos(PersonagemCosmos *personagem, float delta) {
    if (personagem == NULL || !personagem->carregado || delta <= 0.0f) {
        return;
    }

    personagem->tempo += delta;
    personagem->tempoAcao += delta;

    // Retorna ao estado parado depois de concluir uma reacao temporaria.
    if (personagem->duracaoAcao > 0.0f &&
        personagem->tempoAcao >= personagem->duracaoAcao) {
        personagem->acao = PERSONAGEM_PARADO;
        personagem->tempoAcao = 0.0f;
        personagem->duracaoAcao = 0.0f;
    }
}

// Calcula a escala e a inclinacao para representar a acao atual.
static void obterTransformacaoPersonagem(
    const PersonagemCosmos *personagem,
    float *escala,
    float *rotacao,
    float *deslocamentoY
) {
    float tempo = personagem->tempo;
    float acaoTempo = personagem->tempoAcao;

    *escala = 1.0f + 0.008f * sinf(tempo * 1.8f);
    *rotacao = 0.7f * sinf(tempo * 1.1f);
    *deslocamentoY = 4.0f * sinf(tempo * 1.8f);

    switch (personagem->acao) {
        case PERSONAGEM_PENSANDO:
            *rotacao -= 2.0f;
            *deslocamentoY -= 2.0f * sinf(acaoTempo * 2.0f);
            break;

        case PERSONAGEM_ACERTO:
            *escala += 0.035f * sinf(acaoTempo * 10.0f);
            *deslocamentoY -= 9.0f * fabsf(sinf(acaoTempo * 8.0f));
            break;

        case PERSONAGEM_ERRO:
            *rotacao += 2.0f * sinf(acaoTempo * 35.0f) *
                        (1.0f - acaoTempo / personagem->duracaoAcao);
            break;

        case PERSONAGEM_MARCO:
            *escala += 0.025f * sinf(acaoTempo * 8.0f);
            *deslocamentoY -= 5.0f * fabsf(sinf(acaoTempo * 5.0f));
            break;

        case PERSONAGEM_VITORIA:
            *escala += 0.045f * fabsf(sinf(acaoTempo * 7.0f));
            *deslocamentoY -= 10.0f * fabsf(sinf(acaoTempo * 7.0f));
            break;

        case PERSONAGEM_PARADO:
        default:
            break;
    }
}

// Desenha a textura centralizada, mantendo a proporcao original da imagem.
void desenharPersonagemCosmos(const PersonagemCosmos *personagem, Rectangle area) {
    if (personagem == NULL || !personagem->carregado || area.width <= 0.0f || area.height <= 0.0f) {
        return;
    }

    float larguraTextura = (float)personagem->textura.width;
    float alturaTextura = (float)personagem->textura.height;

    if (larguraTextura <= 0.0f || alturaTextura <= 0.0f) {
        return;
    }

    // Ajusta a imagem a area disponivel sem distorcer o personagem.
    float escalaBase = fminf(area.width / larguraTextura, area.height / alturaTextura);
    float escalaAnimacao;
    float rotacao;
    float deslocamentoY;

    obterTransformacaoPersonagem(
        personagem,
        &escalaAnimacao,
        &rotacao,
        &deslocamentoY
    );

    float larguraDestino = larguraTextura * escalaBase * escalaAnimacao;
    float alturaDestino = alturaTextura * escalaBase * escalaAnimacao;

    Rectangle origem = {
        0.0f,
        0.0f,
        larguraTextura,
        alturaTextura
    };

    Rectangle destino = {
        area.x + (area.width - larguraDestino) / 2.0f,
        area.y + (area.height - alturaDestino) / 2.0f + deslocamentoY,
        larguraDestino,
        alturaDestino
    };

    Vector2 origemRotacao = {
        larguraDestino / 2.0f,
        alturaDestino / 2.0f
    };

    Color tonalidade = WHITE;

    // Aplica uma tonalidade discreta durante os efeitos de resposta.
    if (personagem->acao == PERSONAGEM_ACERTO) {
        tonalidade = (Color){220, 255, 240, 255};
    } else if (personagem->acao == PERSONAGEM_ERRO) {
        tonalidade = (Color){255, 220, 220, 255};
    } else if (personagem->acao == PERSONAGEM_MARCO ||
               personagem->acao == PERSONAGEM_VITORIA) {
        tonalidade = (Color){255, 244, 205, 255};
    }

    DrawTexturePro(
        personagem->textura,
        origem,
        destino,
        origemRotacao,
        rotacao,
        tonalidade
    );
}

// Libera a textura e reinicializa a estrutura para evitar reutilizacao indevida.
void liberarPersonagemCosmos(PersonagemCosmos *personagem) {
    if (personagem == NULL) {
        return;
    }

    if (personagem->carregado) {
        UnloadTexture(personagem->textura);
    }

    *personagem = (PersonagemCosmos){0};
}
