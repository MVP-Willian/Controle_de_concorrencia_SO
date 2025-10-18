//
// Created by michaelwpv on 16/10/2025.
//

#ifndef DEBITO_H
#define DEBITO_H
#include "theme.h"

typedef struct
{
    int id_transacao;
    ContaBancaria conta_origem;
    ContaBancaria conta_destino;
    double valor;

} Debito;


Debito* inicializa_debito(const int id_transacao, const ContaBancaria* conta_origem, const ContaBancaria* conta_destino, double valor);
void executa_debito(Debito* debito);
void finaliza_debito(Debito* debito);

#endif 