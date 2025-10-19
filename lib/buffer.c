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
    printf("Buffer inicializado com sucesso.\n");

}

void produzir_item_sem_controle(BufferCompartilhado *buffer, Debito debito)
{
    while(buffer->contador>=TAMANHO_MAXIMO)
    {
        // Espera ativa (busy waiting) - NÃO RECOMENDADO
        usleep(1000); // 1 milissegundo de pausa para evitar uso excessivo da CPU
        printf("\033[41m\033[30m[AVISO PROD] Impossibilitado de se produzir item, pois o número máximo %d itens do Buffer foi atingido.\033[0m\n", buffer->contador);
        pthread_testcancel(); // Permite que a thread seja cancelada enquanto espera
    }

    if((buffer->itens[buffer->in].status == 0) && buffer->itens[buffer->in].id_transacao != 0)
    {
        printf("\033[41m\033[30m[AVISO PROD]: Tentativa de produzir débito no lugar de debito ainda não processado (ID %d).\033[0m\n", buffer->itens[buffer->in].id_transacao);
    }

    buffer->itens[buffer->in] = debito;
    buffer->in = (buffer->in + 1) % TAMANHO_MAXIMO;
    buffer->contador++;
    buffer->gerado_por_id++;
    printf("[INFO PROD] Item foi produzido no buffer.\n");
};


/*void destruir_buffer(BufferCompartilhado *buffer);*/

Debito consumir_item_sem_controle(BufferCompartilhado *buffer)
{
    //1. CHECAGEM INSEGURA/ESPERA OCUPADA (BUSY WAITING)
    int max_retries = 10;
    while(buffer->contador <= 0)
    {
        max_retries--;
        printf("\033[41m\033[30m[AVISO CONS]  Impossibilitado de se consumir item, pois o buffer está vazio.\033[0m\n");
        usleep(10000); // 10000 microssegundos de pausa
        pthread_testcancel(); // Permite que a thread seja cancelada enquanto espera
    }
    
    printf("[INFO CONSUMIDOR] Status do debito que irá ser consumido: %d\n", buffer->itens[buffer->out].status);
    if(buffer->itens[buffer->out].status == 1)
    {
        printf("\033[41m\033[30m[AVISO CONSUMIDOR]: Tentativa de consumir débito já processado (ID %d).\033[0m\n", buffer->itens[buffer->out].id_transacao);
    }

    Debito debito_consumido = buffer->itens[buffer->out];
    buffer->itens[buffer->out].status = 1; // Marca como consumido (opcional)
    buffer->out = (buffer->out + 1) % TAMANHO_MAXIMO;
    // SIMULA A INTERRUPÇÃO NO MEIO DA LEITURA/ESCRITA DO CONTADOR
    int temp_contador = buffer->contador; // Lê
    usleep(1); // ATRAZO FORÇADO
    buffer->contador = temp_contador - 1; //escreve

    printf("[INFO CONSUMIDOR] Item consumido do buffer: ID Transação %d, Valor %.2f\n\n", debito_consumido.id_transacao, debito_consumido.valor);;
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
    printf("Item foi produzido no buffer com controle.\n");

    //4. DESBLOQUEIA O MUTEX
    pthread_mutex_unlock(&mutex_buffer);

    //5. SINALIZA O SEMÁFORO DE ITENS DISPONÍVEIS
    sem_post(sem_cheio);
    printf("Item SEG: ID %d produzido. Contador: %d\n", debito.id_transacao, buffer->contador);
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
    
    printf("Item SEG: ID %d consumido. Contador: %d\n", item_consumido.id_transacao, buffer->contador);
    
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
    
    printf("Buffer interno resetado e memória limpa.\n");
    
    // NOTA: O 'free(buffer)' deve ser chamado no arquivo runner (produtorXconsumidor.c)
    // porque foi lá que a memória foi alocada com malloc.
}