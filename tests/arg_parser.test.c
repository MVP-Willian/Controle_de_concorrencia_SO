//
// Created by luizg on 02/10/2025.
//

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../include/arg_parser.h"
#include "../include/tests.h"

int test_run = 0;
int test_failed = 0;

int test_all_args_present()
{
    int quantidade = 0;
    char* tipo = NULL;
    Argument defs[] = {
        {"--quantidade", 'q', "", ARG_TYPE_INT, 1, &quantidade, 0},
        {"--tipo", 't', "", ARG_TYPE_STRING, 1, &tipo, 0}
    };

    const int defs_count = sizeof(defs) / sizeof(Argument);
    char* argv[] = {"program", "--quantidade", "123", "--tipo", "parafuso"};
    const int argc = sizeof(argv) / sizeof(char*);

    const int result = parse_args(argc, argv, defs, defs_count);

    assert(result == 0);
    assert(quantidade == 123);
    assert(strcmp(tipo, "parafuso") == 0);

    return 1;
}

// TODO: Fazer os testes de como o programa se comportar em erro (valor inválido, faltando parâmetro, etc).
// TODO: Fazer testes se tá pegando valores curtos
// TODO: Fazer testes require tá funcionando
// TODO: Fazer testes de mensagem imprimindo

int main()
{
    printf("--- Iniciando testes para arg_parser ---\n");

    RUN_TEST(test_all_args_present);

    printf("--------------------------------------\n");
}
