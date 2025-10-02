# Trabalho Prático 1 de Sistemas Operacionais - 2025/2

## Como Rodar

### Compilar e Executar

> Por enquanto, rodar o programa não será possível, até que todas as bibliotecas estejam funcionais.
> Isto é, quando terminarmos qualquer um dos módulos de Concorrência.

### Rodada de Testes

> Ainda será aprimorado para que cada biblioteca tenha um teste único, que pode ser testado com
> `make run-test <biblioteca>` ou `make run-test` para testar tudo.

Todos os códigos de teste estão na pasta `/tests`.
Para executá-los, basta rodar o comando `make build-tests` para compilar e executar `make run-tests`.

## Documentação

### Estrutura do Projeto

```text
├── bin/
│   ├── consumidor_produtor.o
│   ├── leitor_escritor.o
│   └── test_<biblioteca>.o
├── include/
│   └── <biblioteca>.h
├── lib/
│   └── <biblioteca>.c
├── src/
│   ├── consumidor_produtor.o
│   └── leitor_escritor.o
├── tests/
│   └── <biblioteca>.test.c
├── Makefile
└── README.md
```

### Descrição dos Componentes

* `bin/` - Contém os executáveis finais gerados pelo processo de compilação. É o diretório de "saída" do projeto.

* `include/` - Contém todos os arquivos de cabeçalho (.h). Eles definem as interfaces públicas (as "assinaturas" das
  funções) para as bibliotecas e módulos, servindo como um "contrato" de como o código deve ser utilizado.

* `lib/` - Armazena o código-fonte (.c) de bibliotecas reutilizáveis. Estas são funcionalidades genéricas e modulares (como
  o arg_parser.c) que dão suporte à aplicação principal, mas não fazem parte de sua lógica central.

* `src/` - Guarda o código-fonte (.c) da aplicação principal. É aqui que reside a lógica de negócio do projeto, incluindo o
  ponto de entrada main.c.

* `tests/` - Contém o código-fonte dos testes (.test.c). Cada arquivo aqui é um programa independente que serve para
  verificar o correto funcionamento de uma biblioteca específica do diretório lib/.

### Como Escrever um Teste?

Para garantir a qualidade das bibliotecas em `lib/`, siga estes passos para criar um novo teste unitário.

1.  **Crie o Arquivo de Teste**
    -   Para cada biblioteca (ex: `lib/utils.c`), crie um arquivo correspondente no diretório `tests/` (ex: `tests/utils.test.c`).

2.  **Escreva a Função de Teste**
    -   Siga o padrão **AAA (Arrange, Act, Assert)**:
        -   **Arrange (Organizar):** Prepare as variáveis e dados de entrada.
        -   **Act (Agir):** Chame a função da biblioteca que será testada.
        -   **Assert (Verificar):** Use `assert()` para garantir que o resultado foi o esperado.
    -   A função de teste deve ter um nome descritivo (iniciado com `test_`) e retornar `1` em caso de sucesso.

3.  **Execute a Função no Test Runner**
    -   Dentro da função `main()` do seu arquivo de teste, adicione seu novo teste usando a macro `RUN_TEST(nome_da_sua_funcao)`.

4.  **Atualize o `Makefile`**
    -   Adicione uma nova regra ao `Makefile` para que ele saiba como compilar seu novo teste e incluí-lo no comando `make test`.

[Clique aqui para acessar o docs](https://docs.google.com/document/d/1y3xCDTN5BQ304H-XjCmLYPSic5DiraAq5Xw-LXptIdw/edit?usp=sharing)
