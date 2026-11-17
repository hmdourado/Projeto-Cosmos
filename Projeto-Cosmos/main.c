#include "include/cadastro.h"
#include "include/menu.h"

int main(void){
    Cadastro cadastro;

    inicializarCadastro(&cadastro);
    executarMenu(&cadastro);
    liberarCadastro(&cadastro);

    return 0;
}