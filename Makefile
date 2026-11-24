
CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude

MENU_SRC ?= src/menu.c

# Modulos de regras e gerenciamento de dados.
BASE_SRC = src/pergunta.c \
           src/cadastro.c \
           src/arquivo.c \
           src/jogo.c \
           src/ajuda.c \
           src/pistas.c \
           src/ranking.c

# Modulos de interface grafica, audio e personagem.
PAGINAS_SRC = src/telas/cadastro.c \
              src/telas/jogo.c \
              src/telas/ajuda.c \
              src/telas/ranking.c \
              src/telas/audio.c \
              src/telas/personagem.c

# Modulo de reproducao de video.
VIDEO_SRC = src/video.c

ifeq ($(OS),Windows_NT)
EXE_EXT = .exe
RAYLIB_LIBS ?= lib/libraylib_win64.a -lopengl32 -lgdi32 -lwinmm
else
EXE_EXT =
CPPFLAGS += -D_POSIX_C_SOURCE=200809L
RAYLIB_LIBS ?= lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
endif

HEADERS = $(wildcard include/*.h)

PROGRAMA = build/cosmos$(EXE_EXT)
TERMINAL = build/cosmos_terminal$(EXE_EXT)
TEST_JOGO = build/test_jogo$(EXE_EXT)
TEST_INTEGRACAO = build/teste_integracao$(EXE_EXT)
TEST_AJUDA = build/test_ajuda$(EXE_EXT)
PREVIEW_JOGO = build/preview_tela_jogo$(EXE_EXT)

.PHONY: all clean run run-terminal run-grafico test preview

all: $(PROGRAMA)

build:
	mkdir -p build

# Preview independente da tela de perguntas.
$(PREVIEW_JOGO): tests/preview_tela_jogo.c $(BASE_SRC) $(PAGINAS_SRC) $(VIDEO_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/preview_tela_jogo.c $(BASE_SRC) $(PAGINAS_SRC) $(VIDEO_SRC) $(LDFLAGS) $(LDLIBS) $(RAYLIB_LIBS) -o $@

preview: $(PREVIEW_JOGO)
	./$(PREVIEW_JOGO)

# Aplicacao principal.
$(PROGRAMA): main.c $(BASE_SRC) $(MENU_SRC) $(PAGINAS_SRC) $(VIDEO_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c $(BASE_SRC) $(MENU_SRC) $(PAGINAS_SRC) $(VIDEO_SRC) $(LDFLAGS) $(LDLIBS) $(RAYLIB_LIBS) -o $@

# Aplicacao em modo terminal.
$(TERMINAL): main.c $(BASE_SRC) $(TERMINAL_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c $(BASE_SRC) $(TERMINAL_SRC) $(LDFLAGS) $(LDLIBS) -o $@

# Testes automatizados.
$(TEST_JOGO): tests/test_jogo.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_jogo.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

$(TEST_INTEGRACAO): tests/teste_integracao.c main.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/teste_integracao.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

$(TEST_AJUDA): tests/test_ajuda.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_ajuda.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

test: $(TEST_JOGO) $(TEST_INTEGRACAO) $(TEST_AJUDA)
	./$(TEST_JOGO)
	./$(TEST_INTEGRACAO)
	./$(TEST_AJUDA)

# Execucao.
run: $(PROGRAMA)
	./$(PROGRAMA) $(ARGS)

run-grafico: run

run-terminal: $(TERMINAL)
	./$(TERMINAL) $(ARGS)

# Remove executaveis gerados, preservando os dados do usuario.
clean:
	$(RM) build/cosmos build/cosmos.exe \
		build/cosmos_terminal build/cosmos_terminal.exe \
		build/test_jogo build/test_jogo.exe \
		build/teste_integracao build/teste_integracao.exe \
		build/test_ajuda build/test_ajuda.exe \
		build/preview_tela_jogo build/preview_tela_jogo.exe
