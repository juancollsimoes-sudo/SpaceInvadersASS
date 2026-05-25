#!/bin/bash

# Salir si ocurre un error
set -e

echo "🚀 Iniciando configuración del proyecto 'Space Invaders' en x86_64 Assembly..."

# 1. Crear estructura de carpetas
echo "📁 Creando directorios: src, include, assets..."
mkdir -p src include assets

# 2. Crear archivo Makefile
echo "🛠️ Generando Makefile..."
cat << 'EOF' > Makefile
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
EOF

# 3. Crear el script run.sh
echo "▶️ Generando script run.sh..."
cat << 'EOF' > run.sh
#!/bin/bash

echo "🔨 Compilando el proyecto..."
make

if [ $? -eq 0 ]; then
    echo "✅ Compilación exitosa. Ejecutando el juego..."
    ./space_invaders
else
    echo "❌ Error en la compilación. Revisa los mensajes anteriores."
fi
EOF

# Dar permisos de ejecución a los scripts
chmod +x run.sh

echo ""
echo "🎉 ¡Estructura base del proyecto completada!"
echo "Ahora puedes ejecutar tu juego usando: ./run.sh"
echo "Aviso: Como aún no hay código en src/, el Makefile dará un error al compilar, esto es normal por ahora."
