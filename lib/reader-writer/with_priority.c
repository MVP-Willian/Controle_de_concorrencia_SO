//
// Created by luizg on 09/10/2025.
//

#include "../../include/reader-writer/with_priority.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/logger.h"
#include "../../include/reader-writer/reader_writer.h"

// Variáveis de controle para readers-writers com prioridade de escritores
static pthread_mutex_t resource_mutex;      // Protege o recurso compartilhado
static pthread_mutex_t reader_mutex;        // Protege o contador de leitores
static pthread_mutex_t writer_mutex;        // Controla acesso dos escritores
static int reader_count = 0;                // Número de leitores ativos
static int writer_count = 0;                // Número de escritores esperando/ativos
static pthread_mutex_t writer_count_mutex;  // Protege o contador de escritores

void with_priority_initialize()
{
    pthread_mutex_init(&resource_mutex, NULL);
    pthread_mutex_init(&reader_mutex, NULL);
    pthread_mutex_init(&writer_mutex, NULL);
    pthread_mutex_init(&writer_count_mutex, NULL);
    reader_count = 0;
    writer_count = 0;
}

void with_priority_destroy()
{
    pthread_mutex_destroy(&resource_mutex);
    pthread_mutex_destroy(&reader_mutex);
    pthread_mutex_destroy(&writer_mutex);
    pthread_mutex_destroy(&writer_count_mutex);
}

void* reader_with_priority(void* arg)
{
    const ThreadArgs* args = (ThreadArgs*)arg;
    char titular[100];
    char buffer[256];

    logger(RW_READER, args->thread_id, "Thread Criada");

    usleep(100);

    // Verifica se há escritores esperando
    pthread_mutex_lock(&writer_mutex);
    pthread_mutex_lock(&reader_mutex);

    reader_count++;
    if (reader_count == 1) {
        // Primeiro leitor tenta adquirir o recurso
        pthread_mutex_lock(&resource_mutex);
    }

    pthread_mutex_unlock(&reader_mutex);
    pthread_mutex_unlock(&writer_mutex);

    logger(RW_READER, args->thread_id, "Entrando na região crítica");

    // Leitura protegida - sem leitura suja
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

    pthread_mutex_lock(&reader_mutex);
    reader_count--;
    if (reader_count == 0) {
        // Último leitor libera o recurso
        pthread_mutex_unlock(&resource_mutex);
    }
    pthread_mutex_unlock(&reader_mutex);

    logger(RW_READER, args->thread_id, "Thread Finalizada");

    return NULL;
}

void* writer_with_priority(void* arg)
{
    const ThreadArgs* args = (ThreadArgs*)arg;
    char buffer[100];

    logger(RW_WRITER, args->thread_id, "Thread Criada");

    usleep(100);

    // Incrementa contador de escritores para bloquear novos leitores
    pthread_mutex_lock(&writer_count_mutex);
    writer_count++;
    pthread_mutex_unlock(&writer_count_mutex);

    // Escritor tem prioridade - bloqueia novos leitores
    pthread_mutex_lock(&writer_mutex);
    pthread_mutex_lock(&resource_mutex);

    logger(RW_WRITER, args->thread_id, "Entrando na região crítica");

    const double saldo_atual = args->conta->saldo;
    snprintf(buffer, sizeof(buffer), "Saldo antes de atualizar: %.2f", saldo_atual);
    logger(RW_WRITER, args->thread_id, buffer);
    usleep(1);
    args->conta->saldo = saldo_atual + args->valor_operacao;
    snprintf(buffer, sizeof(buffer), "Saldo após de atualizar: %.2f", args->conta->saldo);
    logger(RW_WRITER, args->thread_id, buffer);

    logger(RW_WRITER, args->thread_id, "Saindo da região crítica");

    pthread_mutex_unlock(&resource_mutex);
    pthread_mutex_unlock(&writer_mutex);

    // Decrementa contador de escritores
    pthread_mutex_lock(&writer_count_mutex);
    writer_count--;
    pthread_mutex_unlock(&writer_count_mutex);

    logger(RW_WRITER, args->thread_id, "Thread Finalizada");

    return NULL;
}
