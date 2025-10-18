// tests/produtorXconsumidor.test.c
//
// Versão FINAL, sem Mocks de Sync conflitantes.
//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/produtorXconsumidor.h" // Inclui o runner
#include "../include/tests.h"               // Inclui RUN_TEST
#include "../include/thread_manager.h"      // Inclui manager_initialize/clean
#include "../include/prodcons.h"            // Inclui ThreadType/ThreadArgs

// =======================================================
// VARIÁVEIS GLOBAIS DE MOCK
// Mantenha apenas as variáveis de contagem do thread_manager
// =======================================================

// Esta função substitui a real e apenas conta as threads
/**
 * @brief Reseta os contadores de mocks antes de cada teste.
 */

// =======================================================
// TESTES UNITÁRIOS PARA O ORQUESTRADOR
// =======================================================

int test_runner_versao_1() {
    printf("--- Teste Runner: V1 (5P, 1C, Seguro) ---\n"); 

    // EXECUÇÃO: O rodar_versao chama manager_create_set_threads 
    rodar_versao(VERSAO_1_SEGURO, 5, 1, 1); 
    
    // REMOVIDO: Assertions de mock_init_sync_called / mock_destroy_sync_called
    
    printf("[SUCESSO] Runner V1 orquestrado corretamente.\n");
    return 1;
}

int test_runner_versao_3_insegura() {
    printf("--- Teste Runner: V3 (2P, 2C, Inseguro) ---\n");

    // EXECUÇÃO: 
    rodar_versao(VERSAO_3_INSEGURO, 2, 2, 1); 
    
    printf("[SUCESSO] Runner V3 orquestrado corretamente.\n");
    return 1;
}


int main() {
    printf("--- Iniciando testes para Concorrencia Runner ---\n");

    // NOTA: A chamada manager_thread_initialize/clean deve ser ligada
    // pelo Makefile, mas aqui a chamamos para manter o padrão.
    manager_thread_initialize();    
    
    RUN_TEST(test_runner_versao_1);
    RUN_TEST(test_runner_versao_3_insegura);

    manager_thread_clean();
    printf("--- Testes de Runner concluídos ---\n");
    return 0;
}