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
    printf("Buffer inicializado com sucesso.");

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
    printf("Item foi produzido no buffer");
};


/*void destruir_buffer(BufferCompartilhado *buffer);*/

/*void consumir_item(BufferCompartilhado *buffer);*/