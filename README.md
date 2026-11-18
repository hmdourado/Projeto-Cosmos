# Projeto-Cosmos

Nesta atividade os alunos deverão desenvolver um sistema que permita a criação e
manipulação de baralhos no estilo “Jogo do Milhão”. O sistema deve ser desenvolvido utilizando
a linguagem C e os conceitos visto em sala de aula. A atividade deverá ser desenvolvida em
trios impreterivelmente.
O sistema desenvolvido deverá obrigatoriamente contemplar, mas não estar restrito, aos
seguintes aspectos:

◦ Cadastro:
▪ Escolha uma temática para o seu jogo. Exemplos: Futebol, Games, Séries, Animes,
Cultura Pop/Geek, História da Computação, Linguagens de Programação, Memes,
Marvel, Puzzles(lógica e raciocínio).
▪ Para gerenciar uma coleção de perguntas para o jogo, defina uma estrutura
Pergunta contendo, no mínimo: enunciado (texto), 4 alternativas (texto), letra da
alternativa correta(char), nível de dificuldade. O sistema deverá permitir cadastrar
(inserir/ listar/ pesquisar/ alterar/ excluir) as perguntas disponíveis. Essa relação
deve aumentar e diminuir dinamicamente.
▪ O jogo será dividido em 15 perguntas organizadas em 5 níveis de dificuldade: muito
fácil, fácil, médio, difícil e muito difícil. As duas primeiras questões serão muito
fáceis, seguidas por duas fáceis (níveis 3 e 4). As perguntas de nível médio vão do 5
ao 8, sendo o nível 5 o primeiro marco de segurança. Do 9 ao 12, as perguntas
serão difíceis, com o nível 10 como segundo marco. Por fim, as três últimas
perguntas (13 a 15) terão dificuldade muito difícil, incluindo a questão final de um
milhão.
▪ Deverá permitir pesquisar e visualizar as perguntas por nível de dificuldade,
navegando entre elas. Para isso utilize cores, sons e sua criatividade.
▪ Deverá apresentar um menu inicial com as opções disponíveis. Caso necessário,
submenus. A interface deverá ser fácil e intuitiva, seja criativo, utilize cores e
beeps :) . Trate erros do usuário com mensagens e alertas.


◦ Arquivos
▪ Na primeira execução, a lista com os dados para as Figurinhas iniciais existentes
será construída lendo os dados de um arquivo de texto (.CSV - valores separados
por vírgula) que será criado pela equipe e carregado em um vetor de tamanho
dinâmico. Ao sair do programa, todos os dados, com eventuais alterações, devem
ser salvos em um arquivo binário no HD;
▪ Nas demais execuções, ao entrar e sair do programa, todos os dados devem ser
lidos e salvos nesse arquivo binário no HD;
▪ A lista com os dados para as perguntas iniciais será construída lendo os dados
de um arquivo .csv (valores separados por vírgula) construído pela equipe. Esse
arquivo deve ter no mínimo a seguinte quantidade de perguntas: 20 muito fácil,
20 fácil, 15 médio, 15 difícil e 10 muito difícil.
▪ O sistema deverá permitir exportar os dados atuais em um arquivo de texto no
formato .CSV (separado por vírgula).
Observações sobre o código:
◦ Os códigos referentes a cada estrutura devem estar em um arquivo fonte (.c) separado
com seu respectivo arquivo de cabeçalho(.h). Utilize outros arquivos para organizar seu
código conforme necessidade;
◦ O programa deverá acompanhar um arquivo makefile que permita as operações básicas
de make (compilação), clean (limpeza) e run (execução);
◦ Como parte do critério de avaliação, a qualidade do código-fonte do sistema será
avaliada. Organize seu programa em arquivos .c e .h, funções. Evite o uso de variáveis
globais. O código deverá estar comentado, edentado e com bons nomes de variáveis e
funções. Siga o padrão de documentação apresentado na disciplina (Doxygen).

• Parte Criativa (40% da nota)
◦ Partida: Adicione uma funcionalidade que permita jogar uma partida de Jogo do Milhão
contra o computador utilize a regra oficial do SBT como base.
◦ Adicione novas regras e funcionalidades ao jogo. Exemplo: diferentes esquemas de
ajuda (podem ser baseadas no tema do jogo), interface gráfica(raylib), ranking músicas e
sons e etc. Use sua criatividade!
◦ Obs: Para o desenvolvimento da parte criativa, você pode adicionar mais campos ao
arquivo .csv com os dados das questões.
• Entrega via Github
◦ Cada equipe deverá criar um repositório no Github, compartilhado entre os membros e
com o professor. O link do repositório deverá ser enviado ao professor até dia a data
estabelecida na tabela abaixo juntamente com os dados iniciais.
Avaliação: Implementação + Apresentação
Além do envio do código fonte, os(as) alunos(as) deverão realizar uma apresentação projeto
que terá duração de 10 e 15 minutos. Durante a apresentação, deverão descrever a abordagem
utilizada e apresentar uma visão geral da implementação. Em seguida, deverão mostrar o
sistema em funcionamento. Após a apresentação, serão arguidos individualmente pelo professor
e pelos demais alunos da turma em relação ao desenvolvimento do projeto. A apresentação
será avaliada de acordo com os seguintes critérios:
● Respeito ao tempo estipulado para a apresentação;
● Criatividade na apresentação e no desenvolvimento do sistema;
● Coerência do projeto desenvolvido e o que foi apresentado;
● Domínio do projeto verificado por meio da explicação do código fonte;