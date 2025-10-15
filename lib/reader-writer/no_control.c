//
// Created by luizg on 09/10/2025.
//

#include "../../include/reader-writer/no_control.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/logger.h"
#include "../../include/reader-writer/reader_writer.h"

void* reader_no_control(void* arg)
{
    const ThreadArgs* args = (ThreadArgs*)arg;
    char titular[100];
    char buffer[256];

    logger(RW_READER, args->thread_id, "Thread Criada");

    sleep(1);
    logger(RW_READER, args->thread_id, "Entrando na região crítica");

    const int numero_conta = args->conta->numero_conta;
    const double saldo = args->conta->saldo;
    strncpy(titular, args->conta->titular, 99);
    titular[99] = '\0';
    snprintf(buffer, sizeof(buffer),
             "Numero da conta: %d | Saldo: %.2f | Titular: %s",
             numero_conta, saldo, titular);
    logger(RW_READER, args->thread_id, buffer);

    logger(RW_READER, args->thread_id, "Saindo da região crítica");
    logger(RW_READER, args->thread_id, "Thread Finalizada");

    return NULL;
}

void* writer_no_control(void* arg)
{
    const ThreadArgs* args = (ThreadArgs*)arg;
    char buffer[100];

    logger(RW_WRITER, args->thread_id, "Thread Criada");

    usleep(100);

    logger(RW_WRITER, args->thread_id, "Entrando na região crítica");

    snprintf(buffer, sizeof(buffer), "Saldo antes de atualizar: %.2f", args->conta->saldo);
    logger(RW_WRITER, args->thread_id, buffer);
    double saldo_atual = args->conta->saldo;
    usleep(1);
    args->conta->saldo = saldo_atual + args->valor_operacao;
    snprintf(buffer, sizeof(buffer), "Saldo após de atualizar: %.2f", args->conta->saldo);
    logger(RW_WRITER, args->thread_id, buffer);


    logger(RW_WRITER, args->thread_id, "Saindo da região crítica");
    logger(RW_WRITER, args->thread_id, "Thread Finalizada");

    return NULL;
}
