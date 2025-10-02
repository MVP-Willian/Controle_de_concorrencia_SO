# Diretórios
INCLUDE_DIR := include
LIB_DIR 	:= lib
SRC_DIR 	:= src
TEST_DIR 	:= tests
BIN_DIR 	:= bin

# Flags e Configuração do Compilador
CC 			:= gcc
CFLAGS 		:= -Wall -g -O2
INCLUDES 	:= -I$(INCLUDE_DIR)

# Encontrando todos os arquivos
LIB_FILES 		:= $(wildcard $(LIB_DIR)/*.c)
SRC_FILES 		:= $(wildcard $(SRC_DIR)/*.c)
TEST_SRC_FILES 	:= $(wildcard $(TEST_DIR)/*.c)

# Gerando os nomes para cada executável
TEST_EXEC := $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/tests/%.o, $(TEST_SRC_FILES))
MAIN_EXEC := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRC_FILES))

.PHONY: all build build-tests run-testes clean

all: build

build: $(MAIN_EXEC)
	@echo "Programas principais compilados com sucesso em $(BIN_DIR)/"

build-tests: $(TEST_EXEC)
	@echo "Testes compilados com sucesso em $(BIN_DIR)/tests"

run-tests: build-tests
	@if [ -z "$(lib)" ]; then \
  		echo "--- Rodando TODOS os testes ---"; \
  		for test in $(TEST_EXEC); do \
  			echo ""; \
  			echo "==> Executando $$test <=="; \
  			./$$test; \
  		done; \
  		echo ""; \
  		echo "--- Todos os testes foram executados ---"; \
	else \
		echo ""; \
		echo "--- Rodando teste específico para $(lib) ---"; \
		./$(BIN_DIR)/tests/$(lib).test.o; \
	fi

clean:
	@echo "Limpando arquivos gerados..."
	@rm -rf $(BIN_DIR)

$(MAIN_EXEC) $(TEST_EXEC): | $(BIN_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)/tests

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c $(LIB_FILES)
	@echo "Compilando programa: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(BIN_DIR)/tests/%.o: $(TEST_DIR)/%.c $(LIB_FILES)
	@echo "Compilando teste: $@"
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@