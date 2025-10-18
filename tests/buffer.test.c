//
// Created by michaelwpv on 17/10/2025
//
#include <assert.h>
#include <stdio.h>
#include "../include/buffer.h"
#include "../include/tests.h"
#include "../include/debito.h"
#include "../include/sync.h"
#include <memory.h>
#include <stdlib.h>

//Assinaturas externas 
extern void produzir_item(BufferCompartilhado *buffer, Debito debito);
extern void init_sync();
extern void destroy_sync();
//extern sem_t sem_vazio; //contador de vagas (capacidade do buffer)
//extern sem_t sem_cheio; //contador de itens (itens disponíveis no buffer)
//extern pthread_mutex_t mutex_buffer; //mutex para exclusão mútua no buffer

int test_init()
{
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);
    /*destruir_buffer(&buffer);*/

    assert(1);
    return 1;
}

int test_produzir_sem_controle(){
    printf("--- Teste de Fluxo: produzir_iem_sem_controle (Inseguro) ---\n");
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);

    ContaBancaria conta_origem = {12345, 1000.0, "João Silva"};
    ContaBancaria conta_destino = {67890, 500.0, "Maria Oliveira"};
    Debito* debito = inicializa_debito(1, &conta_origem, &conta_destino, 200.0);
    if(debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito para teste.\n");
        return 0;
    }

    produzir_item_sem_controle(&buffer, *debito);

    assert(buffer.contador == 1);
    assert(buffer.itens[0].id_transacao == 1);
    assert(buffer.itens[0].valor == 200.0);

    free(debito);
    return 1;
}

int test_consumir_sem_controle(){
    printf("--- Teste de Fluxo: consumir_item_sem_controle (Inseguro) ---\n");
    BufferCompartilhado buffer;
    inicializar_buffer(&buffer);

    ContaBancaria conta_origem = {12345, 1000.0, "João Silva"};
    ContaBancaria conta_destino = {67890, 500.0, "Maria Oliveira"};
    Debito* debito = inicializa_debito(1, &conta_origem, &conta_destino, 200.0);
    if(debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito para teste.\n");
        return 0;
    }

    produzir_item_sem_controle(&buffer, *debito);

    Debito debito_consumido = consumir_item_sem_controle(&buffer);

    assert(buffer.itens[0].id_transacao == 1);
    assert(buffer.out == 1);
    assert(buffer.contador == 0);
    assert(debito_consumido.id_transacao == 1);
    assert(debito_consumido.valor == 200.0);

    free(debito);
    return 1;
}

int test_produzir_controle() {
    printf("--- Teste de Fluxo: produzir_item (Seguro) ---\n");

    // VARIÁVEIS DE SETUP
    BufferCompartilhado buffer;
    ContaBancaria c_dummy = {11111, 100.0, "Teste"};
    
    // Lista de 3 débitos
    Debito d1 = {501, c_dummy, c_dummy, 5.00};
    Debito d2 = {502, c_dummy, c_dummy, 15.00};
    Debito d3 = {503, c_dummy, c_dummy, 25.00};

    // 1. SETUP: Inicializar Buffer e Sincronização
    inicializar_buffer(&buffer);
    init_sync(); // Inicializa sem_vazio (TAMANHO_MAXIMO) e sem_cheio (0)

    // 2. EXECUÇÃO: Produzir 3 itens em sequência
    produzir_item(&buffer, d1);
    produzir_item(&buffer, d2);
    produzir_item(&buffer, d3);

    // 3. VERIFICAÇÃO (ASSERTIONS)

    // Assertion 1: O contador deve ser 3.
    assert(buffer.contador == 3);
    
    // Assertion 2: O ponteiro 'in' deve ter avançado para a posição 3.
    assert(buffer.in == 3); 
    
    // Assertion 3: O primeiro item (d1) deve estar na posição correta e ser o correto.
    assert(buffer.itens[0].id_transacao == 501);
    assert(buffer.itens[0].valor == 5.00);

    // Assertion 4: O último item (d3) deve estar na posição correta.
    assert(buffer.itens[2].id_transacao == 503);


    
    // Assertion 5: Verificar o estado dos semáforos após as 3 produções (opcional, mas bom para prova de conceito).
    int s_vazio, s_cheio;
    sem_getvalue(sem_vazio, &s_vazio);
    sem_getvalue(sem_cheio, &s_cheio);
    
    // Esperado: sem_vazio = TAMANHO_MAXIMO - 3
    // Esperado: sem_cheio = 3
    assert(s_cheio == 3); 
    assert(s_vazio == TAMANHO_MAXIMO - 3);
    
   
    // 4. LIMPEZA
    destroy_sync();

    printf("Teste de Fluxo Produtor (Seguro) passou com sucesso. Itens finais: %d\n", buffer.contador);
    return 1;
}

int test_consumir_item(){
    printf("--- Teste de Fluxo: consumir_item (Seguro) ---\n");

    // VARIÁVEIS DE SETUP
    BufferCompartilhado buffer;
    ContaBancaria c_dummy = {22222, 200.0, "Teste2"};

    // Lista de 2 débitos
    Debito d1 = {601, c_dummy, c_dummy, 10.00};
    Debito d2 = {602, c_dummy, c_dummy, 20.00};

    // 1. SETUP: Inicializar Buffer e Sincronização
    inicializar_buffer(&buffer);
    init_sync();

    // 2. Produzir 2 itens
    produzir_item(&buffer, d1);
    produzir_item(&buffer, d2);

    // 3. Consumir 1 item
    Debito consumido = consumir_item(&buffer);

    // 4. ASSERTIONS

    // Assertion 1: O contador deve ser 1.
    assert(buffer.contador == 1);

    // Assertion 2: O ponteiro 'out' deve ter avançado para a posição 1.
    assert(buffer.out == 1);

    // Assertion 3: O débito consumido deve ser igual ao primeiro produzido (d1).
    assert(consumido.id_transacao == 601);
    assert(consumido.valor == 10.00);

    // Assertion 4: O próximo item no buffer deve ser d2.
    assert(buffer.itens[1].id_transacao == 602);

    // Assertion 5: Verificar o estado dos semáforos após consumir 1 item.
    int s_vazio, s_cheio;
    sem_getvalue(sem_vazio, &s_vazio);
    sem_getvalue(sem_cheio, &s_cheio);

    // Esperado: sem_vazio = TAMANHO_MAXIMO - 1
    // Esperado: sem_cheio = 1
    assert(s_cheio == 1);
    assert(s_vazio == TAMANHO_MAXIMO - 1);

    // 5. LIMPEZA
    destroy_sync();

    printf("Teste de Fluxo Consumidor (Seguro) passou com sucesso. Itens finais: %d\n", buffer.contador);
    return 1;
}

int test_destruir_buffer() {
    printf("--- Teste de Limpeza: destruir_buffer ---\n");

    BufferCompartilhado buffer;
    ContaBancaria c_dummy = {100, 1000.0, "Dummy"};
    
    // 1. SETUP: Inicializar o buffer e simular uso/preenchimento
    inicializar_buffer(&buffer);
    
    // Simular que 5 itens foram produzidos
    buffer.contador = 5;
    buffer.in = 5 % TAMANHO_MAXIMO; // Assumindo TAMANHO_MAXIMO > 5
    buffer.out = 2; // Simular que 2 itens já foram consumidos (para testar o reset)
    buffer.itens[3] = (Debito){1003, c_dummy, c_dummy, 50.0}; // Colocar um item para verificar a limpeza

    // Assertion de Pré-condição: Garantir que o setup está correto
    assert(buffer.contador == 5);
    assert(buffer.in != 0);

    // 2. EXECUÇÃO: Destruir o buffer
    destruir_buffer(&buffer);

    // 3. VERIFICAÇÃO (ASSERTIONS)

    // Assertion 1: O contador deve ser zero após a destruição.
    assert(buffer.contador == 0);

    // Assertion 2: Os ponteiros 'in' e 'out' devem ser resetados para zero.
    assert(buffer.in == 0);
    assert(buffer.out == 0);
    
    // Assertion 3: Verificar se o conteúdo da posição que tinha um débito foi limpo (memset para zero)
    // O valor do ID na posição 3 deve ser 0 (pois o memset zerou a memória).
    assert(buffer.itens[3].id_transacao == 0); 
    
    // Nota: Esta assertiva de limpeza é a prova de que o memset em destruir_buffer está funcionando.
    
    printf("Teste de Limpeza de Buffer passou com sucesso. Estado: Resetado.\n");
    return 1;
}

int main()
{
    printf("--- Iniciando testes para buffer ---\n");

    RUN_TEST(test_init);
    RUN_TEST(test_produzir_sem_controle);
    RUN_TEST(test_consumir_sem_controle);
    RUN_TEST(test_produzir_controle);
    RUN_TEST(test_consumir_item);
    RUN_TEST(test_destruir_buffer);

    printf("----------------------------------------\n");
}