//
// Created by michaelwpv on 16/10/2025.
//

#include "../include/debito.h"
#include "../include/theme.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

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

    novo_debito->conta_origem = *conta_origem;  
    novo_debito->conta_destino = *conta_destino;
    novo_debito->valor = valor;

    return novo_debito;
}


