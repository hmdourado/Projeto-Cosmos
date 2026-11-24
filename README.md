# Jogo do Milhão Cósmico

Jogo de perguntas e respostas com temática de Astronomia, desenvolvido em linguagem C para a disciplina **Fundamentos de Programação 2 (FDCO2B)**, do curso de Engenharia de Computação da **Universidade Tecnológica Federal do Paraná (UTFPR) — Campus Apucarana**.

> **Status:** em desenvolvimento. A camada de interface gráfica com Raylib está em integração com os módulos de dados e regras da partida.

## Sobre o projeto

O sistema implementa a estrutura de um jogo no estilo *Jogo do Milhão*, permitindo gerenciar um banco de perguntas, iniciar partidas e persistir os dados entre execuções. O projeto é organizado em módulos C, com separação entre cadastro, persistência, lógica do jogo e interface.

### Regras principais

- Cada partida contém **15 perguntas**, distribuídas em cinco níveis: 2 muito fáceis, 2 fáceis, 4 médias, 4 difíceis e 3 muito difíceis.
- As perguntas 5 e 10 representam os marcos de segurança.
- O banco inicial deve conter pelo menos **80 perguntas**: 20 muito fáceis, 20 fáceis, 15 médias, 15 difíceis e 10 muito difíceis.

### Funcionalidades previstas no escopo

- Cadastro de perguntas: inserir, listar, pesquisar, alterar e excluir.
- Armazenamento dinâmico da coleção de perguntas.
- Consulta e navegação de perguntas por nível de dificuldade.
- Importação inicial de perguntas por CSV, persistência em arquivo binário e exportação dos dados atuais para CSV.
- Partida contra o computador, com progressão, respostas, pontuação, desistência e marcos de segurança.
- Interface gráfica com Raylib e recursos criativos temáticos.

## Tecnologias

- **C** (padrão C11)
- **Raylib** para a interface gráfica
- **GCC MinGW-w64 64 bits** no Windows
- **GNU Make** para automatizar a compilação
- **Git** para controle de versão

## Estrutura do projeto

```text
Projeto-Cosmos/
├── include/          # Cabeçalhos (.h)
├── src/              # Implementação dos módulos (.c)
│   └── telas/        # Implementação das telas da interface
├── lib/              # Bibliotecas utilizadas pelo projeto
├── build/            # Executáveis e artefatos de compilação
├── main.c            # Ponto de entrada
├── Makefile          # Instruções de compilação e execução
└── README.md
```

A estrutura pode ser ampliada conforme a implementação dos módulos e das funcionalidades criativas.

## Como compilar e executar

### Versão de terminal

Na raiz do projeto, com um compilador C11 e Make:

```sh
make run-terminal
make test
```

`make run-terminal` compila e abre o jogo no próprio terminal. `make test` executa os testes sem abrir uma janela. No Windows com MinGW, use `mingw32-make run-terminal CC=gcc` e `mingw32-make test CC=gcc` se essa for a instalação disponível.

Para fazer a demonstração com arquivos separados dos dados padrão, use:

```sh
COSMOS_RANKING=build/ranking-demo.tsv make run-terminal \
  ARGS="build/perguntas-demo.dat dados/perguntas.csv"
```

No PowerShell, configure primeiro `$env:COSMOS_RANKING = 'build/ranking-demo.tsv'` e passe os mesmos caminhos em `ARGS`.

| Comando | Resultado |
| --- | --- |
| `make run-terminal` | Compila e executa a versão de texto, sem vincular a Raylib. |
| `make test` | Executa as suítes de jogo, ajudas e integração de arquivos. |

A versão gráfica requer uma Raylib compatível com o sistema. O Makefile usa uma biblioteca para Windows ou opções de ligação de Linux. No macOS, a configuração padrão pode falhar com `library 'GL' not found`. O alvo de terminal funciona sem essas bibliotecas gráficas.

### Windows

**Pré-requisitos**

1. Instalar o [w64devkit x64 2.0.0](https://github.com/skeeto/w64devkit/releases/download/v2.0.0/w64devkit-x64-2.0.0.exe), conforme o guia de instalação da disciplina.
2. Adicionar o diretório `bin` do w64devkit ao `PATH`. Exemplo, caso instalado em `C:\w64devkit`:

   ```text
   C:\w64devkit\bin
   ```

3. Disponibilizar a biblioteca Raylib para Windows no caminho configurado pelo `Makefile` (na configuração atual, `lib/libraylib_win64.a`).

**Verificar as ferramentas** em um novo PowerShell:

```powershell
where.exe gcc
gcc --version
gcc -dumpmachine
where.exe mingw32-make
```

O compilador deve ser MinGW-w64 de 64 bits (`x86_64-w64-mingw32`).

**Compilar e executar**, a partir da raiz do repositório:

```powershell
# Limpar arquivos gerados
mingw32-make clean

# Compilar
mingw32-make

# Executar
mingw32-make run
```

O comando `mingw32-make run CC=gcc` também pode ser utilizado quando o alvo `run` do `Makefile` estiver configurado para executar o binário com a sintaxe do Windows.

### Linux (Debian, Ubuntu e derivados)

**Pré-requisitos**

```bash
sudo apt-get update
sudo apt-get install build-essential make git \
  libasound2-dev libx11-dev libxrandr-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev \
  libxinerama-dev libwayland-dev libxkbcommon-dev
```

Também é necessário ter uma versão da Raylib compilada/instalada para Linux e configurar o `Makefile` para utilizar essa biblioteca e as dependências de ligação do Linux.

**Compilar e executar**, depois de configurar a biblioteca e o alvo Linux no `Makefile`:

```bash
# Limpar arquivos gerados
make clean

# Compilar
make

# Executar
make run
```

> **Nota sobre suporte multiplataforma:** a configuração de ligação atualmente registrada no `Makefile` utiliza a biblioteca `lib/libraylib_win64.a` e bibliotecas de sistema do Windows. Esse arquivo não pode ser usado em Linux. Portanto, a compilação Linux depende de uma configuração específica para Raylib/Linux no `Makefile`; ainda não está validada nesta versão do projeto.

## Dados e persistência

O banco inicial de perguntas é fornecido em CSV. O sistema também prevê salvar os dados atualizados em arquivo binário ao encerrar e recuperá-los nas execuções seguintes, além de permitir exportar o cadastro atual para CSV.

## Desenvolvimento e qualidade

O código é dividido em arquivos `.c` e `.h`, com responsabilidades separadas por módulo. A implementação deve manter nomes descritivos, indentação consistente, tratamento de erros, gerenciamento correto de memória e documentação das funções no padrão **Doxygen**, conforme as orientações da disciplina.

## Como jogar

- No menu principal, use **1** para jogar, **2** para gerenciar perguntas, **3** para consultar o ranking e **0** para salvar e sair.
- Na partida, responda com **A–D**, acione as ajudas com **1–4** ou pressione **S** para desistir. Cada ajuda pode ser usada uma vez por partida.
- Ao final, informe um nome para registrar o resultado ou pressione Enter para pular. O ranking mostra até dez resultados, ordenados pelo prêmio final e pelo número de acertos. Empates mantêm a ordem de registro.

Acertar a quinta pergunta garante **R$ 5.000**. Acertar a décima garante **R$ 50.000**. Uma resposta errada encerra a partida com o último valor garantido. Desistir preserva o prêmio atual. O 15º acerto encerra a partida com **R$ 1.000.000**. A tabela completa, os efeitos das ajudas e a API para a interface estão em [Regras do jogo e integração](docs/regras.md).

## Arquitetura e conceitos de programação

`main.c` coordena o fluxo abaixo. O Makefile seleciona qual implementação de `executarMenu` será vinculada ao executável.

```text
Inicializar Cadastro → tentar carregar o binário → importar CSV se necessário
                              ↓
           executarMenu → cadastro / partida / ranking
                              ↓
                      salvar binário → liberar memória
```

| Módulo | Responsabilidade | Conceitos aplicados |
| --- | --- | --- |
| [Pergunta](include/pergunta.h) | Representa e valida enunciado, quatro alternativas, dificuldade e gabarito. | `struct`, `enum`, arrays de caracteres e validação. |
| [Cadastro](include/cadastro.h) | Controla as perguntas e permite inserir, buscar, alterar e remover. | Ponteiros, vetor dinâmico, `realloc` e `free`. |
| [Arquivo](include/arquivo.h) | Importa e exporta CSV, salva e carrega o binário. | `FILE *`, E/S de texto e binária, tratamento de erros. |
| [Jogo](include/jogo.h) | Sorteia as perguntas e controla respostas, prêmios e estados finais. | Amostragem sem repetição, arrays e máquina de estados. |
| [Ajuda](include/ajuda.h) e [pistas](src/pistas.c) | Controlam usos, eliminam alternativas e fornecem pistas, contexto e sugestões. | Vetores de estado, busca por ID e sorteio. |
| [Ranking](include/ranking.h) | Registra resultados e devolve as dez melhores posições. | Histórico em arquivo e inserção ordenada em vetor limitado. |
| [Menu de terminal](src/menu_temporario.c) e [menu Raylib](src/menu.c) | Implementam a entrada e a saída de cada interface. | Separação da interface e das regras, compilação por módulos. |

### Como os dados são mantidos

**Cadastro.** `Cadastro` guarda um ponteiro para o vetor, a quantidade ocupada e a capacidade reservada. Quando o vetor enche, `adicionarPergunta` tenta ampliar a capacidade em dez posições. O resultado de `realloc` é guardado em um ponteiro temporário para preservar o vetor original caso falte memória. Ao encerrar, `liberarCadastro` libera o vetor. A busca por ID é linear.

**Partida.** `iniciarPartida` confere a quantidade disponível em cada dificuldade antes de sortear. Para cada nível, preenche as vagas iniciais e usa amostragem por reservatório para dar aos registros seguintes a chance de substituir uma vaga. Em seguida, embaralha as escolhidas dentro do mesmo nível. `Partida` guarda cópias das 15 perguntas, o índice atual, os acertos, o prêmio atual e o garantido. Assim, a partida tem os dados necessários mesmo depois de liberar o cadastro. Um acerto comum avança, o último acerto vence, um erro causa derrota e a desistência encerra preservando o valor acumulado.

**Ajudas.** O uso de cada ajuda vale para toda a partida. Os efeitos na tela valem para a pergunta atual. Ao avançar, o jogo limpa as alternativas eliminadas e as sugestões, mas mantém registrados os usos já gastos. Missão Espacial e Telescópio preservam o uso quando falta conteúdo para a pergunta.

### Arquivos

| Arquivo | Uso |
| --- | --- |
| `dados/perguntas.csv` | Banco inicial. As colunas são ID, nível, enunciado, quatro alternativas e letra correta. Campos textuais entre aspas podem conter vírgulas, aspas duplicadas e quebras de linha. |
| `dados/perguntas.dat` | Cadastro carregado primeiro. O programa lê a quantidade e os registros binários de `Pergunta`. Como o formato grava estruturas C diretamente, a compatibilidade depende da representação usada pela compilação. |
| `dados/ranking.tsv` | Histórico local com estado final, prêmio, acertos e nome, separados por tabulações. `COSMOS_RANKING` permite usar outro arquivo. |

## Verificação e roteiro de apresentação

| Suíte de `make test` | O que verifica |
| --- | --- |
| [Jogo](tests/test_jogo.c) | Sorteio, distribuição, IDs únicos, cadastro insuficiente, entradas inválidas, progressão, marcos, vitória, derrota e desistência. |
| [Ajudas](tests/test_ajuda.c) | Disponibilidade, limites de uso, bloqueio de alternativas eliminadas, textos, interação entre as ajudas e reinício. |
| [Integração](tests/teste_integracao.c) | Primeira execução via CSV, recarga pelo binário, recuperação de binário truncado, erros de gravação e preservação de vírgulas, aspas e quebras de linha no CSV. |

Sugestão para dividir a demonstração entre os três integrantes:

1. **Cadastro e arquivos.** Inicie com o caminho de demonstração, mostre as 80 perguntas e abra o menu de cadastro. Mostre uma exportação e explique por que o CSV é lido quando o binário ainda não existe.
2. **Interface e integração.** Mostre o papel de `executarMenu` e em que ponto a interface recebe o cadastro. Use o terminal para demonstrar o fluxo integrado e apresente a janela Raylib conforme o estágio atual.
3. **Regras, ajudas e ranking.** Inicie uma partida, use a Supernova para mostrar duas alternativas eliminadas e tente reutilizá-la. Encerre a partida, registre o nome, abra o ranking e execute `make test` para mostrar como os marcos e os estados finais são verificados.

## Referência Doxygen

Com o Doxygen instalado, execute na raiz:

```sh
doxygen Doxyfile
```

Abra `build/doxygen/html/index.html` para consultar as estruturas, funções e arquivos do projeto. A configuração inclui os cabeçalhos e fontes do Cosmos e exclui os cabeçalhos de terceiros.

## Contexto acadêmico

- **Instituição:** Universidade Tecnológica Federal do Paraná (UTFPR), Campus Apucarana
- **Curso:** Engenharia de Computação
- **Disciplina:** Fundamentos de Programação 2 — FDCO2B (2026/2)
- **Docente:** Profa. Muriel de Souza Godoi
- **Tema:** Astronomia

---

Projeto acadêmico desenvolvido em equipe para aplicação prática dos conceitos de programação em C.
