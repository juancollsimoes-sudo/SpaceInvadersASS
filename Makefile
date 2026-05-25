# Ensamblador y enlazador
ASM = nasm
# Usamos gcc como enlazador para facilitar la vinculación con las librerías dinámicas de C como SDL2
LD = gcc

# Banderas
ASMFLAGS = -f elf64
# -no-pie se suele requerir para evitar problemas con código independiente de posición en algunos sistemas modernos
LDFLAGS = -no-pie -lSDL2

# Directorios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Archivos fuente y objetos
SRCS = $(wildcard $(SRC_DIR)/*.asm)
OBJS = $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = space_invaders

# Regla principal
.PHONY: all clean

all: $(TARGET)

# Enlace final
$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)

# Ensamblado
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) -I$(INC_DIR)/ -o $@ $<

# Creación de directorio obj/ si no existe
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpiar archivos generados
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
