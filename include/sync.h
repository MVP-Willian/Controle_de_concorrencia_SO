//
// Created by michaelwillian on 17/10/2025.
//


#ifndef SYNC_H
#define SYNC_H  

#include <semaphore.h>
#include <pthread.h>
#include "buffer.h"

extern sem_t *sem_vazio; //contador de vagas (capacidade do buffer)
extern sem_t *sem_cheio; //contador de itens (itens disponíveis no buffer)
extern pthread_mutex_t mutex_buffer; //mutex para exclusão mútua no buffer
extern pthread_mutex_t contas_mutex;


/**
 * @brief Inicializa os mecanismos de sincronização
 */
void init_sync();

/**
 * @brief Destroi os mecanismos de sincronização
 */
void destroy_sync();

#endif // SYNC_H