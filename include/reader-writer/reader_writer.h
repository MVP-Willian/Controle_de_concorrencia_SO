//
// Created by luizg on 09/10/2025.
//

#ifndef READER_WRITER_H
#define READER_WRITER_H

#include "../theme.h"

#define READER "LEITOR"
#define WRITER "ESCRITOR"

typedef struct
{
    int thread_id;
    ContaBancaria *conta;
    double valor_operacao;
} ThreadArgs;

#endif //READER_WRITER_H
