//
// Created by michaelwpv on 16/10/2025.
//
#ifndef BUFFER_CIRCULAR_SINC_H
#define BUFFER_CIRCULAR_SINC_H


#include <pthread.h>


#define TAMANHO_MAXIMO 5

/// @brief 
typedef struct 
{
    int itens[TAMANHO_MAXIMO];
    int inicio;
    int fim;
    int contador;

    Mutex_t lock_exclusivo;

    Semaforo_t espacos_vazios;

    Semaforo_t itens_disponiveis;

} BufferCircularSincronizado_t;

/**
 * @brief Inicialiaza buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser inicializado.
 */
void inicializar_buffer(BufferCircularSincronizado_t *buffer);


/**
 * @brief Produz trabalho no buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser inserido o trabalho.
 * @param trabalho_id trabalho que será inserido no buffer.
 */
void produzir_item(BufferCircularSincronizado_t *buffer, int trabalho_id);


/**
 * @brief Finaliza/Libera buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser finalizado.
 */
void destruir_buffer(BufferCircularSincronizado_t *buffer);

/**
 * @brief Consome trabalho do buffer
 *
 *
 * @param buffer ponteiro para o buffer que irá ser consumido.
 */
int consumir_item(BufferCircularSincronizado_t *buffer);


#endif 