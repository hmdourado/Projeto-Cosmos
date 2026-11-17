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

`obterPerguntaAtual` retorna NULL fora de uma partida em andamento.
`responderPergunta` retorna o resultado da ação e avança automaticamente após
acerto, exceto na última pergunta, que encerra com vitória. A interface deve
guardar as informações necessárias ao feedback antes de chamar a função.
`premioAtual` representa o prêmio final após qualquer encerramento.

Execute `make run`, escolha 1 para jogar e 0 para salvar e sair do programa.
Execute `make test` para verificar a lógica e a integração de arquivos.
Buraco Negro e ranking serão adicionados em etapas posteriores.

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
