#include "../include/produtorXconsumidor.h"
#include "../include/thread_manager.h"
#include "../include/prodcons.h"
#include "../include/sync.h"
#include "../include/debito.h"
#include "../include/theme.h" // Para ContaBancaria
#include "../include/buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>



// =========================================================================
// VARIÁVEIS DE ESTADO DE MÓDULO (DECLARADAS NO TOPO PARA VISIBILIDADE)
// =========================================================================

static BufferCompartilhado *g_buffer = NULL;
static int g_is_safe_mode = 0;
static int g_produtor_count = 0; 
static int g_total_debitos_por_produtor = 0;
static int g_total_debitos_gerados = 0;

// FONTE DE DADOS REAL: DEBITOS*
static Debito *g_fonte_dados_total = NULL; // <<< ESTA VARIÁVEL PRECISA SER VISTA POR generator_args



// Fonte de dados de exemplo (As contas bancárias usadas para gerar débitos)
ContaBancaria g_conta_origem_base = {5000, 100000.00, "Fonte Pagadora"};
ContaBancaria g_conta_destino_base = {6000, 50000.00, "Fonte Recebedora"};


// =========================================================================
// FUNÇÃO AUXILIAR: GERAÇÃO DE DADOS (Para simular o "banco de dados")
// =========================================================================

void set_produtor_count(int value) {
    g_produtor_count = value;
}

/**
 * @brief Gera uma lista de débitos aleatórios para o cenário.
 * @param quantidade O número total de débitos a gerar.
 * @return Um ponteiro para o array Debito alocado na heap.
 */
Debito* gerar_fonte_dados(int quantidade) {
    Debito *debitos = (Debito *)malloc(quantidade * sizeof(Debito));
    if (!debitos) {
        perror("Falha ao alocar fonte de dados");
        return NULL;
    }

    srand(time(NULL));
    for (int i = 0; i < quantidade; i++) {
        // Inicializa o débito. Note que é alocado na heap temporariamente.
        Debito* temp_debito = inicializa_debito(
            1000 + i, // ID da transação
            &g_conta_origem_base, 
            &g_conta_destino_base, 
            (double)(rand() % 100) + 1.00 // Valor entre 1.00 e 100.99
        );
        g_total_debitos_gerados += temp_debito->valor;
        if (temp_debito) {
            debitos[i] = *temp_debito; // Copia a estrutura para o array
            finaliza_debito(temp_debito); // Libera o ponteiro temporário
        }
    }
    return debitos;
}


// =========================================================================
// FUNÇÃO AUXILIAR: Geração de Argumentos para o Thread Manager (O GLUE CODE)
// =========================================================================

/**
 * @brief Função exigida pelo thread_manager para criar argumentos para CADA thread.
 * @param index O índice da thread (0, 1, 2, ...).
 * @return Um ponteiro void* (ThreadArgs*) para os argumentos da thread.
 */
void* generator_args(int index) {
    ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
    if (!args) {
        perror("Falha ao alocar ThreadArgs");
        exit(EXIT_FAILURE);
    }
    
    // Configura os argumentos comuns
    args->buffer = g_buffer;
    args->thread_id = index + 1; // ID da thread (1-based)
    args->is_safe_mode = g_is_safe_mode;
    args->duracao_execucao_ms = 10; // Tempo de trabalho simulado (10ms)
    args->dados_especificos = NULL; 

    // Se a thread é PRODUTORA (index < g_produtor_count)
    if (index < g_produtor_count) {
        
        // Aloca e preenche o ProdutorData para a thread
        ProdutorData *p_data = (ProdutorData *)malloc(sizeof(ProdutorData));
        if (!p_data) { perror("Falha ao alocar ProdutorData"); exit(EXIT_FAILURE); }
        
        // Calcule o bloco de dados que esta thread deve processar
        int inicio_bloco = index * g_total_debitos_por_produtor;
        
        p_data->debitos_a_produzir = &g_fonte_dados_total[inicio_bloco];
        p_data->total_debitos = g_total_debitos_por_produtor;
        
        args->dados_especificos = p_data;
    }
    // Threads Consumidoras usam dados_especificos = NULL

    return (void*)args;
}


// =========================================================================
// FUNÇÃO PRINCIPAL: ORQUESTRAÇÃO rodar_versao
// =========================================================================

void rodar_versao(int versao, int num_prod, int num_cons, int duracao_segundos) {
    printf("\n======================================================\n");
    printf("RODANDO VERSÃO %d | P: %d | C: %d | MODO: %s\n", 
           versao, num_prod, num_cons, (versao != VERSAO_3_INSEGURO) ? "SEGURA" : "INSEGURA (VULNERÁVEL)");
    printf("======================================================\n");
    
    // 1. Configuração Inicial e Alocação de Recursos
    g_buffer = (BufferCompartilhado *)malloc(sizeof(BufferCompartilhado));
    if (!g_buffer) { perror("Falha ao alocar buffer"); exit(EXIT_FAILURE); }
    inicializar_buffer(g_buffer);
    
    g_is_safe_mode = (versao != VERSAO_3_INSEGURO);
    g_produtor_count = num_prod;
    
    // Define que cada produtor fará 10 débitos
    g_total_debitos_por_produtor = 10; 
    const int TOTAL_DEBITOS_GERADOS = num_prod * g_total_debitos_por_produtor; 

    // Geração da fonte de dados total (Simula o banco de dados de boletos)
    g_fonte_dados_total = gerar_fonte_dados(TOTAL_DEBITOS_GERADOS); 
    printf("[INFO INIT] Fonte de dados gerada com %d débitos.\n", TOTAL_DEBITOS_GERADOS);
    printf("[INFO INIT] Total de débitos a ser descontado da conta bancária: %.2f\n", (double)g_total_debitos_gerados);
    printf("[INFO INIT] Saldo que a conta destino tem que ficar no final: %.2f\n", 
           g_conta_destino_base.saldo + (double)g_total_debitos_gerados);
    printf("[INFO INIT] Saldo que a conta origem tem que ficar no final: %.2f\n", 
           g_conta_origem_base.saldo - (double)g_total_debitos_gerados);

    if (g_is_safe_mode) {
        init_sync(); // Inicializa Mutex e Semáforos (V1 e V2)
    }
    
    // 2. CRIAÇÃO E INÍCIO DAS THREADS
    manager_thread_initialize();

    // Produtores (Função main: produtor_main)
    manager_create_set_threads(num_prod, PRODUCER, produtor_main, generator_args);
    
    // Consumidores (Função main: consumidor_main)
    // O índice das threads consumidoras começa após o último produtor
    manager_create_set_threads(num_cons, CONSUMER, consumidor_main, generator_args);

    
    // O uso de sleep é necessário para que a main não termine antes das threads
    // Em um sistema real, o processo terminaria após as threads Produtoras e Consumidoras
    // esvaziarem o buffer, mas aqui, o cancelamento por tempo é mais simples.
    sleep(duracao_segundos); 
    
    // 4. LIMPEZA E FINALIZAÇÃO (Fase de Limpeza)
    // FORÇA O CANCELAMENTO DE TODAS AS THREADS BLOQUEADAS (Consumidor)
    manager_thread_cancel_all(); // <--- ESSA CHAMADA LIBERA O CONSUMIDOR DO SEM_WAIT
    manager_thread_wait_all(); // Coleta o status e aguarda o término

    if (g_is_safe_mode) {
        destroy_sync();
    }
    
    // 5. RELATÓRIO FINAL
    printf("\n[RESULTADO] Buffer Final: %d itens (Idealmente: 0)\n", g_buffer->contador);
    printf("[RESULTADO] Total de débitos gerados: %d\n", g_buffer->gerado_por_id);
    printf("[RESULTADO] Total do valor dos débitos processados: %.2f\n", (double)g_total_debitos_gerados);
    printf("[RESULTADO] Saldo final da Conta Origem: %.2f (Idealmente: %.2f)\n", 
           g_conta_origem_base.saldo, 
           100000.00 - (double)g_total_debitos_gerados);
    printf("[RESULTADO] Saldo final da Conta Destino: %.2f (Idealmente: %.2f)\n", 
           g_conta_destino_base.saldo, 
           50000.00 + (double)g_total_debitos_gerados);
    if (g_buffer->contador != 0) {
        printf("[ALERTA] Buffer não esvaziado ou erro de contagem.\n");
    }
    
    // Liberação de memória
    destruir_buffer(g_buffer); 
    free(g_buffer);
    free(g_fonte_dados_total);
}