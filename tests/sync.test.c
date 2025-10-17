//
//created by michaelwillian on 17/10/2025.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/sync.h"
#include "../include/tests.h"


int test_init()
{
    // Inicializa os mecanismos de sincronização
    init_sync();

    // Verifica se os semáforos e mutex foram inicializados corretamente
    assert(pthread_mutex_trylock(&mutex_buffer) == 0);
    pthread_mutex_unlock(&mutex_buffer);


    assert(sem_trywait(sem_vazio) == 0);
    sem_post(sem_vazio);
    assert(sem_trywait(sem_cheio) == -1);
    sem_post(sem_cheio);


    printf("Teste de inicialização dos mecanismos de sincronização passou com sucesso.\n");

    // Destroi os mecanismos de sincronização
    destroy_sync();

    return 1; // Retorna 1 para indicar que o teste passou
}

int main()
{
    printf("--- Iniciando testes para sincronização ---\n");

    RUN_TEST(test_init);

    printf("--- Todos os testes de sincronização concluídos ---\n");
    return 0;
}