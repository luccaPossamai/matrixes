# Compilador e flags
CXX = g++
CXXFLAGS = -O3 -std=c++17 -Iincludes

# Caminho de compilação (path atual)
COMPILE_DIR := $(shell pwd)
CXXFLAGS += -DCOMPILE_DIR="\"$(COMPILE_DIR)\""

# Executável final
NAME = matrix

# Diretórios
OBJ_DIR = build

# Fontes na raiz
SRCS = $(wildcard *.cpp)

# Transforma foo.cpp → build/foo.o
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Bibliotecas externas (Raylib + Linux)
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

.PHONY: all clean run

all: $(NAME)

# Linkagem
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Compilação de cada .cpp → build/.o
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(NAME)
	./$(NAME)

clean:
	rm -rf $(OBJ_DIR) $(NAME)
