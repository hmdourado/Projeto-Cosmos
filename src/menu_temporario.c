/**
 * @file menu_temporario.c
 * @brief Interface de terminal para jogar e gerenciar perguntas.
 */
#include "../include/arquivo.h"
#include "../include/menu.h"
#include "../include/jogo.h"
#include "../include/ranking.h"
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


/**
 * @brief Localiza a próxima pergunta de determinado nível.
 *
 * @param cadastro Cadastro de perguntas.
 * @param nivel Nível de dificuldade desejado.
 * @param inicio Índice inicial da busca.
 * @return Índice encontrado ou quantidade caso não encontre.
 */
static size_t buscarIndicePorNivel(
    const Cadastro *cadastro,
    NivelDificuldade nivel,
    size_t inicio
){
    for (size_t i = inicio; i < cadastro->quantidade; i++){
        if (cadastro->perguntas[i].nivel == nivel){
            return i;
        }
    }

    return cadastro->quantidade;
}

/**
 * @brief Localiza a pergunta anterior de determinado nível.
 *
 * @param cadastro Cadastro de perguntas.
 * @param nivel Nível de dificuldade desejado.
 * @param inicio Índice atual da pergunta.
 * @return Índice encontrado ou quantidade caso não encontre.
 */
static size_t buscarIndiceAnteriorPorNivel(
    const Cadastro *cadastro,
    NivelDificuldade nivel,
    size_t inicio
){
    while (inicio > 0){
        inicio--;

        if (cadastro->perguntas[inicio].nivel == nivel){
            return inicio;
        }
    }

    return cadastro->quantidade;
}

/**
 * @brief Permite visualizar e navegar pelas perguntas de um nível.
 */
static void visualizarPorDificuldade(const Cadastro *cadastro){

    puts("\nVisualizar perguntas por dificuldade");

    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++){
        printf("%d - %s\n", nivel, obterNomeNivel(nivel));
    }

    puts("0 - Voltar");

    int opcao = lerOpcao();

    if (opcao == '0' || opcao == EOF){
        return;
    }

    if (opcao < '1' || opcao > '5'){
        puts("Nível inválido.");
        return;
    }

    NivelDificuldade nivel = (NivelDificuldade)(opcao - '0');

    // Localiza a primeira pergunta do nível escolhido.
    size_t indice = buscarIndicePorNivel(cadastro, nivel, 0);

    if (indice == cadastro->quantidade){
        puts("Nenhuma pergunta encontrada nesse nível.");
        return;
    }

    // Permite navegar pelas perguntas encontradas.
    for (;;){

        Pergunta *pergunta = &cadastro->perguntas[indice];

        printf("\nID: %d\n", pergunta->id);
        printf("Dificuldade: %s\n", obterNomeNivel(pergunta->nivel));

        exibirPergunta(pergunta);

        printf("Resposta correta: %c\n", pergunta->respostaCorreta);

        puts("\nN - Próxima pergunta");
        puts("A - Pergunta anterior");
        puts("0 - Voltar");

        opcao = lerOpcao();

        if (opcao == '0' || opcao == EOF){
            return;
        }

        if (opcao == 'N'){

            size_t proximo = buscarIndicePorNivel(
                cadastro, nivel, indice + 1
            );

            if (proximo == cadastro->quantidade){
                puts("Não existem mais perguntas nesse nível.");
            } else {
                indice = proximo;
            }

        } else if (opcao == 'A'){

            size_t anterior = buscarIndiceAnteriorPorNivel(
                cadastro, nivel, indice
            );

            if (anterior == cadastro->quantidade){
                puts("Esta é a primeira pergunta desse nível.");
            } else {
                indice = anterior;
            }

        } else {
            puts("Opção inválida.");
        }
    }
}


/**
 * @brief Exporta o cadastro atual para um arquivo CSV.
 *
 * @param cadastro Cadastro que será exportado.
 */
static void exportarCadastroCSV(const Cadastro *cadastro){

    char caminho[260];

    printf("Caminho do arquivo CSV de destino: ");

    if (!lerTexto(caminho, sizeof(caminho))){
        puts("Exportação cancelada.");
        return;
    }

    if (exportarCSV(cadastro, caminho)){
        printf("Cadastro exportado com sucesso para: %s\n", caminho);
    } else {
        puts("Não foi possível exportar o cadastro.");
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
        "1 - Inserir\n"
        "2 - Listar\n"
        "3 - Pesquisar por ID\n"
        "4 - Alterar por ID\n"
        "5 - Excluir por ID\n"
        "6 - Visualizar por dificuldade\n"
        "7 - Exportar cadastro para CSV\n"
        "0 - Voltar");

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

            case '6':
                visualizarPorDificuldade(cadastro);
                break;

            case '7':
                exportarCadastroCSV(cadastro);
                break;
                
            default:
                puts("Opção inválida.");
        }
    }
}

static void mostrarRanking(const char *caminho){
    ResultadoRanking resultados[RANKING_LIMITE];
    int quantidade = consultarRanking(caminho, resultados);
    if (quantidade < 0){
        puts("Não foi possível consultar o ranking.");
        return;
    }
    puts("\nRanking - 10 melhores resultados");
    if (quantidade == 0){
        puts("Nenhum resultado registrado.");
    }
    for (int i = 0; i < quantidade; i++){
        const ResultadoRanking *resultado = &resultados[i];
        printf("%d. %s | R$ %d | %d acertos | %s\n", i + 1,
               resultado->nome, resultado->premio, resultado->acertos,
               descreverResultadoRanking(resultado->estado));
    }
}

static void salvarResultadoNoTerminal(const char *caminho, const Partida *partida){
    char nome[RANKING_TAM_NOME];
    for (;;){
        puts("Nome para o ranking (Enter para pular):");
        int leitura = lerLinha(nome, sizeof(nome));
        if (leitura == EOF || (leitura == 1 && nome[0] == '\0')){
            return;
        }
        if (leitura == 1 && nomeRankingValido(nome)){
            break;
        }
        puts("Informe um nome válido de até 40 bytes, sem tabulações.");
    }
    if (registrarResultadoRanking(caminho, nome, partida)){
        puts("Resultado registrado no ranking.");
    } else {
        puts("Não foi possível gravar o resultado no ranking.");
    }
}

/** @brief Executa uma partida completa e retorna ao menu ao encerrar. */
static void jogarNoTerminal(Cadastro *cadastro, const char *ranking){
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
    puts("\nVocê tem estas ajudas disponíveis. Cada uma pode ser usada uma vez por partida:");
    puts("1 - Supernova (50/50): elimina duas alternativas incorretas.");
    puts("2 - Missão Espacial: revela uma pista sobre a pergunta.");
    puts("3 - Telescópio: mostra uma informação de contexto diferente da pista.");
    puts("4 - Especialista: sugere uma alternativa e pode errar.");
    puts("Quando a pergunta não tiver pista ou contexto, o uso da ajuda é preservado.");
    while (partida.estado == PARTIDA_EM_ANDAMENTO){
        const Pergunta *pergunta = obterPerguntaAtual(&partida);
        printf("\nPergunta %d de %d - %s\n\n", partida.indiceAtual + 1,
               TOTAL_PERGUNTAS_PARTIDA, obterNomeNivel(pergunta->nivel));
        printf("Prêmio atual: R$ %d | Garantido: R$ %d | Valendo: R$ %d\n\n",
               partida.premioAtual, partida.premioGarantido,
               obterPremioPergunta(partida.indiceAtual + 1));
        printf("%s\n\n", pergunta->enunciado);
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
        int temAjudaDisponivel = 0;
        for (int i = 0; i < TOTAL_AJUDAS; i++){
            if (partida.ajudas.usadas[i] ||
                ((i == AJUDA_MISSAO || i == AJUDA_TELESCOPIO) &&
                 obterTextoAjuda(pergunta, (TipoAjuda)i) == NULL)){
                continue;
            }
            if (!temAjudaDisponivel){
                puts("\nVocê possui algumas opções de ajudas:");
                temAjudaDisponivel = 1;
            }
            printf("%d - %s [disponível]\n", i + 1, nomes[i]);
        }
        if (!temAjudaDisponivel){
            puts("\nVocê já utilizou todas as ajudas disponíveis.");
        }
        puts(temAjudaDisponivel
             ? "\nResponda A-D, use 1-4 para pedir ajuda ou S para desistir:"
             : "\nResponda A-D ou S para desistir:");
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
    if (!feof(stdin) && !ferror(stdin)){
        salvarResultadoNoTerminal(ranking, &partida);
    }
}

void executarMenu(Cadastro *cadastro){
    const char *ranking = getenv("COSMOS_RANKING");
    if (ranking == NULL || *ranking == '\0'){
        ranking = RANKING_ARQUIVO;
    }
    for (;;){
        printf("\nProjeto Cosmos - terminal\n");
        printf("Perguntas carregadas: %zu\n1 - Iniciar partida\n"
               "2 - Gerenciar perguntas\n3 - Ver ranking\n0 - Salvar e sair\n",
               cadastro->quantidade);
        fflush(stdout);
        int opcao = lerOpcao();
        if (opcao == '0' || opcao == EOF){
            return;
        }
        if (opcao == '1'){
            jogarNoTerminal(cadastro, ranking);
        } else if (opcao == '2'){
            gerenciarPerguntas(cadastro);
        } else if (opcao == '3'){
            mostrarRanking(ranking);
        } else {
            puts("Opção inválida.");
        }
    }
}
