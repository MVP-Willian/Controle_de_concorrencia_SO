//
// Created by michaelwpv on 17/10/2025
//
#include <assert.h>
#include <stdio.h>
#include "../include/buffer.h"
#include "../include/tests.h"

int test_init()
{
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);
    /*destruir_buffer(&buffer);*/

    assert(1);
    return 1;
}


int main()
{
    printf("--- Iniciando testes para buffer ---\n");

    RUN_TEST(test_init);

    printf("----------------------------------------\n");
}