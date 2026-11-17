/**
 * @file menu.c
 * @author Arthur Morais Palitot Fernandes (arthurpalitot@gmail.com | arthurmoraisfernandes@alunos.utfpr.edu.br)
 * @brief Interface textual simples para testar o cadastro de perguntas.
 * @version 0.1
 * @date 16-11-2026
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "../include/menu.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>


// Lê uma linha do teclado e remove o '\n'.
static int lerLinha(char *texto, int tamanho){
    if (fgets(texto, tamanho, stdin) == NULL){
        return 0;
    }

    // Se a linha não coube no vetor, descarta o resto dela.
    if (strchr(texto, '\n') == NULL){
        int c;
        while ((c = getchar()) != '\n' && c != EOF){
        }
    }

    texto[strcspn(texto, "\r\n")] = '\0';
    return 1;
}

// 


/**
 * @brief Lê um número inteiro.
 * 
 * @param mensagem 
 * @return int -1 se não for um número e 0 se a entrada acabar
 */
static int lerInteiro(const char *mensagem){
    char linha[32];
    int numero;

    printf("%s", mensagem);

    if (!lerLinha(linha, sizeof(linha))){
        return 0;
    }

    if (sscanf(linha, "%d", &numero) != 1){
        return -1;
    }

    return numero;
}

static void exibirNiveis(void){
    for (int n = MUITO_FACIL; n <= MUITO_DIFICIL; n++){
        printf("  %d - %s\n", n, obterNomeNivel((NivelDificuldade)n));
    }
}

// Pede os dados de uma pergunta. Retorna 1 se a pergunta ficou válida.
static int lerPerguntaUsuario(Pergunta *p){
    char resposta[8];

    inicializarPergunta(p);

    printf("Enunciado: ");
    if (!lerLinha(p->enunciado, TAM_ENUNCIADO)){
        return 0;
    }

    for (int i = 0; i < QUANT_ALTERNATIVAS; i++){
        printf("Alternativa %c: ", obterNomeAlternativa(i));
        if (!lerLinha(p->alternativas[i], TAM_ALTERNATIVA)){
            return 0;
        }
    }

    printf("Resposta correta (A, B, C ou D): ");
    if (!lerLinha(resposta, sizeof(resposta))){
        return 0;
    }
    p->respostaCorreta = (char)toupper((unsigned char)resposta[0]);

    exibirNiveis();
    p->nivel = (NivelDificuldade)lerInteiro("Nível (1 a 5): ");

    // A validação já existe em pergunta.c.
    if (!validarPergunta(p)){
        printf("Dados inválidos. A pergunta não foi salva.\n");
        return 0;
    }

    return 1;
}

static void mostrarPergunta(Pergunta *p){
    printf("\nID: %d", p->id);
    exibirPergunta(p);
    printf("Resposta correta: %c\n", p->respostaCorreta);
}

static void exibirMenuPrincipal(void){
    printf("\n===== MENU PRINCIPAL =====\n");
    printf("1 - Gerenciar perguntas\n");
    printf("0 - Sair\n");
}

static void exibirMenuCadastro(void){
    printf("\n===== PERGUNTAS =====\n");
    printf("1 - Cadastrar\n");
    printf("2 - Listar todas\n");
    printf("3 - Pesquisar por ID\n");
    printf("4 - Listar por dificuldade\n");
    printf("5 - Alterar\n");
    printf("6 - Excluir\n");
    printf("0 - Voltar\n");
}

static void cadastrarPerguntaMenu(Cadastro *cadastro){
    Pergunta nova;

    if (!lerPerguntaUsuario(&nova)){
        return;
    }

    // O ID é o maior ID existente + 1.
    nova.id = 1;
    for (size_t i = 0; i < cadastro->quantidade; i++){
        if (cadastro->perguntas[i].id >= nova.id){
            nova.id = cadastro->perguntas[i].id + 1;
        }
    }

    if (adicionarPergunta(cadastro, &nova)){
        printf("Pergunta cadastrada! ID: %d\n", nova.id);
    } else {
        printf("Erro ao cadastrar a pergunta.\n");
    }
}

static void listarPerguntasMenu(Cadastro *cadastro){
    if (cadastro->quantidade == 0){
        printf("Nenhuma pergunta cadastrada.\n");
        return;
    }

    for (size_t i = 0; i < cadastro->quantidade; i++){
        mostrarPergunta(&cadastro->perguntas[i]);
    }
}

static void pesquisarPerguntaMenu(Cadastro *cadastro){
    Pergunta *p = buscarPerguntaPorId(cadastro, lerInteiro("ID da pergunta: "));

    if (p == NULL){
        printf("Pergunta não encontrada.\n");
    } else {
        mostrarPergunta(p);
    }
}

static void listarPorNivelMenu(Cadastro *cadastro){
    int achou = 0;

    exibirNiveis();
    int nivel = lerInteiro("Nível (1 a 5): ");

    for (size_t i = 0; i < cadastro->quantidade; i++){
        if ((int)cadastro->perguntas[i].nivel == nivel){
            mostrarPergunta(&cadastro->perguntas[i]);
            achou = 1;
        }
    }

    if (!achou){
        printf("Nenhuma pergunta encontrada nesse nível.\n");
    }
}

static void alterarPerguntaMenu(Cadastro *cadastro){
    Pergunta nova;
    int id = lerInteiro("ID da pergunta: ");

    if (buscarPerguntaPorId(cadastro, id) == NULL){
        printf("Pergunta não encontrada.\n");
        return;
    }

    if (!lerPerguntaUsuario(&nova)){
        return;
    }

    if (atualizarPergunta(cadastro, id, &nova)){
        printf("Pergunta atualizada!\n");
    } else {
        printf("Erro ao atualizar a pergunta.\n");
    }
}

static void excluirPerguntaMenu(Cadastro *cadastro){
    if (removerPergunta(cadastro, lerInteiro("ID da pergunta: "))){
        printf("Pergunta removida!\n");
    } else {
        printf("Pergunta não encontrada.\n");
    }
}

static void menuPerguntas(Cadastro *cadastro){
    int opcao;

    do {
        exibirMenuCadastro();
        opcao = lerInteiro("Opção: ");

        switch (opcao){
            case 1: cadastrarPerguntaMenu(cadastro); break;
            case 2: listarPerguntasMenu(cadastro);   break;
            case 3: pesquisarPerguntaMenu(cadastro); break;
            case 4: listarPorNivelMenu(cadastro);    break;
            case 5: alterarPerguntaMenu(cadastro);   break;
            case 6: excluirPerguntaMenu(cadastro);   break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}

void executarMenu(Cadastro *cadastro){
    int opcao;

    if (cadastro == NULL){
        return;
    }

    do {
        exibirMenuPrincipal();
        opcao = lerInteiro("Opção: ");

        if (opcao == 1){
            menuPerguntas(cadastro);
        } else if (opcao != 0){
            printf("Opção inválida.\n");
        }
    } while (opcao != 0);
}
