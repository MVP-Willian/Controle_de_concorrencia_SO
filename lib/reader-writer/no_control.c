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

    logger(READER, args->thread_id, "Thread Criada");

    sleep(1);
    logger(READER, args->thread_id, "Entrando na região crítica");

    int numero_conta = args->conta->numero_conta;
    double saldo = args->conta->saldo;

    char titular[100];
    strncpy(titular, args->conta->titular, 99);
    titular[99] = '\0';

    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "Numero da conta: %d | Saldo: %.2f | Titular: %s",
             numero_conta, saldo, titular);
    logger(READER, args->thread_id, buffer);
    logger(READER, args->thread_id, "Saindo da região crítica");
    logger(READER, args->thread_id, "Thread Finalizada");

    return NULL;
}

void* writer_no_control(void* arg)
{
    return NULL;
}
