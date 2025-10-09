//
// Created by luizg on 09/10/2025.
//

#include "../include/theme.h"

#include <stdlib.h>
#include <string.h>

ContaBancaria* inicializa_conta(const int numero_conta, const double saldo, const char* titular)
{
    ContaBancaria* conta = (ContaBancaria*)malloc(sizeof(ContaBancaria));

    if (conta)
    {
        conta->numero_conta = numero_conta;
        conta->saldo = saldo;
        strncpy(conta->titular, titular, 99);
        conta->titular[99] = '\0';
        return conta;
    }

    return NULL;
}

void destroi_conta(ContaBancaria* conta)
{
    if (conta)
    {
        free(conta);
    }
}
