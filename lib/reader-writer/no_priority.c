//
// Created by luizg on 09/10/2025.
//

#include "../../include/reader-writer/no_priority.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/logger.h"
#include "../../include/reader-writer/reader_writer.h"

static pthread_mutex_t write_mutex;

void no_priority_initialize()
{
    pthread_mutex_init(&write_mutex, NULL);
}

void no_priority_destroy()
{
    pthread_mutex_destroy(&write_mutex);
}

void* reader_no_priority(void* arg)
{
    const ThreadArgs* args = (ThreadArgs*)arg;
    char titular[100];
    char buffer[256];

    logger(RW_READER, args->thread_id, "Thread Criada");

    usleep(100);
    logger(RW_READER, args->thread_id, "Entrando na região crítica");

    // Leitura sem bloqueio - permite leitura suja
    const int numero_conta = args->conta->numero_conta;
    const double saldo = args->conta->saldo;
    strncpy(titular, args->conta->titular, 99);
    titular[99] = '\0';
    usleep(1);
    snprintf(buffer, sizeof(buffer),
             "Numero da conta: %d | Saldo: %.2f | Titular: %s",
             numero_conta, saldo, titular);
    logger(RW_READER, args->thread_id, buffer);

    logger(RW_READER, args->thread_id, "Saindo da região crítica");
    logger(RW_READER, args->thread_id, "Thread Finalizada");

    return NULL;
}

void* writer_no_priority(void* arg)
{
    const ThreadArgs* args = (ThreadArgs*)arg;
    char buffer[100];

    logger(RW_WRITER, args->thread_id, "Thread Criada");

    usleep(100);

    // Adquire o mutex para escrita exclusiva
    pthread_mutex_lock(&write_mutex);
    logger(RW_WRITER, args->thread_id, "Entrando na região crítica");

    const double saldo_atual = args->conta->saldo;
    snprintf(buffer, sizeof(buffer), "Saldo antes de atualizar: %.2f", saldo_atual);
    logger(RW_WRITER, args->thread_id, buffer);
    usleep(1);
    args->conta->saldo = saldo_atual + args->valor_operacao;
    snprintf(buffer, sizeof(buffer), "Saldo após de atualizar: %.2f", args->conta->saldo);
    logger(RW_WRITER, args->thread_id, buffer);

    logger(RW_WRITER, args->thread_id, "Saindo da região crítica");
    pthread_mutex_unlock(&write_mutex);

    logger(RW_WRITER, args->thread_id, "Thread Finalizada");

    return NULL;
}
