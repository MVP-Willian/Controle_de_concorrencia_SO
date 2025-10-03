//
// Created by michaelwpv on 03/10/2025.
//

#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <pthread.h>


/** 
 * @enum ThreadType
 * @brief Enumeração indentificar o papel de cada thread_local
 */
typedef enum {
    READER,
    WRITER,
    PRODUCER,
    CONSUMER
} ThreadType;

/**
 * @struct ThreadManaged
 * @brief Estrutura que armazena as informações essenciais de uma thread.
 *
 * Mantém o handle retornado por pthread_create, além de metadados
 * úteis para o gerenciamento.
 */
typedef struct {
    pthread_t handle;
    int idThread;
    ThreadType type;
} ThreadManaged;


/**
 * @brief Inicializa o sistema de gerenciamento de threads.
 *
 * Deve ser a primeira função da biblioteca a ser chamada. Aloca os recursos
 * necessários para controlar as threads que serão criadas.
 */
void manager_initialize();

/**
 * @brief Cria e inicia uma única thread gerenciada.
 *
 * Função base para criação de threads. É útil para cenários mais complexos
 * onde as threads são criadas individualmente.
 *
 * @param type O tipo (papel) da thread a ser criada.
 * @param function O ponteiro para a função que a thread executará.
 * @param args O ponteiro para os argumentos que serão passados para a função da thread.
 * @return Um ponteiro para a estrutura ThreadManaged recém-criada, ou NULL em caso de erro.
 */
ThreadManaged* manager_create_thread(ThreadType type, void *(*function)(void *), void *args);

/**
 * @brief Cria um lote de threads do mesmo tipo e com a mesma função base.
 *
 *
 * @param amount O número de threads a serem criadas no lote.
 * @param type O tipo (papel) de todas as threads do lote.
 * @param function_thread O ponteiro para a função que todas as threads executarão.
 * @param generator_args Um ponteiro para uma função que gera os argumentos para cada thread.
 * Esta função receberá o índice (0, 1, 2...) como parâmetro e
 * deverá retornar um `void*` com os argumentos para aquela thread.
 * Pode ser NULL se as threads não precisarem de argumentos.
 */
void manager_create_set_threads(int amount, ThreadType type, void* (*function_thread)(void *), void* (*generator_args)(int));

/**
 * @brief Aguarda a finalização de todas as threads criadas pelo gerenciador.
 *
 * Esta função bloqueia a execução da thread chamadora (geralmente a main)
 * até que todas as threads de trabalho tenham concluído.
 */
void manager_wait_all();

/**
 * @brief Libera todos os recursos alocados pelo gerenciador de threads.
 *
 * Deve ser a última função da biblioteca a ser chamada para evitar vazamentos de memória.
 */
void manager_clean();




#endif //THREAD_MANAGER_H 