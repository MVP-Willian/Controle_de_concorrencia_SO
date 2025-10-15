//
// Created by luizg on 09/10/2025.
//


#include <stdio.h>
#include <stdlib.h>

#include "../../include/tests.h"
#include "../../include/reader-writer/no_control.h"
#include "../../include/reader-writer/reader_writer.h"
#include "../../include/theme.h"

static ContaBancaria* g_shared_conta;

int test_no_control_reader()
{
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");

    ThreadArgs args = {.thread_id = 1, .conta = g_shared_conta, .valor_operacao = 0.0};

    reader_no_control(&args);
    return 1;
}

int test_no_control_writer()
{
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");

    ThreadArgs args = {.thread_id = 1, .conta = g_shared_conta, .valor_operacao = -10.0};

    writer_no_control(&args);
    return 1;
}

int main()
{
    printf("--- Iniciando testes para logger ---\n");

    RUN_TEST(test_no_control_reader);
    RUN_TEST(test_no_control_writer);


    printf("--------------------------------------\n");
}
