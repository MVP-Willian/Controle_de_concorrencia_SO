//
// Created by michael on 17/10/2025
//

#ifndef PRODCONS_C
#define PRODCONS_C

#include "buffer.h" 

// include/prodcons.h (Adicionar ao seu .h)

typedef enum {
    PRODUCER_SUCCESS = 0,
    PRODUCER_ERROR_DATA_MISSING = 1,
    PRODUCER_ERROR_ALLOCATION = 2
} ProdutorStatus;

typedef enum {
    CONSUMER_SUCCESS = 0,
    CONSUMER_ERROR_ALLOCATION = 1
} ConsumidorStatus;

// Estrutura para a Thread Produtora receber a lista de débitos a serem produzidos
typedef struct {
    Debito *debitos_a_produzir; //Ponteiro para a lista de débitos
    int total_debitos;       //Total de débitos na lista
} ProdutorData;


//Estrutura de argumentos para as threads produtoras e consumidoras
typedef struct {
    BufferCompartilhado* buffer;
    int thread_id;
    int is_safe_mode; // 1 para modo seguro (com controle), 0 para modo inseguro (sem controle)
    int duracao_execucao_ms;

    //Ponteiro para os dados específicos da thread (será ProdutorData* ou NULL para consumidor)
    void* dados_especificos;
} ThreadArgs;

void *produtor_main(void* args);
void *consumidor_main(void* args);

#endif // PRODCONS_C