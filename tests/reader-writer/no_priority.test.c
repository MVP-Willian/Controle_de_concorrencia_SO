//
// Created by luizg on 09/10/2025.
//

#include <stdio.h>
#include <stdlib.h>

#include "../../include/tests.h"
#include "../../include/reader-writer/no_priority.h"
#include "../../include/reader-writer/reader_writer.h"
#include "../../include/theme.h"
#include "../../include/thread_manager.h"

static ContaBancaria* g_shared_conta;

int test_no_priority_reader()
{
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    no_priority_initialize();

    ThreadArgs args = {.thread_id = 1, .conta = g_shared_conta, .valor_operacao = 0.0};

    reader_no_priority(&args);

    no_priority_destroy();
    return 1;
}

int test_no_priority_writer()
{
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    no_priority_initialize();

    ThreadArgs args = {.thread_id = 1, .conta = g_shared_conta, .valor_operacao = -10.0};

    writer_no_priority(&args);

    no_priority_destroy();
    return 1;
}

void* generate_args(const int id)
{
    const int DISCOUNT_VALUE = -10;

    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args->thread_id = id;
    args->conta = g_shared_conta;
    args->valor_operacao = DISCOUNT_VALUE;

    return args;
}

int test_no_priority_n_writers()
{
    const int n = 5;
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    no_priority_initialize();
    manager_thread_initialize();

    manager_create_set_threads(n, WRITER, writer_no_priority, generate_args);

    manager_thread_wait_all();

    printf("Resultado final: %.2f\n", g_shared_conta->saldo);

    no_priority_destroy();
    return 1;
}

int test_no_priority_mixed_readers_writers()
{
    const int n_readers = 5;
    const int n_writers = 5;
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    no_priority_initialize();
    manager_thread_initialize();

    // Cria escritores
    manager_create_set_threads(n_writers, WRITER, writer_no_priority, generate_args);

    manager_create_set_threads(n_readers, READER, reader_no_priority, generate_args);

    manager_thread_wait_all();

    printf("Resultado final: %.2f\n", g_shared_conta->saldo);

    no_priority_destroy();
    return 1;
}

int main()
{
    printf("--- Iniciando testes para no_priority ---\n");

    RUN_TEST(test_no_priority_reader);
    RUN_TEST(test_no_priority_writer);
    RUN_TEST(test_no_priority_n_writers);
    RUN_TEST(test_no_priority_mixed_readers_writers);

    printf("------------------------------------------\n");
}
