/**
 * @file menu_temporario.c
 * @brief Interface de terminal para jogar e gerenciar perguntas.
 */
#include "../include/menu.h"
#include "../include/jogo.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int lerLinha(char *linha, size_t tamanho){
    fflush(stdout);
    if (fgets(linha, tamanho, stdin) == NULL){
        return EOF;
    }
    char *quebra = strchr(linha, '\n');
    if (quebra != NULL){
        *quebra = '\0';
        return 1;
    }

    int caractere = getchar();
    if (caractere == '\n' || caractere == EOF){
        return 1;
    }
    while ((caractere = getchar()) != '\n' && caractere != EOF){}
    return 0;
}

/**
 * @brief Lê uma opção de um caractere, descartando entradas longas.
 * @return Opção em maiúscula, '?' para entrada inválida ou EOF ao encerrar.
 */
static int lerOpcao(void){
    char linha[64];
    int leitura = lerLinha(linha, sizeof(linha));
    if (leitura == EOF){
        return EOF;
    }
    if (leitura == 0){
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

static int lerTexto(char *texto, size_t tamanho){
    for (;;){
        int leitura = lerLinha(texto, tamanho);
        if (leitura == EOF){
            return 0;
        }
        if (leitura && texto[strspn(texto, " \t\r\v\f")] != '\0'){
            return 1;
        }
        printf("Informe um texto com até %zu bytes: ", tamanho - 1);
    }
}

static int lerPergunta(Pergunta *pergunta){
    inicializarPergunta(pergunta);
    printf("Enunciado: ");
    if (!lerTexto(pergunta->enunciado, sizeof(pergunta->enunciado))){
        return 0;
    }
    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
        printf("Alternativa %c: ", obterNomeAlternativa(i));
        if (!lerTexto(pergunta->alternativas[i], sizeof(pergunta->alternativas[i]))){
            return 0;
        }
    }

    int opcao;
    do {
        printf("Alternativa correta (A-D): ");
        opcao = lerOpcao();
        if (opcao == EOF){
            return 0;
        }
    } while (opcao < 'A' || opcao > 'D');
    pergunta->respostaCorreta = (char)opcao;

    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        printf("%d - %s\n", nivel, obterNomeNivel(nivel));
    }
    do {
        printf("Dificuldade (1-5): ");
        opcao = lerOpcao();
        if (opcao == EOF){
            return 0;
        }
    } while (opcao < '1' || opcao > '5');
    pergunta->nivel = opcao - '0';
    return validarPergunta(pergunta);
}

static void inserirNoCadastro(Cadastro *cadastro){
    int maiorId = 0;
    for (size_t i = 0; i < cadastro->quantidade; i++){
        if (cadastro->perguntas[i].id > maiorId){
            maiorId = cadastro->perguntas[i].id;
        }
    }
    if (maiorId == INT_MAX){
        puts("Não foi possível gerar um novo ID.");
        return;
    }
    Pergunta pergunta;
    if (lerPergunta(&pergunta)){
        pergunta.id = maiorId + 1;
        if (adicionarPergunta(cadastro, &pergunta)){
            printf("Pergunta cadastrada com ID %d.\n", pergunta.id);
        } else {
            puts("Não foi possível cadastrar a pergunta.");
        }
    }
}

static void listarCadastro(const Cadastro *cadastro){
    if (cadastro->quantidade == 0){
        puts("Nenhuma pergunta cadastrada.");
    }
    for (size_t i = 0; i < cadastro->quantidade; i++){
        const Pergunta *pergunta = &cadastro->perguntas[i];
        printf("ID %d | %s | %s\n", pergunta->id,
               obterNomeNivel(pergunta->nivel), pergunta->enunciado);
    }
}

static Pergunta *localizarNoCadastro(Cadastro *cadastro){
    char linha[64];
    printf("ID da pergunta: ");
    int leitura = lerLinha(linha, sizeof(linha));
    if (leitura == EOF){
        return NULL;
    }
    char *fim;
    errno = 0;
    long id = strtol(linha, &fim, 10);
    if (!leitura || fim == linha || errno == ERANGE || id <= 0 ||
        id > INT_MAX || fim[strspn(fim, " \t\r\v\f")] != '\0'){
        puts("Informe um ID inteiro positivo válido.");
        return NULL;
    }
    Pergunta *pergunta = buscarPerguntaPorId(cadastro, (int)id);
    if (pergunta == NULL){
        puts("Pergunta não encontrada.");
    }
    return pergunta;
}

static void gerenciarPerguntas(Cadastro *cadastro){
    for (;;){
        puts("\nCadastro de perguntas\n"
             "1 - Inserir\n2 - Listar\n3 - Pesquisar por ID\n"
             "4 - Alterar por ID\n5 - Excluir por ID\n0 - Voltar");
        int opcao = lerOpcao();
        switch (opcao){
            case '0':
            case EOF:
                return;
            case '1':
                inserirNoCadastro(cadastro);
                break;
            case '2':
                listarCadastro(cadastro);
                break;
            case '3':
            case '4':
            case '5': {
                Pergunta *pergunta = localizarNoCadastro(cadastro);
                if (pergunta == NULL){
                    break;
                }
                int id = pergunta->id;
                if (opcao == '5'){
                    if (removerPergunta(cadastro, id)){
                        puts("Pergunta excluída.");
                    }
                    break;
                }
                printf("ID %d\n", id);
                exibirPergunta(pergunta);
                printf("Alternativa correta: %c\n", pergunta->respostaCorreta);
                if (opcao == '4'){
                    Pergunta atualizada;
                    if (lerPergunta(&atualizada) &&
                        atualizarPergunta(cadastro, id, &atualizada)){
                        puts("Pergunta alterada.");
                    }
                }
                break;
            }
            default:
                puts("Opção inválida.");
        }
    }
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
            printf("%c) %s\n", 'A' + i, pergunta->alternativas[i]);
        }
        puts("Responda A-D ou S para desistir:");
        fflush(stdout);
        int opcao = lerOpcao();
        if (opcao == 'S' || opcao == EOF){
            desistirPartida(&partida);
            break;
        }

        // Guarda o gabarito antes da resposta, pois um acerto avança a partida.
        char correta = pergunta->respostaCorreta;
        ResultadoResposta resultado = responderPergunta(&partida, (char)opcao);
        if (resultado == RESPOSTA_INVALIDA){
            puts("Entrada inválida. Use A, B, C, D ou S.");
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
        printf("Perguntas carregadas: %zu\n1 - Iniciar partida\n"
               "2 - Gerenciar perguntas\n0 - Salvar e sair\n",
               cadastro->quantidade);
        fflush(stdout);
        int opcao = lerOpcao();
        if (opcao == '0' || opcao == EOF){
            return;
        }
        if (opcao == '1'){
            jogarNoTerminal(cadastro);
        } else if (opcao == '2'){
            gerenciarPerguntas(cadastro);
        } else {
            puts("Opção inválida.");
        }
    }
}
