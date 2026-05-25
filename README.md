# 👾 Space Invaders - Pure x86_64 Assembly (Modular)

Un clon modular del clásico juego **Space Invaders** escrito completamente en **ensamblador x86_64** para Linux, utilizando la biblioteca **SDL2** para el renderizado de gráficos 2D y la gestión de entradas de teclado. El proyecto está estructurado de manera modular y sigue rigurosamente las especificaciones de la **System V AMD64 ABI** para garantizar un alto rendimiento y estabilidad de memoria.

---

## 🚀 Características del Proyecto

* **Modularidad Real**: Código dividido en módulos especializados (físicas, entrada, renderizado, jugador, enemigos e interfaz de usuario).
* **Estabilidad Robusta**: Stack 100% alineado a 16 bytes y preservación estricta de registros de CPU (`RBX`, `R12`-`R15`) según la convención de llamadas de Linux, eliminando errores de violación de segmento.
* **Inteligencia y Movimiento Independiente**:
  * Los enemigos ya no se mueven en una cuadrícula rígida; cada uno tiene asignado un patrón de movimiento individual (diagonal con rebote o trayectoria senoidal).
  * **Ataques en Picada (Diving System)**: Cada ~2 segundos, un enemigo en formación se desprende de forma independiente y persigue agresivamente al jugador a gran velocidad (comportamiento Kamikaze).
  * **Generación Gradual en Cascada**: Los enemigos aparecen secuencialmente uno a uno, materializándose en la pantalla en lugar de aparecer todos de golpe.
  * **Reciclado de Enemigos (Screen Wrap)**: Si un enemigo en picada cruza la parte inferior, reaparece dinámicamente en el cielo en una posición aleatoria para reincorporarse al combate.
* **Progresión Equilibrada**: Cantidad inicial de enemigos reducida a 6 en la primera oleada, con un incremento dinámico y progresivo de dificultad y tamaño de rejilla en cada nivel consecutivo.
* **Interfaz Retro**: Dibujado del puntaje (score) en tiempo real mediante renderizado de mapas de bits de fuente pixel-art de 5x5 píxeles escrita a mano en ASM.

---

## 📂 Arquitectura del Código

El código fuente se encuentra organizado dentro de la carpeta `src/` y las cabeceras en `include/`:

* **[src/main.asm](src/main.asm)**: El punto de entrada del programa. Inicializa los sistemas y coordina el bucle de juego principal (`game_loop`).
* **[src/video.asm](src/video.asm)**: Inicialización del subsistema de video de SDL2, creación de la ventana y el renderer, y dibujado de cada fotograma (limpieza de pantalla, jugador, balas y enemigos).
* **[src/enemies.asm](src/enemies.asm)**: Lógica principal de la IA alienígena. Maneja el spawn en cascada, cálculo de rejillas dinámicas, temporizadores de ataques kamikaze y el algoritmo de reciclado/wrapping.
* **[src/player.asm](src/player.asm)**: Control de la posición de la nave del jugador, límites de pantalla, enfriamiento (cooldown) y disparo simultáneo de hasta 5 proyectiles activos.
* **[src/physics.asm](src/physics.asm)**: Detección de colisiones mediante rectángulos delimitadores (AABB) entre balas-enemigos y enemigos-jugador, gestionando vidas y fin del juego.
* **[src/input.asm](src/input.asm)**: Manejo de eventos de SDL y lectura en tiempo real del mapa de estado del teclado.
* **[src/ui.asm](src/ui.asm)**: Extrae los dígitos del puntaje y los dibuja en pantalla usando una tabla de búsqueda (LUT) de caracteres pixelados personalizados.
* **[include/game.inc](include/game.inc)**: Cabecera maestra que define constantes de juego (dimensiones, velocidades, offsets de estructura) y declaraciones `extern` compartidas entre los módulos.

---

## 🛠️ Requisitos de Instalación

Para compilar y ejecutar este juego en tu sistema Linux (distribuciones basadas en Debian/Ubuntu/Arch), necesitarás instalar el ensamblador NASM, GCC y las librerías de desarrollo de SDL2:

### En Debian/Ubuntu:
```bash
sudo apt update
sudo apt install nasm build-essential libsdl2-dev
```

### En Arch Linux / Manjaro:
```bash
sudo pacman -S nasm base-devel sdl2
```

---

## ⚙️ Compilación y Ejecución

El proyecto incluye un `Makefile` automatizado para realizar construcciones limpias y eficientes:

1. **Compilar el Proyecto**:
   Genera el archivo ejecutable binario `space_invaders` y los archivos objeto intermedios en la carpeta `obj/`:
   ```bash
   make
   ```

2. **Ejecutar el Juego**:
   Puedes ejecutar el binario directamente:
   ```bash
   ./space_invaders
   ```
   O utilizar el script bash automatizado que limpia, compila y arranca el juego en un solo paso:
   ```bash
   chmod +x run.sh
   ./run.sh
   ```

3. **Limpiar Archivos Generados**:
   Elimina el ejecutable y todos los archivos objeto compilados para forzar una reconstrucción limpia:
   ```bash
   make clean
   ```

---

## 🎮 Controles del Juego

* **Flecha Izquierda (`←`)**: Mueve la nave hacia la izquierda.
* **Flecha Derecha (`→`)**: Mueve la nave hacia la derecha.
* **Barra Espaciadora (`Space`)**: Dispara proyectiles láser (máximo 5 proyectiles en pantalla a la vez).
* **Cerrar Ventana / Alt + F4**: Sale de la partida de forma segura liberando los recursos de memoria.

---

## 📝 Licencia

Este proyecto es de código abierto y está libre para propósitos de práctica, estudio de arquitectura de computadores x86_64 y programación a bajo nivel en Linux.
