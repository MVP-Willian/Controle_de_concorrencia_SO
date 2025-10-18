//
// Created by luizg on 02/10/2025.
//

#include <stdio.h>
#include <stdlib.h>

#include "arg_parser.h"
#include "logger.h"
#include "thread_manager.h"
#include "reader-writer/no_control.h"
#include "reader-writer/no_priority.h"
#include "reader-writer/reader_writer.h"
#include "reader-writer/with_priority.h"
#include "produtorXconsumidor.h"

#define PRINCIPAL "PROGRAMA PRINCIPAL"
#define PRINCIPAL_ID (-1)

enum
{
    READER_WRITER = 0,
    PRODUCER_CONSUMER = 1,
};

static int quant_writers;
static int quant_readers;
static int test_type = READER_WRITER;
static ContaBancaria* g_shared_conta;
const int DISCOUNT_VALUE = -10;

void* generate_writer_args(const int id)
{
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

void testar_leitores_escritores(const int q1, const int q2)
{
    logger(PRINCIPAL, PRINCIPAL_ID, "Testando leitores e escritores sem prioridade");

    char buffer[255];
    double valor_conta_inicial;

    g_shared_conta = inicializa_conta(123, 500, "Usuário Teste");

    valor_conta_inicial = g_shared_conta->saldo;

    no_priority_initialize();
    manager_thread_initialize();

    manager_create_set_threads(q1, WRITER, writer_no_priority, generate_writer_args);

    manager_create_set_threads(q2, READER, reader_no_priority, generate_reader_args);

    manager_thread_wait_all();

    sprintf(buffer, "Resultado final: %.2f (esperado: %.2f)",
            g_shared_conta->saldo, valor_conta_inicial + (q1 * DISCOUNT_VALUE));

    logger(PRINCIPAL, PRINCIPAL_ID, buffer);

    no_priority_destroy();

    logger(PRINCIPAL, PRINCIPAL_ID, "Testando leitores e escritores com prioridade");

    g_shared_conta = inicializa_conta(123, 500, "Luiz Gabriel");

    valor_conta_inicial = g_shared_conta->saldo;

    with_priority_initialize();
    manager_thread_initialize();

    manager_create_set_threads(q1, READER, reader_with_priority, generate_reader_args);

    manager_create_set_threads(q2, WRITER, writer_with_priority, generate_writer_args);

    manager_thread_wait_all();

    sprintf(buffer, "Resultado final: %.2f (esperado: %.2f)",
            g_shared_conta->saldo, valor_conta_inicial + (q1 * DISCOUNT_VALUE));

    logger(PRINCIPAL, PRINCIPAL_ID, buffer);

    with_priority_destroy();

    logger(PRINCIPAL, PRINCIPAL_ID, "Testando leitores e escritores sem controle");

    g_shared_conta = inicializa_conta(123, 500, "Luiz Gabriel");

    valor_conta_inicial = g_shared_conta->saldo;

    manager_thread_initialize();

    manager_create_set_threads(q1, WRITER, writer_no_control, generate_writer_args);

    manager_create_set_threads(q2, READER, reader_no_control, generate_reader_args);

    manager_thread_wait_all();

    sprintf(buffer, "Resultado final: %.2f (esperado: %.2f)",
            g_shared_conta->saldo, valor_conta_inicial + (q1 * DISCOUNT_VALUE));

    logger(PRINCIPAL, PRINCIPAL_ID, buffer);
}

void testar_produtores_consumidores(const int q1, const int q2)
{
    logger(PRINCIPAL, PRINCIPAL_ID, "Testando Produtores e Consumidores");

    char buffer[255];

    // Versão 1: Segura (com semáforos e mutex)
    logger(PRINCIPAL, PRINCIPAL_ID, "Testando Versão 1 (SEGURA - com sincronização)");
    manager_thread_initialize();
    rodar_versao(VERSAO_1_SEGURO, q1, q2, 5); // 5 segundos de execução
    manager_thread_clean();

    // Versão 2:
    logger(PRINCIPAL, PRINCIPAL_ID, "Testando Versão 2 (SEGURA - com mutex)");
    manager_thread_initialize();
    rodar_versao(VERSAO_2_SEGURO_MUTEX, q1, q2, 5); // 5 segundos de execução
    manager_thread_clean();

    // Versão 3: Insegura (sem controle de concorrência)
    logger(PRINCIPAL, PRINCIPAL_ID, "Testando Versão 3 (INSEGURA - sem sincronização)");
    sprintf(buffer, "ATENÇÃO: Versão insegura - esperado condições de corrida!");
    logger(PRINCIPAL, PRINCIPAL_ID, buffer);

    manager_thread_initialize();
    rodar_versao(VERSAO_3_INSEGURO, q1, q2, 5); // 5 segundos de execução
    manager_thread_clean();

    logger(PRINCIPAL, PRINCIPAL_ID, "Testes de Produtor/Consumidor concluídos");

}

int main(int argc, char* argv[])
{
    logger(PRINCIPAL, PRINCIPAL_ID, "Iniciando o programa principal");

    Argument defs[] = {
        {
            .long_name = "--writer",
            .short_name = 'w',
            .help_text = "Quantidade de escritores ou produtores",
            .type = ARG_TYPE_INT,
            .required = 1,
            .value = &quant_writers,
            .found = 0
        },
        {
            .long_name = "--reader",
            .short_name = 'r',
            .help_text = "Quantidade de leitores ou consumidores",
            .type = ARG_TYPE_INT,
            .required = 1,
            .value = &quant_readers,
            .found = 0
        },
        {
            .long_name = "--switch",
            .short_name = 's',
            .help_text = "Definir teste para Produtor e Consumidor (Padrão: Leitor e Escritor)",
            .type = ARG_TYPE_FLAG,
            .required = 0,
            .value = &test_type,
            .found = 0
        }
    };

    const int defs_count = sizeof(defs) / sizeof(Argument);

    const int status = parse_args(argc, argv, defs, defs_count);

    if (status == MISSING_REQUIRED)
    {
        logger(PRINCIPAL, PRINCIPAL_ID, "(Erro) Faltou algum argumento obrigatório");
        print_usage(argv[0], defs, defs_count);
        return -1;
    }

    if (test_type)
    {
        logger(PRINCIPAL, PRINCIPAL_ID, "Teste definido para Produtor e Consumidor");
    }
    else
    {
        logger(PRINCIPAL, PRINCIPAL_ID, "Teste padrão utilizado de Leitor e Escritor");
    }

    char buffer[255];
    sprintf(buffer, "Quantidade de escritores: %d", quant_writers);
    logger(PRINCIPAL, PRINCIPAL_ID, buffer);
    sprintf(buffer, "Quantidade de leitores: %d", quant_readers);
    logger(PRINCIPAL, PRINCIPAL_ID, buffer);

    if (test_type)
    {
        testar_produtores_consumidores(quant_writers, quant_readers);
    }
    else
    {
        testar_leitores_escritores(quant_writers, quant_readers);
    }

    logger(PRINCIPAL, PRINCIPAL_ID, "Programa finalizado com sucesso");
    return 0;
}
