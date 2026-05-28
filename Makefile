# Ensamblador, compilador de C y enlazador
ASM = nasm
CC = gcc
LD = gcc
CARGO = cargo

# Banderas
ASMFLAGS = -f elf64
CFLAGS = -Wall -Wextra -O2 -I$(INC_DIR) -D_GNU_SOURCE
# -no-pie se suele requerir para evitar problemas con código independiente de posición en algunos sistemas modernos
LDFLAGS = -no-pie -lSDL2 -lpthread -ldl -lm

# Directorios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Archivos fuente y objetos (Híbridos C / ASM)
ASM_SRCS = $(wildcard $(SRC_DIR)/*.asm)
C_SRCS   = $(wildcard $(SRC_DIR)/*.c)

ASM_OBJS = $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%_asm.o, $(ASM_SRCS))
C_OBJS   = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%_c.o, $(C_SRCS))

OBJS = $(ASM_OBJS) $(C_OBJS)
RUST_LIB = rust_core/target/release/libspace_invaders_core.a
TARGET = space_invaders

# Regla principal
.PHONY: all clean

all: $(TARGET)

# Enlace final
$(TARGET): $(OBJS) $(RUST_LIB)
	$(LD) -o $@ $^ $(LDFLAGS)

# Compilación Rust
$(RUST_LIB): rust_core/src/*.rs rust_core/Cargo.toml
	cd rust_core && $(CARGO) build --release

# Ensamblado de archivos ASM
$(OBJ_DIR)/%_asm.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) -I$(INC_DIR)/ -o $@ $<

# Compilación de archivos C
$(OBJ_DIR)/%_c.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

# Creación de directorio obj/ si no existe
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpiar archivos generados
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	cd rust_core && $(CARGO) clean
