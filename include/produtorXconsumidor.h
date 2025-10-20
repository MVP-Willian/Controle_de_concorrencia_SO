#include "./thread_manager.h"
#include "./sync.h"
#include "./debito.h"
#include "./theme.h" // Para ContaBancaria
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define VERSAO_1_SEGURO 1
#define VERSAO_2_SEGURO_MUTEX 2
#define VERSAO_3_INSEGURO 3 

void main_monitor(int num_threads);
// ...
// =========================================================================
// FUNÇÃO AUXILIAR: GERAÇÃO DE DADOS (Para simular o "banco de dados")
// =========================================================================

/**
 * @brief Gera uma lista de débitos aleatórios para o cenário.
 * @param quantidade O número total de débitos a gerar.
 * @return Um ponteiro para o array Debito alocado na heap.
 */
Debito* gerar_fonte_dados(int quantidade);

// =========================================================================
// FUNÇÃO AUXILIAR: Geração de Argumentos para o Thread Manager (O GLUE CODE)
// =========================================================================

/**
 * @brief Função exigida pelo thread_manager para criar argumentos para CADA thread.
 * @param index O índice da thread (0, 1, 2, ...).
 * @return Um ponteiro void* (ThreadArgs*) para os argumentos da thread.
 */
void* generator_args(int index) ;


// =========================================================================
// FUNÇÃO PRINCIPAL: ORQUESTRAÇÃO rodar_versao
// =========================================================================

void rodar_versao(int versao, int num_prod, int num_cons, int duracao_segundos, int debitor_por_produtor);