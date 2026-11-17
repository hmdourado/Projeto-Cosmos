/**
 * @file menu_temporario.c
 * @brief Interface de terminal para testar a partida sem Raylib.
 */
#include "../include/menu.h"
#include "../include/jogo.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Lê uma opção de um caractere, descartando entradas longas.
 * @return Opção em maiúscula, '?' para entrada inválida ou EOF ao encerrar.
 */
static int lerOpcao(void){
    char linha[64];
    if (fgets(linha, sizeof(linha), stdin) == NULL){
        return EOF;
    }

    // Descarta uma entrada longa para não reutilizá-la na próxima pergunta.
    if (strchr(linha, '\n') == NULL && !feof(stdin)){
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF){}
        return '?';
    }

    // Aceita espaços ao redor da opção, exigindo somente um caractere.
    size_t inicio = 0;
    while (isspace((unsigned char)linha[inicio])){
        inicio++;
    }
    if (linha[inicio] == '\0'){
        return '?';
    }
    int opcao = toupper((unsigned char)linha[inicio++]);
    while (isspace((unsigned char)linha[inicio])){
        inicio++;
    }
    return linha[inicio] == '\0' ? opcao : '?';
}

/** @brief Executa uma partida completa e retorna ao menu ao encerrar. */
static void jogarNoTerminal(Cadastro *cadastro){
    Partida partida;
    ResultadoInicio inicio = iniciarPartida(&partida, cadastro);
    if (inicio != INICIO_SUCESSO){
        if (inicio == INICIO_PERGUNTAS_INSUFICIENTES){
            puts("Não há perguntas suficientes de cada dificuldade para iniciar.");
        } else {
            puts("Não foi possível iniciar: cadastro inválido.");
        }
        return;
    }

    puts("Prêmios provisórios: marcos nas perguntas 5 e 10.");
    while (partida.estado == PARTIDA_EM_ANDAMENTO){
        const Pergunta *pergunta = obterPerguntaAtual(&partida);
        printf("\nPergunta %d de %d - %s\n", partida.indiceAtual + 1,
               TOTAL_PERGUNTAS_PARTIDA, obterNomeNivel(pergunta->nivel));
        printf("Prêmio atual: R$ %d | Garantido: R$ %d | Valendo: R$ %d\n",
               partida.premioAtual, partida.premioGarantido,
               obterPremioPergunta(partida.indiceAtual + 1));
        printf("%s\n", pergunta->enunciado);
        for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
            if (!partida.ajudas.eliminadas[i]){
                printf("%c) %s\n", 'A' + i, pergunta->alternativas[i]);
            }
        }
        // Mantém o conteúdo das ajudas visível até o fim da pergunta.
        for (int tipo = AJUDA_MISSAO; tipo <= AJUDA_TELESCOPIO; tipo++){
            if (partida.ajudas.ativas[tipo]){
                const char *texto = obterTextoAjuda(pergunta, (TipoAjuda)tipo);
                if (texto != NULL){
                    printf("%s: %s\n", tipo == AJUDA_MISSAO ? "Missão Espacial" : "Telescópio", texto);
                }
            }
        }
        if (partida.ajudas.sugestaoEspecialista != '\0'){
            printf("Especialista: sugiro %c (chance de acerto: %d%%).\n",
                   partida.ajudas.sugestaoEspecialista, obterChanceEspecialista(pergunta->nivel));
        }
        const char *nomes[TOTAL_AJUDAS] = {"Supernova 50/50", "Missão Espacial", "Telescópio", "Especialista"};
        for (int i = 0; i < TOTAL_AJUDAS; i++){
            printf("%d - %s [%s]\n", i + 1, nomes[i],
                   partida.ajudas.usadas[i] ? "usada" : "disponível");
        }
        puts("Responda A-D ou S para desistir:");
        fflush(stdout);
        int opcao = lerOpcao();
        if (opcao == 'S' || opcao == EOF){
            desistirPartida(&partida);
            break;
        }

        // As teclas 1-4 acionam ajudas sem responder ou avançar a pergunta.
        if (opcao >= '1' && opcao <= '4'){
            TipoAjuda tipo = (TipoAjuda)(opcao - '1');
            ResultadoAjuda resultado = usarAjuda(&partida, tipo);
            if (resultado == AJUDA_JA_USADA){
                puts("Esta ajuda já foi usada nesta partida.");
            } else if (resultado == AJUDA_SEM_CONTEUDO){
                puts("Esta pergunta não possui conteúdo para essa ajuda. Uso preservado.");
            } else if (resultado != AJUDA_SUCESSO){
                puts("Não foi possível usar a ajuda.");
            } else {
                printf("Ajuda utilizada: %s.\n", nomes[tipo]);
            }
            continue;
        }

        // Guarda o gabarito antes da resposta, pois um acerto avança a partida.
        char correta = pergunta->respostaCorreta;
        ResultadoResposta resultado = responderPergunta(&partida, (char)opcao);
        if (resultado == RESPOSTA_INVALIDA){
            puts("Entrada inválida. Use uma alternativa disponível, 1-4 para ajudas ou S.");
        } else if (resultado == RESPOSTA_INCORRETA){
            printf("Resposta incorreta. Alternativa correta: %c.\n", correta);
        } else {
            puts("Resposta correta!");
            if (partida.acertos == 5 || partida.acertos == 10){
                puts("Marco de segurança alcançado!");
            }
        }
    }

    // Mostra o resultado usando os valores já calculados pelo módulo de jogo.
    if (partida.estado == PARTIDA_VITORIA){
        puts("Vitória! Você completou as 15 perguntas!");
    } else if (partida.estado == PARTIDA_DERROTA){
        puts("Fim de jogo: derrota.");
    } else {
        puts("Partida encerrada por desistência.");
    }
    printf("Acertos: %d | Prêmio final: R$ %d\n", partida.acertos, partida.premioAtual);
}

void executarMenu(Cadastro *cadastro){
    for (;;){
        printf("\nProjeto Cosmos - terminal\n");
        printf("Perguntas carregadas: %zu\n1 - Iniciar partida\n0 - Salvar e sair\n",
               cadastro->quantidade);
        fflush(stdout);
        int opcao = lerOpcao();
        if (opcao == '0' || opcao == EOF){
            return;
        }
        if (opcao == '1'){
            jogarNoTerminal(cadastro);
        } else {
            puts("Opção inválida.");
        }
    }
}
