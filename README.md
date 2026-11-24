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
# Compilar
mingw32-make

# Executar
mingw32-make run

# Limpar arquivos gerados
mingw32-make clean
```

O compilador deve ser MinGW-w64 de 64 bits (`x86_64-w64-mingw32`).

**Compilar e executar**, a partir da raiz do repositório:

```powershell
mingw32-make clean
mingw32-make CC=gcc
.\build\cosmos.exe
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
# Compilar
make

# Executar
make run

# Limpar arquivos gerados
make clean
```

> **Nota sobre suporte multiplataforma:** a configuração de ligação atualmente registrada no `Makefile` utiliza a biblioteca `lib/libraylib_win64.a` e bibliotecas de sistema do Windows. Esse arquivo não pode ser usado em Linux. Portanto, a compilação Linux depende de uma configuração específica para Raylib/Linux no `Makefile`; ainda não está validada nesta versão do projeto.

## Dados e persistência

O banco inicial de perguntas é fornecido em CSV. O sistema também prevê salvar os dados atualizados em arquivo binário ao encerrar e recuperá-los nas execuções seguintes, além de permitir exportar o cadastro atual para CSV.

## Desenvolvimento e qualidade

O código é dividido em arquivos `.c` e `.h`, com responsabilidades separadas por módulo. A implementação deve manter nomes descritivos, indentação consistente, tratamento de erros, gerenciamento correto de memória e documentação das funções no padrão **Doxygen**, conforme as orientações da disciplina.

## Contexto acadêmico

- **Instituição:** Universidade Tecnológica Federal do Paraná (UTFPR), Campus Apucarana
- **Curso:** Engenharia de Computação
- **Disciplina:** Fundamentos de Programação 2 — FDCO2B (2026/2)
- **Docente:** Profa. Muriel de Souza Godoi
- **Tema:** Astronomia

---

Projeto acadêmico desenvolvido em equipe para aplicação prática dos conceitos de programação em C.
