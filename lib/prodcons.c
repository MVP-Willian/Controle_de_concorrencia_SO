// lib/prodcons.c
//
// Created by user on 17/10/2025
//

#include "../include/prodcons.h"
#include "../include/debito.h"
#include "../include/buffer.h"
#include "../include/thread_manager.h"
#include "../include/produtorXconsumidor.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


// =========================================================================
// DECLARAÇÕES EXTERNAS (Funções implementadas em outros arquivos)
// =========================================================================

// Funções de Buffer (em buffer.c)
extern void produzir_item(BufferCompartilhado *buffer, Debito debito); 
extern Debito consumir_item(BufferCompartilhado *buffer); 
extern void produzir_item_sem_controle(BufferCompartilhado *buffer, Debito debito);  
extern Debito consumir_item_sem_controle(BufferCompartilhado *buffer);
extern void main_monitor(int num_threads);
extern int g_consumidor_count; // Declaração externa da variável global de consumidor
extern int g_produtor_count; // Declaração externa da variável global de produtor
// Funções de Manipulação de Débito (em debito.c)
extern void executa_debito(Debito* debito);

// =========================================================================
// FUNÇÕES AUXILIARES
// =========================================================================

// Simula trabalho com sleep (com ponto de cancelamento)
void simular_trabalho(int duracao_ms) {
    if(duracao_ms > 0) {
        usleep(duracao_ms * 1000); // Converte milissegundos para microssegundos
    }
    pthread_testcancel(); // Permite que a thread seja cancelada
}

// =========================================================================
// FUNÇÃO PRINCIPAL DO PRODUTOR
// =========================================================================

void *produtor_main(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    BufferCompartilhado* buffer = thread_args->buffer;

    // Aloca e inicializa o ponteiro de status de saida
    ProdutorStatus* status_ptr = (ProdutorStatus*) malloc(sizeof(ProdutorStatus));
    if (!status_ptr) {
        fprintf(stderr, "\033[41m\033[30mErro: Falha na alocação de memória para status do produtor.\033[0m\n");
        return NULL;
    }
    *status_ptr = PRODUCER_SUCCESS; // Usa PRODCONS_SUCCESS, conforme o .h

    int id = thread_args->thread_id;
    int safe_mode = thread_args->is_safe_mode;

    ProdutorData* produtor_data = (ProdutorData*)thread_args->dados_especificos;

    // 1. Verificação de Dados (Seguindo o padrão produtor_main)
    if( !produtor_data || !produtor_data->debitos_a_produzir){
        fprintf(stderr, "\033[41m\033[30mErro [P %d]: Dados específicos do produtor inválidos.\033[0m\n", id);
        *status_ptr = PRODUCER_ERROR_DATA_MISSING;
        return (void*)status_ptr;
    }

    // 2. Iteração e Produção
    for (int i = 0; i < produtor_data->total_debitos; i++) {
        Debito* debito_para_produzir = &produtor_data->debitos_a_produzir[i];
        
        manager_update_transaction_id(id, debito_para_produzir->id_transacao);
        manager_update_thread_status(id, THREAD_STATUS_PRODUZINDO);

        // AÇÃO: Alterna entre seguro e inseguro
        if (safe_mode) {
            // Versões 1 & 2 (SEGURAS): Usa semáforo/
            manager_update_thread_status(id, THREAD_STATUS_DORMINDO);
            main_monitor(g_produtor_count + g_consumidor_count); // Atualiza o monitor antes de produzir
            produzir_item(buffer, *debito_para_produzir);
            manager_update_thread_status(id, THREAD_STATUS_PRODUZINDO);
            main_monitor(g_produtor_count + g_consumidor_count); // Atualiza o monitor após produzir

        }
        else {
            // Versão 3 (INSEGURA): SEM CONTROLE. Onde a inconsistência ocorre.
            produzir_item_sem_controle(buffer, *debito_para_produzir);
        }
        // Simula trabalho
        manager_update_transaction_id(id, 0);
        manager_update_thread_status(id, THREAD_STATUS_DORMINDO);
        simular_trabalho(thread_args->duracao_execucao_ms);
    }
    pthread_mutex_lock(&stdout_mutex);
    printf("[P %d]: Finalizado. Todos os débitos produzidos.\n", id);
    fflush(stdout);
    pthread_mutex_unlock(&stdout_mutex);

    manager_update_thread_status(id, THREAD_STATUS_FINALIZADO);
    if(produtor_data){
        free(produtor_data);
    }
    free(thread_args);
    return (void*)status_ptr;
}

// =========================================================================
// FUNÇÃO PRINCIPAL DO CONSUMIDOR
// =========================================================================

void *consumidor_main(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    BufferCompartilhado* buffer = thread_args->buffer;

    // Aloca e inicializa o ponteiro de status de saida (Seguindo o padrão do Consumidor)
    ConsumidorStatus* status_ptr = (ConsumidorStatus*) malloc(sizeof(ConsumidorStatus));
    if (!status_ptr) {
        fprintf(stderr, "Erro: Falha na alocação de memória para status do consumidor.\n");
        return NULL;
    }
    *status_ptr = CONSUMER_SUCCESS; 

    int safe_mode = thread_args->is_safe_mode;
    

    
    // O Consumidor roda em loop infinito, esperando por itens para processar/pagar
    while (1) {
        Debito item_consumido;

        // AÇÃO: Alterna entre seguro e inseguro (Chamando as funções de buffer.c)
        if (safe_mode) {
            // Versões SEGURAS: Usa semáforo/mutex
            manager_update_thread_status(thread_args->thread_id, THREAD_STATUS_DORMINDO);
            main_monitor(g_produtor_count + g_consumidor_count); // Monitora o estado de espera

            item_consumido = consumir_item(buffer);

            manager_update_transaction_id(thread_args->thread_id, item_consumido.id_transacao);
            
            manager_update_thread_status(thread_args->thread_id, THREAD_STATUS_CONSUMINDO);
            main_monitor(g_produtor_count + g_consumidor_count);
        }
        else {
            // Versão INSEGURA: Busy Waiting (Pode travar a CPU e expor a Race Condition)
            manager_update_thread_status(thread_args->thread_id, THREAD_STATUS_ESPERA_OCUPADA);
            main_monitor(g_produtor_count + g_consumidor_count);
            item_consumido = consumir_item_sem_controle(buffer);

            manager_update_transaction_id(thread_args->thread_id, item_consumido.id_transacao);
            manager_update_thread_status(thread_args->thread_id, THREAD_STATUS_CONSUMINDO);
            main_monitor(g_produtor_count + g_consumidor_count);

        }
        
        // Execução (Simula o pagamento)
        executa_debito(&item_consumido);

        // Simula tempo de processamento
        simular_trabalho(thread_args->duracao_execucao_ms);
        
        if (safe_mode) {
            // 1. Zera a ID do item que acabou de ser processado (limpeza de dados).
            manager_update_transaction_id(thread_args->thread_id, 0); 

            // 2. Define o status de espera para o próximo ciclo.
            manager_update_thread_status(thread_args->thread_id, THREAD_STATUS_DORMINDO);
            
            // 3. Força a atualização do monitor para VER o DORMINDO/ID=0.
            main_monitor(g_produtor_count + g_consumidor_count); 
        }
        
        // NOTA: O loop é quebrado pela thread principal com pthread_cancel
    }
    
    return (void*)status_ptr;
}