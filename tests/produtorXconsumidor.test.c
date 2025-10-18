//
// Created by michaelwillian on 17/10/2025
//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/produtorXconsumidor.h"
#include "../include/tests.h"
#include "../include/thread_manager.h" 

// Variáveis Globais de Mock para rastrear as chamadas
static int mock_prod_amount = 0;
static int mock_cons_amount = 0;
static ThreadType mock_prod_type = -1;
static ThreadType mock_cons_type = -1;
static int mock_init_sync_called = 0;
static int mock_destroy_sync_called = 0;


// =======================================================
// MOCKS (Substituição de Funções Reais)
// =======================================================

// Substitui as chamadas de inicialização e limpeza de sync
void test_init_sync_mock() { mock_init_sync_called = 1; }
void test_destroy_sync_mock() { mock_destroy_sync_called = 1; }

// =======================================================
// TESTES UNITÁRIOS PARA O ORQUESTRADOR
// =======================================================

/**
 * @brief Reseta os contadores de mocks antes de cada teste.
 */
void reset_mocks() {
    mock_prod_amount = 0;
    mock_cons_amount = 0;
    mock_init_sync_called = 0;
    mock_destroy_sync_called = 0;
}


int test_runner_versao_1() {
    printf("--- Teste Runner: V1 (5P, 1C, Seguro) ---\n");
    reset_mocks();

    // EXECUÇÃO: Cenário V1 (5 Produtores, 1 Consumidor, Seguro)
    rodar_versao(VERSAO_1_SEGURO, 5, 1, 1); 

    // VERIFICAÇÃO
    assert(mock_init_sync_called == 1);       // Deve inicializar sync (Modo Seguro)
    assert(mock_destroy_sync_called == 1);    // Deve limpar sync
    assert(mock_prod_amount == 5);            // Deve criar 5 Produtores
    assert(mock_cons_amount == 1);            // Deve criar 1 Consumidor
    assert(mock_prod_type == PRODUCER);       // Tipo deve ser PRODUCER
    assert(mock_cons_type == CONSUMER);       // Tipo deve ser CONSUMER
    
    printf("[SUCESSO] Runner V1 orquestrado corretamente.\n");
    return 1;
}

int test_runner_versao_3_insegura() {
    printf("--- Teste Runner: V3 (2P, 2C, Inseguro) ---\n");
    reset_mocks();

    // EXECUÇÃO: Cenário V3 (2 Produtores, 2 Consumidores, Inseguro)
    rodar_versao(VERSAO_3_INSEGURO, 2, 2, 1); 

    // VERIFICAÇÃO
    assert(mock_init_sync_called == 0);       // NÃO deve inicializar sync (Modo Inseguro)
    assert(mock_destroy_sync_called == 0);    // NÃO deve limpar sync
    assert(mock_prod_amount == 2);            // Deve criar 2 Produtores
    assert(mock_cons_amount == 2);            // Deve criar 2 Consumidores
    
    printf("[SUCESSO] Runner V3 orquestrado corretamente.\n");
    return 1;
}


int main() {
    printf("--- Iniciando testes para Concorrencia Runner ---\n");

    // NOTA: O thread_manager_initialize/clean deve ser chamado aqui
    manager_thread_initialize();    
    
    RUN_TEST(test_runner_versao_1);
    RUN_TEST(test_runner_versao_3_insegura);

    manager_thread_clean();
    printf("--- Testes de Runner concluídos ---\n");
    return 0;
}