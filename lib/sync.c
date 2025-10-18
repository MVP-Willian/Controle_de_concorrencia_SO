//
// Created by michaelwillian on 17/10/2025.
//

#include "../include/sync.h"
#include "../include/buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

//Definição dos semáforos e mutex
sem_t *sem_vazio; //contador de vagas (capacidade do buffer)
sem_t *sem_cheio; //contador de itens (itens disponíveis no buffer)
pthread_mutex_t mutex_buffer; //mutex para exclusão mútua no buffer

/**
 * @brief Inicializa os mecanismos de sincronização
 */
void init_sync() {
    const char *NOME_SEM_VAZIO = "/sem_vazio_michael";
    const char *NOME_SEM_CHEIO = "/sem_cheio_michael";
    
    if(pthread_mutex_init(&mutex_buffer, NULL) != 0) {
        perror("Falha ao inicializar o mutex do buffer");
        exit(EXIT_FAILURE);
    }

    //Limpeza de semáforos nomeados antigos, caso existam
    sem_unlink(NOME_SEM_VAZIO);
    sem_unlink(NOME_SEM_CHEIO);

    sem_vazio = sem_open(NOME_SEM_VAZIO, O_CREAT, 0644, TAMANHO_MAXIMO);
    if(sem_vazio == SEM_FAILED) {
        perror("Falha ao inicializar o semáforo de vagas (sem_vazio)");
        exit(EXIT_FAILURE);
    }

    sem_cheio = sem_open(NOME_SEM_CHEIO, O_CREAT, 0644, 0);
    if(sem_cheio == SEM_FAILED) {
        perror("Falha ao inicializar o semáforo de itens (sem_cheio)");
        exit(EXIT_FAILURE);
    }
    printf("Mecanismos de sincronização inicializados com sucesso.\n");
}

/**
 * @brief Destroi os mecanismos de sincronização
 */
void destroy_sync() {
    const char *NOME_SEM_VAZIO = "/sem_vazio_michael";
    const char *NOME_SEM_CHEIO = "/sem_cheio_michael";

    sem_close(sem_vazio);
    sem_close(sem_cheio);

    sem_unlink(NOME_SEM_VAZIO);
    sem_unlink(NOME_SEM_CHEIO);

    pthread_mutex_destroy(&mutex_buffer);
}