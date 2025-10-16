//
// Created by luizg on 09/10/2025.
//

#include <stdio.h>
#include <stdlib.h>

#include "../../include/tests.h"
#include "../../include/reader-writer/with_priority.h"
#include "../../include/reader-writer/reader_writer.h"
#include "../../include/theme.h"
#include "../../include/thread_manager.h"

static ContaBancaria* g_shared_conta;

int test_with_priority_reader()
{
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    with_priority_initialize();

    ThreadArgs args = {.thread_id = 1, .conta = g_shared_conta, .valor_operacao = 0.0};

    reader_with_priority(&args);

    with_priority_destroy();
    return 1;
}

int test_with_priority_writer()
{
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    with_priority_initialize();

    ThreadArgs args = {.thread_id = 1, .conta = g_shared_conta, .valor_operacao = -10.0};

    writer_with_priority(&args);

    with_priority_destroy();
    return 1;
}

void* generate_writer_args(const int id)
{
    const int DISCOUNT_VALUE = -10;

    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args->thread_id = id;
    args->conta = g_shared_conta;
    args->valor_operacao = DISCOUNT_VALUE;

    return args;
}

void* generate_reader_args(const int id)
{
    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args->thread_id = id;
    args->conta = g_shared_conta;
    args->valor_operacao = 0.0;

    return args;
}

int test_with_priority_n_writers()
{
    const int n = 5;
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    with_priority_initialize();
    manager_thread_initialize();

    manager_create_set_threads(n, WRITER, writer_with_priority, generate_writer_args);

    manager_thread_wait_all();

    printf("Resultado final: %.2f\n", g_shared_conta->saldo);

    with_priority_destroy();
    return 1;
}

int test_with_priority_n_readers()
{
    const int n = 5;
    g_shared_conta = inicializa_conta(42, 100.0, "Usuário Teste");
    with_priority_initialize();
    manager_thread_initialize();

    manager_create_set_threads(n, READER, reader_with_priority, generate_reader_args);

    manager_thread_wait_all();

    printf("Saldo final: %.2f\n", g_shared_conta->saldo);

    with_priority_destroy();
    return 1;
}

int test_with_priority_mixed_readers_writers()
{
    const int n_readers = 10;
    const int n_writers = 5;
    g_shared_conta = inicializa_conta(42, 200.0, "Usuário Teste");
    with_priority_initialize();
    manager_thread_initialize();

    // Cria leitores primeiro
    manager_create_set_threads(n_readers, READER, reader_with_priority, generate_reader_args);

    // Cria escritores (devem ter prioridade)
    manager_create_set_threads(n_writers, WRITER, writer_with_priority, generate_writer_args);

    manager_thread_wait_all();

    printf("Resultado final: %.2f (esperado: %.2f)\n", 
           g_shared_conta->saldo, 200.0 + (n_writers * -10.0));

    with_priority_destroy();
    return 1;
}

int test_with_priority_writer_priority()
{
    const int n_readers = 15;
    const int n_writers = 3;
    g_shared_conta = inicializa_conta(42, 150.0, "Usuário Teste");
    with_priority_initialize();
    manager_thread_initialize();

    // Intercala criação de leitores e escritores
    for (int i = 0; i < n_readers; i++) {
        manager_create_thread(READER, reader_with_priority, generate_reader_args(i));
        if (i % 5 == 0 && i / 5 < n_writers) {
            manager_create_thread(WRITER, writer_with_priority, generate_writer_args(i / 5));
        }
    }

    manager_thread_wait_all();

    printf("Resultado final: %.2f (esperado: %.2f)\n", 
           g_shared_conta->saldo, 150.0 + (n_writers * -10.0));

    with_priority_destroy();
    return 1;
}

// TODO: Verificar o código para encontrar qualquer inconsitência

int main()
{
    printf("--- Iniciando testes para with_priority ---\n");

    RUN_TEST(test_with_priority_reader);
    RUN_TEST(test_with_priority_writer);
    RUN_TEST(test_with_priority_n_writers);
    RUN_TEST(test_with_priority_n_readers);
    RUN_TEST(test_with_priority_mixed_readers_writers);
    RUN_TEST(test_with_priority_writer_priority);

    printf("--------------------------------------------\n");
}
