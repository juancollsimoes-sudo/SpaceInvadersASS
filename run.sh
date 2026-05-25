#!/bin/bash

echo "🔨 Compilando el proyecto..."
make

if [ $? -eq 0 ]; then
    echo "✅ Compilación exitosa. Ejecutando el juego..."
    ./space_invaders
else
    echo "❌ Error en la compilación. Revisa los mensajes anteriores."
fi
