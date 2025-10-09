//
// Created by luizg on 09/10/2025.
//

#ifndef THEME_H
#define THEME_H

typedef struct
{
    int numero_conta;
    double saldo;
    char titular[100];
} ContaBancaria;

ContaBancaria* inicializa_conta(const int numero_conta, const double saldo, const char* titular);

void destroi_conta(ContaBancaria* conta);

#endif //THEME_H