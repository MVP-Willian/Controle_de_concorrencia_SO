// lib/prodcons.c
//
// Created by user on 17/10/2025
//

#include "../include/prodcons.h"
#include "../include/debito.h"
#include "../include/buffer.h"
#include "../include/thread_manager.h"


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
        fprintf(stderr, "Erro: Falha na alocação de memória para status do produtor.\n");
        return NULL;
    }
    *status_ptr = PRODUCER_SUCCESS; // Usa PRODCONS_SUCCESS, conforme o .h

    int id = thread_args->thread_id;
    int safe_mode = thread_args->is_safe_mode;

    ProdutorData* produtor_data = (ProdutorData*)thread_args->dados_especificos;

    // 1. Verificação de Dados (Seguindo o padrão produtor_main)
    if( !produtor_data || !produtor_data->debitos_a_produzir){
        fprintf(stderr, "Erro [P %d]: Dados específicos do produtor inválidos.\n", id);
        *status_ptr = PRODUCER_ERROR_DATA_MISSING;
        return (void*)status_ptr;
    }

    printf("Produtor %d iniciado. Modo seguro: %s\n", id, safe_mode ? "Sim" : "Não");
    printf("Total de débitos a produzir: %d\n", produtor_data->total_debitos);

    // 2. Iteração e Produção
    for (int i = 0; i < produtor_data->total_debitos; i++) {
        Debito* debito_para_produzir = &produtor_data->debitos_a_produzir[i];

        // AÇÃO: Alterna entre seguro e inseguro
        if (safe_mode) {
            // Versões 1 & 2 (SEGURAS): Usa semáforo/mutex
            printf("[P %d] SEG: Agendando #%d (Cont: %d).\n", id, debito_para_produzir->id_transacao, buffer->contador);
            produzir_item(buffer, *debito_para_produzir);
        }
        else {
            // Versão 3 (INSEGURA): SEM CONTROLE. Onde a inconsistência ocorre.
            printf("[P %d] INSEG: Agendando #%d (Cont: %d).\n", id, debito_para_produzir->id_transacao, buffer->contador);
            produzir_item_sem_controle(buffer, *debito_para_produzir);
        }
        // Simula trabalho
        simular_trabalho(thread_args->duracao_execucao_ms);
    }
    printf("[P %d]: Finalizado. Todos os débitos produzidos.\n", id);
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

    // Aloca e inicializa o ponteiro de status de saida (Seguindo o padrão do Produtor)
    ProdutorStatus* status_ptr = (ProdutorStatus*) malloc(sizeof(ProdutorStatus));
    if (!status_ptr) {
        fprintf(stderr, "Erro: Falha na alocação de memória para status do consumidor.\n");
        return NULL;
    }
    *status_ptr = CONSUMER_SUCCESS; 

    int id = thread_args->thread_id;
    int safe_mode = thread_args->is_safe_mode;

    printf("Consumidor %d iniciado. Modo seguro: %s\n", id, safe_mode ? "Sim" : "Não");
    
    // O Consumidor roda em loop infinito, esperando por itens para processar/pagar
    while (1) {
        Debito item_consumido;

        // AÇÃO: Alterna entre seguro e inseguro (Chamando as funções de buffer.c)
        if (safe_mode) {
            // Versões SEGURAS: Usa semáforo/mutex
            printf("[C %d] SEG: Buscando débito.\n", id);
            item_consumido = consumir_item(buffer);
        }
        else {
            // Versão INSEGURA: Busy Waiting (Pode travar a CPU e expor a Race Condition)
            printf("[C %d] INSEG: Buscando débito (ALERTA: BUSY WAIT).\n", id);
            item_consumido = consumir_item_sem_controle(buffer);
        }
        
        // Execução (Simula o pagamento)
        printf("[C %d]: Executando pagamento #%d...\n", id, item_consumido.id_transacao);
        executa_debito(&item_consumido);
        
        // Simula tempo de processamento
        simular_trabalho(thread_args->duracao_execucao_ms);
        
        // NOTA: O loop é quebrado pela thread principal com pthread_cancel
    }
    
    printf("[C %d]: Finalizado (Cancelado).\n", id);
    return (void*)status_ptr;
}