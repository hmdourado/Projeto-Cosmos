CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude

# Substitua pelo arquivo do menu definitivo quando ele estiver disponível.
MENU_SRC ?= src/menu_temporario.c
BASE_SRC = src/pergunta.c src/cadastro.c src/arquivo.c src/jogo.c
HEADERS = $(wildcard include/*.h)
PROGRAMA = build/cosmos

.PHONY: all clean run test

all: $(PROGRAMA)

build:
	mkdir -p build

$(PROGRAMA): main.c $(BASE_SRC) $(MENU_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c $(BASE_SRC) $(MENU_SRC) $(LDFLAGS) $(LDLIBS) -o $@

build/test_jogo: tests/test_jogo.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_jogo.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

build/teste_integracao: tests/teste_integracao.c main.c $(BASE_SRC) $(HEADERS) Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/teste_integracao.c $(BASE_SRC) $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_jogo build/teste_integracao
	./build/test_jogo
	./build/teste_integracao

run: $(PROGRAMA)
	./$(PROGRAMA) $(ARGS)

# Remove somente os executáveis gerados, preservando os dados do usuário.
clean:
	$(RM) build/cosmos build/test_jogo build/teste_integracao
