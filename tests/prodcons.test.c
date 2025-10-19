//
// Created by michael on 17/10/2025
//
#include "../include/produtorXconsumidor.h"
#include "../include/prodcons.h"
#include "../include/debito.h"
#include "../include/thread_manager.h"
#include "../include/sync.h"
#include "../include/tests.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Declaramos externamente as funções que o produtor_main chama:
// Asseguramos que elas não são executadas em concorrência aqui.
extern void produzir_item(BufferCompartilhado *buffer, Debito debito);
extern void produzir_item_sem_controle(BufferCompartilhado *buffer, Debito debito);
extern void simular_trabalho(int duracao_ms);


int test_produtor_main_fluxo_Inseguro() {
    printf("\n--- Teste de Fluxo: produtor_main (Modo Inseguro) ---\n");

    // VARIÁVEIS DE SIMULAÇÃO
    #define NUM_ITENS 3
    ContaBancaria c_dummy = {999, 500.00, "Teste"};
    
    // 1. SETUP: Criar a fonte de dados (Débitos)
    Debito *debitos = (Debito *)malloc(NUM_ITENS * sizeof(Debito));

    for (int i = 0; i < NUM_ITENS; i++) {
        // Inicializa o débito. Note que é alocado na heap temporariamente.
        Debito* temp_debito = inicializa_debito(
            101 + i, // ID da transação
            &c_dummy,
            &c_dummy, 
            10 + (i * 10.00)
        );
        
        if (temp_debito) {
            debitos[i] = *temp_debito; // Copia a estrutura para o array
            finaliza_debito(temp_debito); // Libera o ponteiro temporário
        }
    }
    ProdutorData *p_data = (ProdutorData *)malloc(sizeof(ProdutorData));
    p_data->debitos_a_produzir = debitos;
    p_data->total_debitos = NUM_ITENS;



    // 2. SETUP: Criar o Buffer e os Argumentos da Thread
    BufferCompartilhado buffer_teste;
    inicializar_buffer(&buffer_teste);
    

    ThreadArgs *args = malloc(sizeof(ThreadArgs));
    args->buffer = &buffer_teste;
    args->thread_id = 1;
    args->is_safe_mode = 0; // Modo INSEGURO
    args->duracao_execucao_ms = 0; // Desabilita o sleep para
    args->dados_especificos = p_data;


    // 3. EXECUÇÃO: Chama a função diretamente (execução serial)
    void *raw_resultado = produtor_main((void*)args);
    
    // 4. VERIFICAÇÃO (ASSERTIONS)
    // Converte o resultado para o tipo esperado
    ProdutorStatus *status_ptr = (ProdutorStatus *)raw_resultado;

    if(status_ptr == NULL) {
        fprintf(stderr, "Erro: produtor_main retornou NULL.\n");
        assert(0);
        return 0;
    }

    // Assertion 1: A função deve ter terminado normalmente (retornar NULL)
    assert(*status_ptr == PRODUCER_SUCCESS);

    // Assertion 2: O contador deve ser igual ao número de itens produzidos.
    // Verifica se a lógica de iteração e chamada foi correta.
    assert(buffer_teste.contador == NUM_ITENS);
    
    // Assertion 3: O ponteiro 'in' deve ter avançado 3 posições.
    assert(buffer_teste.in == NUM_ITENS); 
    
    // Assertion 4: O primeiro item (ID 101) deve estar no buffer na posição 0.
    assert(buffer_teste.itens[0].id_transacao == 101);

    free(status_ptr);
    // 5. LIMPEZA

    printf("Teste de Fluxo Produtor (Seguro) passou com sucesso. Itens no buffer: %d\n", buffer_teste.contador);
    return 1;
}

int main(){
    printf("--- Iniciando testes para prodcons (Modo Inseguro/Modo Seguro) ---\n");

    RUN_TEST(test_produtor_main_fluxo_Inseguro);

    printf("--- Testes de  Prodcons concluídos ---\n");
    return 0;
}



