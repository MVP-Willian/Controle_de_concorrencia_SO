# Diretórios
INCLUDE_DIR := include
LIB_DIR 	:= lib
SRC_DIR 	:= src
TEST_DIR 	:= tests
BIN_DIR 	:= bin

TEST_BIN_DIR := $(BIN_DIR)/tests

# Flags e Configuração do Compilador
CC 			:= gcc
CFLAGS 		:= -Wall -g -O2 -pthread
INCLUDES 	:= -I$(INCLUDE_DIR)

# Encontrando todos os arquivos
LIB_FILES 		:= $(shell find $(LIB_DIR) -name '*.c' -type f)
SRC_FILES 		:= $(shell find $(SRC_DIR) -name '*.c' -type f)
TEST_SRC_FILES 	:= $(shell find $(TEST_DIR) -name '*.test.c' -type f)

# Gerando os nomes para cada executável
BASENAMES := $(notdir $(TEST_SRC_FILES))
TEST_EXEC_TARGETS := $(addprefix $(TEST_BIN_DIR)/, $(BASENAMES:%.test.c=%.test))
MAIN_EXEC_TARGETS := $(addprefix $(BIN_DIR)/, $(notdir $(SRC_FILES:%.c=%)))
#TEST_EXEC_TARGETS := $(patsubst $(TEST_DIR)/%.teste.c, $(BIN_DIR)/tests/%.test, $(TEST_SRC_FILES))
#MAIN_EXEC_TARGETS := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRC_FILES))


DIRS_TO_CREATE := $(TEST_BIN_DIR)

.PHONY: all build build-tests run-testes clean debug


all: build

# Regra genérica para criar diretórios necessários
$(DIRS_TO_CREATE):
	@mkdir -p $@

build: $(DIRS_TO_CREATE) $(MAIN_EXEC_TARGETS)
	@echo "Programas principais compilados com sucesso em $(BIN_DIR)/"

debug:
	@echo "--- Depurando Variáveis ---"
	@echo "Diretório de Testes (TEST_BIN_DIR): $(TEST_BIN_DIR)"
	@echo "Arquivos de Teste Encontrados (TEST_SRC_FILES): $(TEST_SRC_FILES)"
	@echo "Alvos de Executáveis de Teste (TEST_EXEC_TARGETS): $(TEST_EXEC_TARGETS)"
	@echo "--------------------------"

build-tests: $(DIRS_TO_CREATE) $(TEST_EXEC_TARGETS)
	@echo "Testes compilados com sucesso em $(TEST_BIN_DIR)"

run-tests: build-tests
	@if [ -z "$(lib)" ]; then \
  		echo "--- Rodando TODOS os testes ---"; \
  		for test_file in $(TEST_EXEC_TARGETS); do \
  			echo ""; \
  			echo "==> Executando $$test <=="; \
  			$$test_file; \
  		done; \
  		echo ""; \
  		echo "--- Todos os testes foram executados ---"; \
	else \
		echo ""; \
		echo "--- Rodando teste específico para $(lib) ---"; \
		./$(TEST_BIN_DIR)/tests/$(lib).test; \
	fi

clean:
	@echo "Limpando arquivos gerados..."
	@rm -rf $(BIN_DIR)

$(MAIN_EXEC) $(TEST_EXEC_TARGETS): | $(BIN_DIR)

$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_FILES)
	@echo "Compilando programa: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@


$(TEST_BIN_DIR)/%.test: $(TEST_DIR)/%.test.c $(LIB_FILES)
	@echo "Compilando teste: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@