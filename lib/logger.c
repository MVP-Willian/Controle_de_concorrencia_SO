//
// Created by luizg on 09/10/2025.
//

#include "../include/logger.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

void logger(const char* tipo, int thread_id, const char* acao)
{
    const time_t now = time(NULL);

    char* time_str = ctime(&now);

    // Retira o \n no final da string
    time_str[strlen(time_str) - 1] = '\0';

    printf("[%s] Thread %d (%s): %s\n",
           time_str, thread_id, tipo, acao);
}
