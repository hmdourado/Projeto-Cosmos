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
Ajudas, Buraco Negro e ranking serão adicionados em etapas posteriores.
