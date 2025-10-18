//
// Created by michaelwpv on 16/10/2025.
//
#ifndef BUFFER_H
#define BUFFER_H


#include <pthread.h>
#include "debito.h"

#define TAMANHO_MAXIMO 10

/// @brief 
typedef struct 
{
    Debito itens[TAMANHO_MAXIMO];
    int in;
    int out;
    int contador;
    
} BufferCompartilhado;

/**
 * @brief Inicialiaza buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser inicializado.
 */
void inicializar_buffer(BufferCompartilhado *buffer);


/**
 * @brief Produz trabalho no buffer
 *    
 *
 * @param buffer ponteiro para o buffer que irá ser inserido o trabalho.
 * @param debito ponteiro para o debito que será inserido no buffer.
 */
void produzir_item(BufferCompartilhado *buffer, Debito debito);

void produzir_item_sem_controle(BufferCompartilhado *buffer, Debito debito);  

/**
 * @brief Finaliza/Libera buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser finalizado.
 */
void destruir_buffer(BufferCompartilhado *buffer);


/**
 * @brief Consome o débito do buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser consumido.
 */
Debito consumir_item_sem_controle(BufferCompartilhado *buffer);



#endif 