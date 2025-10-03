//
// Created by michaelwpv on 03/10/2025
//

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h> // Para sleep


#include "../include/tests.h"
#include "../include/thread_manager.h"

//Variaveis globais
int test_run = 0;
int test_failed = 0;
static int g_shared_count = 0;
static pthread_mutex_t g_test_mutex;

//Funções helper
void* simple_work_function(void *args)
{
    pthread_mutex_lock(&g_test_mutex);
    g_shared_count ++;
    pthread_mutex_unlock(&g_test_mutex);
    return NULL;
}

//Funções com os casos de testes


// Teste para validar a lib thread_manager.c

int test_init_and_cleanup()
{
    manager_thread_initialize();
    manager_thread_clean();

    assert(1);
    return 1;
}


int test_create_one_thread()
{
    pthread_mutex_init(&g_test_mutex, NULL);
    g_shared_count = 0;

    manager_thread_initialize();
    manager_create_thread(PRODUCER, simple_work_function, NULL);
    manager_thread_wait_all();

    assert(g_shared_count == 1);
    manager_thread_clean();
    pthread_mutex_destroy(&g_test_mutex);
    return 1;
}

int main()
{
    printf("--- Iniciando testes para thread_manager ---\n");

    RUN_TEST(test_init_and_cleanup);
    RUN_TEST(test_create_one_thread);
    

    printf("--------------------------------------\n");


}
