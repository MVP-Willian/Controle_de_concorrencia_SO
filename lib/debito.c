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

void executa_debito(Debito* debito)
{
    if(debito == NULL)
    {
        fprintf(stderr, "Erro: Débito é NULL.\n");
        return;
    }

    if(debito->conta_origem.saldo < debito->valor)
    {
        fprintf(stderr, "Erro: Saldo insuficiente na conta de origem.\n");
        return;
    }

    debito->conta_origem.saldo -= debito->valor;
    debito->conta_destino.saldo += debito->valor;

    printf("Débito executado com sucesso: %.2f transferidos da conta %d para a conta %d.\n",
           debito->valor,
           debito->conta_origem.numero_conta,
           debito->conta_destino.numero_conta);
}


