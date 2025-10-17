//
// Created by michael on 17/10/2025
//

#include "../include/buffer.h"
#include "../include/debito.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>




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

    memset(buffer->itens, 0, TAMANHO_MAXIMO * sizeof(Debito));
    printf("Buffer inicializado com sucesso.\n");

}

void produzir_item_sem_controle(BufferCompartilhado *buffer, Debito debito)
{
    while(buffer->contador>=TAMANHO_MAXIMO)
    {
        printf("Impossibilitado de se produzir item, pois o número máximo %d itens do Buffer foi atingido.\n", buffer->contador);
        return;
    }
    buffer->itens[buffer->in] = debito;
    buffer->in = (buffer->in + 1) % TAMANHO_MAXIMO;
    buffer->contador++;
    printf("Item foi produzido no buffer.\n");
};


/*void destruir_buffer(BufferCompartilhado *buffer);*/

Debito consumir_item_sem_controle(BufferCompartilhado *buffer)
{
    //1. CHECAGEM INSEGURA/ESPERA OCUPADA (BUSY WAITING)
    while(buffer->contador <= 0)
    {
        printf("Impossibilitado de se consumir item, pois o buffer está vazio.\n");
    }

    Debito debito_consumido = buffer->itens[buffer->out];
    buffer->out = (buffer->out + 1) % TAMANHO_MAXIMO;
    buffer->contador--;

    printf("Item consumido do buffer: ID Transação %d, Valor %.2f\n", debito_consumido.id_transacao, debito_consumido.valor);
    return debito_consumido;

    /*
    if(buffer->contador <= 0)
    {
        fprintf(stderr, "Erro: Buffer vazio, não é possível consumir item.\n");
        return;
    }

    Debito debito_consumido = buffer->itens[buffer->out];
    buffer->out = (buffer->out + 1) % TAMANHO_MAXIMO;
    buffer->contador--;

    printf("Item consumido do buffer: ID Transação %d, Valor %.2f\n", debito_consumido.id_transacao, debito_consumido.valor);
    */
};