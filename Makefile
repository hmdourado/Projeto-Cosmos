CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude

# Tela Raylib inicial (janela vazia).
MENU_SRC ?= src/menu.c
BASE_SRC = src/pergunta.c src/cadastro.c src/arquivo.c src/jogo.c
PAGINAS_SRC = src/telas/cadastro.c src/telas/jogo.c src/telas/ajuda.c src/telas/ranking.c
ifeq ($(OS),Windows_NT)
EXE_EXT = .exe
RAYLIB_LIBS ?= lib/libraylib_win64.a -lopengl32 -lgdi32 -lwinmm
else
EXE_EXT =
RAYLIB_LIBS ?= lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
endif
HEADERS = $(wildcard include/*.h)
PROGRAMA = build/cosmos$(EXE_EXT)
TEST_JOGO = build/test_jogo$(EXE_EXT)
TEST_INTEGRACAO = build/teste_integracao$(EXE_EXT)

.PHONY: all clean run test

all: $(PROGRAMA)

build:
	mkdir -p build

$(PROGRAMA): main.c $(BASE_SRC) $(MENU_SRC) $(PAGINAS_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c $(BASE_SRC) $(MENU_SRC) $(PAGINAS_SRC) $(LDFLAGS) $(LDLIBS) $(RAYLIB_LIBS) -o $@

$(TEST_JOGO): tests/test_jogo.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_jogo.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

$(TEST_INTEGRACAO): tests/teste_integracao.c main.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/teste_integracao.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

test: $(TEST_JOGO) $(TEST_INTEGRACAO)
	./$(TEST_JOGO)
	./$(TEST_INTEGRACAO)

run: $(PROGRAMA)
	./$(PROGRAMA) $(ARGS)

# Remove somente os executáveis gerados, preservando os dados do usuário.
clean:
	$(RM) build/cosmos build/cosmos.exe build/test_jogo build/test_jogo.exe build/teste_integracao build/teste_integracao.exe
