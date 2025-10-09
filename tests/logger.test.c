//
// Created by luizg on 09/10/2025.
//

#include <stdio.h>

#include "../include/logger.h"
#include "../include/tests.h"

int test_logger_message()
{
    logger("LEITOR", 1, "Inicializando");
    return 1;
}

int main()
{
    printf("--- Iniciando testes para logger ---\n");

    RUN_TEST(test_logger_message);

    printf("--------------------------------------\n");
}
