//
// Created by michaelwpv on 17/10/2025
//
#include <assert.h>
#include <stdio.h>
#include "../include/buffer.h"
#include "../include/tests.h"
#include "../include/debito.h"
#include <memory.h>
#include <stdlib.h>

int test_init()
{
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);
    /*destruir_buffer(&buffer);*/

    assert(1);
    return 1;
}

int test_produzir_sem_controle(){
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);

    ContaBancaria conta_origem = {12345, 1000.0, "João Silva"};
    ContaBancaria conta_destino = {67890, 500.0, "Maria Oliveira"};
    Debito* debito = inicializa_debito(1, &conta_origem, &conta_destino, 200.0);
    if(debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito para teste.\n");
        return 0;
    }

    produzir_item_sem_controle(&buffer, *debito);

    assert(buffer.contador == 1);
    assert(buffer.itens[0].id_transacao == 1);
    assert(buffer.itens[0].valor == 200.0);

    free(debito);
    return 1;
}

int test_consumir_sem_controle(){
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);

    ContaBancaria conta_origem = {12345, 1000.0, "João Silva"};
    ContaBancaria conta_destino = {67890, 500.0, "Maria Oliveira"};
    Debito* debito = inicializa_debito(1, &conta_origem, &conta_destino, 200.0);
    if(debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito para teste.\n");
        return 0;
    }

    produzir_item_sem_controle(&buffer, *debito);

    Debito debito_consumido = consumir_item_sem_controle(&buffer);

    assert(buffer.itens[0].id_transacao == 1);
    assert(buffer.out == 1);
    assert(buffer.contador == 0);
    assert(debito_consumido.id_transacao == 1);
    assert(debito_consumido.valor == 200.0);

    free(debito);
    return 1;
}

int main()
{
    printf("--- Iniciando testes para buffer ---\n");

    RUN_TEST(test_init);
    RUN_TEST(test_produzir_sem_controle);
    RUN_TEST(test_consumir_sem_controle);

    printf("----------------------------------------\n");
}