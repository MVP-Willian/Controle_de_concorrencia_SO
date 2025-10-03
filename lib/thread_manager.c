//
// Created by michaelwpv on 03/02/2025
//

#include "thread_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ThreadManaged** array_threads = NULL;

static int array_threads_count = 0;

static int array_threads_capacity = 0;


// Faz uma manipulação das threads em um array 
static void add_thread_to_manager(ThreadManaged* new_thread){
    if(array_threads_count>array_threads_capacity){
        int new_capacity = (array_threads_capacity==0) ? 10 : array_threads_capacity*2;
        ThreadManaged** new_array_threads = realloc( array_threads, new_capacity * sizeof(ThreadManaged*));

        if(!new_array_threads){
            perror("Falha ao redemensionar o array de threads!");
            exit(EXIT_FAILURE);
        }
    }
    array_threads[array_threads_count] = new_thread;
    array_threads_count++;
}


void manager_initialize() {
    int initial_capacity = 10;
    array_threads = malloc(initial_capacity * sizeof(ThreadManaged*));

    if(!array_threads){
        perror("Falha ao alocar memória inicial para o gerenciador de threads");
        exit(EXIT_FAILURE);

    }
    
    array_threads_count = 0;
    array_threads_capacity = initial_capacity;

}

ThreadManaged* manager_create_thread(ThreadType type, void *(*function_thread)(void *), void *args){
    ThreadManaged* new_thread = malloc(sizeof(ThreadManaged));
    if(!new_thread){
        perror("Falha ao alocar memória inicial para a criação de uma nova thread");
        return NULL;
    }
    new_thread->type = type;
    new_thread->idThread =  array_threads_count + 1; 

    int status = pthread_create(&new_thread->handle, NULL, function_thread, args);
    if(!status){
        fprintf(stderr, "Erro ao criar thread: %s\n", strerror(status));
        free(new_thread);
        return NULL;
    }
    return new_thread;
}

void manager_create_set_thread(int amount, ThreadType type, void* (*function_thread)(void *), void* (*generator_args)(int)){
    for (int numThread = 0; numThread < amount; numThread ++){
        void* args_to_thread = NULL;
        if(generator_args){
            args_to_thread = generator_args(numThread);
        }
        ThreadManaged* thread = manager_create_thread(type, function_thread, generator_args);
        if(!thread){
            fprintf(stderr, "AVISO: Falha ao criar a thread de ID %d do lote.\n", numThread + 1);

        }
    }
}

void manager_wait_all(){
    for (int numThread=0; numThread< array_threads_count; numThread++){
        int status = pthread_join(array_threads[numThread]->handle, NULL);
        if(!status){
            fprintf(stderr, "Erro ao esperar (join) pela thread %d: %s\n", array_threads[numThread]->idThread, strerror(status));
        }

    }
}

void manager_clead(){
    for (int numThread = 0; numThread< array_threads_count; numThread++){
        free(array_threads[numThread]);
    }

    free(array_threads);
    array_threads = NULL;
    array_threads_count = 0;
    array_threads_capacity = 0;

}






