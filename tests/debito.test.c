//
// Created by michaelwpv on 17/10/2025.
//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/buffer.h"
#include "../include/tests.h"

int test_init()
{
    ContaBancaria conta_principal = 
    {
        .numero_conta = 12345,
        .saldo = 1500.50,
        .titular = "Titular Teste Origem"
    };

    ContaBancaria conta_recebedora = 
    {
        .numero_conta = 67890,
        .saldo = 75.00,
        .titular = "Titular Teste Destino"
    };

    double valor_debito_teste = 45.00;
    int id_debito_teste = 1001;

    Debito* novo_debito = inicializa_debito
    (
        id_debito_teste,
        &conta_principal,
        &conta_recebedora,
        valor_debito_teste
    );

    if(novo_debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito de teste.\n");
        assert(0);
        return 0;
    }
    
    if(novo_debito->id_transacao != id_debito_teste ||
       novo_debito->conta_origem.numero_conta != conta_principal.numero_conta ||
       novo_debito->conta_destino.numero_conta != conta_recebedora.numero_conta ||
       novo_debito->valor != valor_debito_teste)
    {
        fprintf(stderr, "Dados do débito inicializado não correspondem aos valores esperados.\n");
        free(novo_debito);
        assert(0);
        return 0;
    }
    else
    {
        printf("Teste de inicialização de débito passou com sucesso.\n");
        printf("Valor: %.2f | Origem: %s\n", novo_debito->valor, novo_debito->conta_origem.titular);
    }

    free(novo_debito);
    assert(1);
    return 1;
}

int test_execucao_debito()
{
    ContaBancaria conta_principal = 
    {
        .numero_conta = 12345,
        .saldo = 1500.50,
        .titular = "Titular Teste Origem"
    };  
    ContaBancaria conta_recebedora = 
    {
        .numero_conta = 67890,
        .saldo = 75.00,
        .titular = "Titular Teste Destino"
    };
    double valor_debito_teste = 45.00;
    int id_debito_teste = 1001;

    Debito* novo_debito = inicializa_debito
    (
        id_debito_teste,
        &conta_principal,
        &conta_recebedora,
        valor_debito_teste
    );
    if(novo_debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito de teste para execução.\n");
        assert(0);
        return 0;
    }
    executa_debito(novo_debito);
    if(novo_debito->conta_origem.saldo != (1500.50 - valor_debito_teste) ||
       novo_debito->conta_destino.saldo != (75.00 + valor_debito_teste))
    {
        fprintf(stderr, "Erro na execução do débito: saldos não atualizados corretamente.\n");
        free(novo_debito);
        assert(0);
        return 0;
    }
    else
    {
        printf("Teste de execução de débito passou com sucesso.\n");
        printf("Novo saldo origem: %.2f | Novo saldo destino: %.2f\n",
               novo_debito->conta_origem.saldo,
               novo_debito->conta_destino.saldo);
    }
    free(novo_debito);
    assert(1);
    return 1;

}

int test_finaliza_debito()
{
    ContaBancaria conta_principal = 
    {
        .numero_conta = 12345,
        .saldo = 1500.50,
        .titular = "Titular Teste Origem"
    };  
    ContaBancaria conta_recebedora = 
    {
        .numero_conta = 67890,
        .saldo = 75.00,
        .titular = "Titular Teste Destino"
    };
    double valor_debito_teste = 45.00;
    int id_debito_teste = 1001;

    Debito* novo_debito = inicializa_debito
    (
        id_debito_teste,
        &conta_principal,
        &conta_recebedora,
        valor_debito_teste
    );
    if(novo_debito == NULL)
    {
        fprintf(stderr, "Erro ao inicializar débito de teste para finalização.\n");
        assert(0);
        return 0;
    }
    finaliza_debito(novo_debito);
    printf("Teste de finalização de débito passou com sucesso.\n");
    assert(1);
    return 1;
}

int main()
{
    printf("--- Iniciando testes para débito ---\n");

    RUN_TEST(test_init);
    RUN_TEST(test_execucao_debito);
    RUN_TEST(test_finaliza_debito);

    printf("----------------------------------------\n");
}