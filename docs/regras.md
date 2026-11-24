# Partida básica no terminal

Esta é uma adaptação provisória para o grupo validar, sem afirmar equivalência
às regras oficiais do SBT. A tabela está centralizada em `obterPremioPergunta`.

| Pergunta | Prêmio (R$) |
| --- | ---: |
| 1 | 1.000 |
| 2 | 2.000 |
| 3 | 3.000 |
| 4 | 4.000 |
| 5 | 5.000 |
| 6 | 10.000 |
| 7 | 20.000 |
| 8 | 30.000 |
| 9 | 40.000 |
| 10 | 50.000 |
| 11 | 100.000 |
| 12 | 200.000 |
| 13 | 300.000 |
| 14 | 500.000 |
| 15 | 1.000.000 |

- As perguntas seguem a distribuição 2 muito fáceis, 2 fáceis, 4 médias,
  4 difíceis e 3 muito difíceis, sem repetição de ID.
- Cada acerto substitui o prêmio atual pelo valor da pergunta e avança.
- Os marcos são conquistados ao acertar as perguntas 5 e 10.
- Um erro encerra a partida com o último valor garantido, ou zero antes do marco 5.
- Desistir preserva o prêmio atual; antes do primeiro acerto, o prêmio é zero.
- Acertar a pergunta 15 resulta em vitória e prêmio de um milhão.
- A-D, inclusive minúsculas, respondem; S desiste. Entradas inválidas não avançam.
- Fim da entrada no terminal encerra por desistência e permite salvar o cadastro.
- Após encerrar, nenhuma resposta adicional pode modificar a partida.

## Integração

O menu em `src/menu_temporario.c` é uma referência de integração para o
Integrante 2. As funções públicas estão em `include/jogo.h` e `include/ajuda.h`.

| Ação da interface | Função e resultado |
| --- | --- |
| Começar uma partida | Chame `iniciarPartida(&partida, cadastro)`. Com `INICIO_SUCESSO`, o estado é `PARTIDA_EM_ANDAMENTO`. Trate `INICIO_PERGUNTAS_INSUFICIENTES` e os erros de cadastro ou parâmetros antes de abrir a partida. |
| Exibir a rodada | Use `obterPerguntaAtual(&partida)`. Ela retorna `NULL` fora de uma partida em andamento. Mostre a rodada com `indiceAtual + 1` e o valor em disputa com `obterPremioPergunta(indiceAtual + 1)`. |
| Receber uma resposta | Chame `responderPergunta(&partida, letra)`. `RESPOSTA_CORRETA` já avança. `RESPOSTA_INCORRETA` encerra em derrota e `RESPOSTA_VITORIA` encerra após o 15º acerto. Trate entradas inválidas ou ações fora da partida sem avançar. Se for mostrar o gabarito, guarde-o antes da chamada. |
| Usar uma ajuda | Chame `usarAjuda(&partida, tipo)`. Após `AJUDA_SUCESSO`, consulte `partida.ajudas` para ocultar alternativas eliminadas e exibir os efeitos ativos. Use `obterTextoAjuda` para buscar o texto da Missão ou do Telescópio. Trate `AJUDA_JA_USADA` e `AJUDA_SEM_CONTEUDO` com mensagens próprias. |
| Desistir ou exibir o resultado | `desistirPartida(&partida)` retorna 1 e muda o estado para `PARTIDA_DESISTENCIA` se houver uma partida em andamento. Ao encerrar, consulte `partida.estado`, `partida.acertos` e `partida.premioAtual`. |

### Testes no terminal

Na raiz do projeto, compile e execute a versão de entrega no terminal:

```sh
COSMOS_RANKING=build/ranking-teste.tsv make run-terminal \
  ARGS="build/perguntas-teste.dat dados/perguntas.csv"
```

No Windows com MinGW, use `mingw32-make` se esse for o comando instalado e
configure `$env:COSMOS_RANKING` no PowerShell.

Para executar a interface gráfica, use `make run` com as opções de
vinculação da Raylib para seu sistema. Ambos os menus fornecem `executarMenu`,
e o ponto de chamada permanece em `main.c`.
`make test` continua disponível para os testes de jogo, ajudas e integração.

1. Escolha **1** no menu principal para iniciar. Na partida, use **1–4** para
   acionar as ajudas e repita uma tecla para conferir o limite de uso.
2. Envie uma alternativa disponível (**A–D**). Um acerto avança e um erro
   mostra o resultado final. A suíte automatizada cobre a vitória e os prêmios
   em cada ponto de derrota ou desistência.
3. Inicie outra partida, pressione **S** para desistir, informe um nome e
   selecione **3** no menu para consultar o ranking. Saia com **0** e abra
   de novo usando os mesmos caminhos para conferir a persistência.

### Escopo do ranking

O ranking registra vitórias, derrotas e desistências quando o jogador informa
um nome ao final. Pressionar Enter no campo permite pular o registro. Cada
resultado guarda nome, prêmio final, total de acertos e estado final. A
classificação exibe até 10 resultados, por maior prêmio e depois maior número
de acertos. Em caso de empate, vale a ordem de registro.

O histórico local fica em `dados/ranking.tsv`. A variável `COSMOS_RANKING`
permite escolher outro arquivo para o terminal. Para a futura interface gráfica,
`include/ranking.h` oferece `registrarResultadoRanking` e `consultarRanking`,
sem dependência de interface. Buraco Negro fica para uma etapa posterior.

## Ajudas

Cada uma das quatro ajudas pode ser usada uma vez por partida. É permitido
combinar ajudas na mesma pergunta. Elas não avançam a partida nem alteram prêmios.

| Tecla | Ajuda | Efeito |
| --- | --- | --- |
| 1 | Supernova | Elimina duas alternativas incorretas sem repetir; a correta permanece. |
| 2 | Missão Espacial | Apresenta uma pista relacionada à pergunta. |
| 3 | Telescópio | Apresenta uma informação de contexto distinta da pista. |
| 4 | Especialista | Sugere uma alternativa disponível, podendo errar. |

O especialista tem chances de acerto de 90%, 80%, 70%, 60% e 50%, da dificuldade
muito fácil à muito difícil. O 50/50 não altera essas chances; quando o especialista
erra, escolhe uma incorreta ainda disponível. Se a Supernova eliminar uma sugestão
anterior, essa sugestão deixa de ser exibida, sem devolver o uso do especialista.

Alternativas eliminadas não podem ser respondidas. Ao avançar, as alternativas
voltam a aparecer e os textos e sugestões são limpos; os usos continuam gastos.
Iniciar outra partida restaura todas as ajudas.

Os textos das 80 perguntas iniciais estão em `src/pistas.c`, associados ao ID e
ao enunciado original. Isso preserva a estrutura `Pergunta` e o formato binário.
Perguntas novas ou com enunciado alterado precisam de conteúdo próprio nesse módulo;
na ausência dele, Missão e Telescópio ficam indisponíveis sem gastar o uso.
Supernova e Especialista funcionam também para perguntas novas.
