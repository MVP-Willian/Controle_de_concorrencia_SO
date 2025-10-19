//
// Created by michaelwpv on 16/10/2025.
//

#include "../include/debito.h"
#include "../include/theme.h"

#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

extern pthread_mutex_t contas_mutex; // declarar em sync.h / sync.c

//Implementação
Debito* inicializa_debito(const int id_transacao, const ContaBancaria* conta_origem, const ContaBancaria* conta_destino, double valor)
{
    Debito* novo_debito = (Debito*) malloc (sizeof(Debito));
    if(novo_debito == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória para novo débito.\n");
        return NULL;
    }
    novo_debito->id_transacao = id_transacao;

    if(conta_origem == NULL || conta_destino == NULL)
    {
        fprintf(stderr, "Erro: Conta de origem ou destino é NULL.\n");
        free(novo_debito);
        return NULL;
    }

    novo_debito->conta_origem = (ContaBancaria*) conta_origem;  
    novo_debito->conta_destino = (ContaBancaria*) conta_destino;
    novo_debito->valor = valor;
    novo_debito->status = 0; // Pendente

    return novo_debito;
}

void executa_debito(Debito* debito)
{
    if(debito == NULL)
    {
        fprintf(stderr, "Erro: Débito é NULL.\n");
        return;
    }
    
    if(debito->conta_origem == NULL || debito->conta_destino == NULL){
        fprintf(stderr, "[AVISO CONTA] Erro: contas no débito são NULL.\n");
        return;
    }

    // Protege alteração das contas:
    pthread_mutex_lock(&contas_mutex);

    if(debito->conta_origem->saldo < debito->valor)
    {
        fprintf(stderr, "[AVISO CONTA] Erro: Saldo insuficiente na conta de origem.\n");
        return;
    }

    debito->conta_origem->saldo -= debito->valor;
    debito->conta_destino->saldo += debito->valor;
    debito->status = 1; // Concluído

    pthread_mutex_unlock(&contas_mutex);
  
}

void finaliza_debito(Debito* debito)
{
    if(debito == NULL)
    {
        fprintf(stderr, "Erro: Débito é NULL.\n");
        return;
    }

    free(debito);
}

