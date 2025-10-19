//
// Created by michael on 17/10/2025
//

#include "../include/buffer.h"
#include "../include/debito.h"
#include "../include/sync.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//Declarações externas (funcoes variaveis definidas em sync.c)
extern sem_t *sem_vazio; //contador de vagas (capacidade do buffer)
extern sem_t *sem_cheio; //contador de itens (itens disponíveis no buffer)
extern pthread_mutex_t mutex_buffer; //mutex para exclusão mútua no buffer



// Implementação
void inicializar_buffer(BufferCompartilhado *buffer)
{
    if(buffer == NULL)
    {
        fprintf(stderr, "Erro: Ponteiro para ButterCompartilhado é NULL.\n");
        return;
    }
    
    buffer->in = 0;
    buffer->out = 0;
    buffer->contador = 0;
    buffer->gerado_por_id = 0;

    memset(buffer->itens, 0, TAMANHO_MAXIMO * sizeof(Debito));
}

void produzir_item_sem_controle(BufferCompartilhado *buffer, Debito debito)
{
    while(buffer->contador>=TAMANHO_MAXIMO)
    {
        // Espera ativa (busy waiting) - NÃO RECOMENDADO
        usleep(1000); // 1 milissegundo de pausa para evitar uso excessivo da CPU
        pthread_testcancel(); // Permite que a thread seja cancelada enquanto espera
    }

    buffer->itens[buffer->in] = debito;
    buffer->in = (buffer->in + 1) % TAMANHO_MAXIMO;
    buffer->contador++;
    buffer->gerado_por_id++;
};


/*void destruir_buffer(BufferCompartilhado *buffer);*/

Debito consumir_item_sem_controle(BufferCompartilhado *buffer)
{
    //1. CHECAGEM INSEGURA/ESPERA OCUPADA (BUSY WAITING)
    int max_retries = 10;
    while(buffer->contador <= 0)
    {
        max_retries--;
        usleep(10000); // 10000 microssegundos de pausa
        pthread_testcancel(); // Permite que a thread seja cancelada enquanto espera
    }
    
    Debito debito_consumido = buffer->itens[buffer->out];
    buffer->itens[buffer->out].status = 1; // Marca como consumido (opcional)
    buffer->out = (buffer->out + 1) % TAMANHO_MAXIMO;
    // SIMULA A INTERRUPÇÃO NO MEIO DA LEITURA/ESCRITA DO CONTADOR
    int temp_contador = buffer->contador; // Lê
    usleep(1); // ATRAZO FORÇADO
    buffer->contador = temp_contador - 1; //escreve

    return debito_consumido;

};


void produzir_item(BufferCompartilhado *buffer, Debito debito)
{
    //1. ESPERA PELO SEMÁFORO DE VAGAS
    sem_wait(sem_vazio);

    //2. TRAVA O MUTEX PARA EXCLUSÃO MÚTUA
    pthread_mutex_lock(&mutex_buffer);

    //3. INSERE O ITEM NO BUFFER
    buffer->itens[buffer->in] = debito;
    buffer->in = (buffer->in + 1) % TAMANHO_MAXIMO;
    buffer->contador++;
    buffer->gerado_por_id++;

    //4. DESBLOQUEIA O MUTEX
    pthread_mutex_unlock(&mutex_buffer);

    //5. SINALIZA O SEMÁFORO DE ITENS DISPONÍVEIS
    sem_post(sem_cheio);
};


Debito consumir_item(BufferCompartilhado *buffer) {
    Debito item_consumido;

    // 1. SINCRONIZAÇÃO: Espera por item disponível (Bloqueia se buffer vazio)
    // Decrementa o contador de itens (sem_cheio). Se zero, a thread dorme.
    sem_wait(sem_cheio); // Usa o ponteiro global de semáforo

    // 2. EXCLUSÃO MÚTUA: Trava o buffer para acesso exclusivo.
    pthread_mutex_lock(&mutex_buffer);

    // ---------------------- INÍCIO DA REGIÃO CRÍTICA ------------------------
    
    // A. Retirada do Item (Cópia da estrutura por valor)
    item_consumido = buffer->itens[buffer->out];
    buffer->itens[buffer->out].status = 1; // Marca como consumido (opcional)
    // B. Avança o ponteiro de saída (circular)
    buffer->out = (buffer->out + 1) % TAMANHO_MAXIMO;

    // C. Atualiza o contador de itens no buffer
    buffer->contador--; 
    
    // ---------------------- FIM DA REGIÃO CRÍTICA --------------------------

    // 3. EXCLUSÃO MÚTUA: Libera o mutex.
    pthread_mutex_unlock(&mutex_buffer);

    // 4. SINCRONIZAÇÃO: Sinaliza que liberou uma vaga (Acorda o Produtor)
    // Incrementa o contador de vagas livres (sem_vazio).
    sem_post(sem_vazio); // Usa o ponteiro global de semáforo
        
    return item_consumido;
};

void destruir_buffer(BufferCompartilhado *buffer)
{
    if (buffer == NULL)
    {
        return;
    }
    
    // Limpa o conteúdo do array interno (preenche com zeros).
    // Isso garante que não haja dados antigos remanescentes.
    memset(buffer->itens, 0, TAMANHO_MAXIMO * sizeof(Debito));
    
    // Reseta os contadores e ponteiros para o estado inicial.
    buffer->in = 0;
    buffer->out = 0;
    buffer->contador = 0;
    
    
    // NOTA: O 'free(buffer)' deve ser chamado no arquivo runner (produtorXconsumidor.c)
    // porque foi lá que a memória foi alocada com malloc.
}