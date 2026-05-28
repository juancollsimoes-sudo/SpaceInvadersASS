# 👾 Space Invaders - Hybrid C/ASM/Rust (Modular)

Un clon modular del clásico juego **Space Invaders** con una arquitectura híbrida de alto rendimiento. Combina la velocidad extrema del **ensamblador x86_64** puro, la estructura en **C** y la seguridad y ecosistema moderno de **Rust**. Utiliza **SDL2** para el renderizado de gráficos 2D y el teclado, interactuando fluidamente a través de la **System V AMD64 ABI**.

---

## 🚀 Características del Proyecto

* **Arquitectura Híbrida (C/ASM/Rust)**: 
  * **C**: Inicialización de ventana, bucle de eventos y control de vida útil de SDL2.
  * **Ensamblador (x86_64)**: Motor de renderizado súper rápido, detección de colisiones de bajo nivel y actualización de matrices en memoria.
  * **Rust**: IA procedimental, motor asíncrono de audio, parseo de configuraciones y persistencia segura de datos. Todo compilado como una librería estática FFI (`libspace_invaders_core.a`).
* **Audio Asíncrono (Rodio + MPSC)**: Sistema de sonido en Rust libre de bloqueos. Los eventos de audio (disparos, explosiones) se envían por canales a un hilo dedicado, evitando retrasos en los fotogramas de ASM.
* **Persistencia Cifrada**: Guarda y carga el "High Score" en un archivo local (`high_score.dat`) utilizando un cifrado XOR seguro implementado en Rust.
* **Configuración TOML**: El juego lee dinámicamente un archivo `config.toml` (vía Rust) al inicio, permitiendo modificar en tiempo real la velocidad del jugador y los enemigos sin recompilar el código ASM.
* **IA Procedimental Avanzada**:
  * Los cálculos complejos de trayectorias (Kamikaze, senoidales, diagonales) han sido exportados a Rust.
  * **Ataques en Picada (Diving System)**: Enemigos independientes atacan al jugador periódicamente.
* **Estabilidad Robusta**: Stack alineado a 16 bytes y preservación estricta de registros volátiles y no-volátiles para permitir las complejas llamadas bidireccionales entre ASM y Rust/C.

---

## 📂 Arquitectura del Código

El código fuente está dividido por ecosistemas:

### Código de Bajo Nivel (C / ASM)
* **`src/main.asm` / `src/video.asm` / `src/input.asm`**: Motor principal y bucles de renderizado con SDL2.
* **`src/enemies.asm` / `src/player.asm` / `src/physics.asm`**: Memoria y dibujo de sprites, llamadas FFI a los módulos de Rust (por ej. `rust_update_enemy` y `rust_play_sound`).
* **`src/menu.c` / `src/difficulty.c`**: Lógica de menús y progresión.

### Core de Alto Nivel (Rust)
Carpeta `rust_core/src/`:
* **`lib.rs`**: Interfaz de exportación de C (`#[no_mangle] extern "C"`).
* **`ai.rs`**: Motor procedimental que calcula y actualiza estructuras de enemigos de 32-bytes directamente en memoria.
* **`audio.rs`**: Hilo de sonido asíncrono gestionado mediante la crate `rodio` interactuando con ALSA.
* **`config.rs`**: Deserializador de TOML para la configuración dinámica inyectada al vuelo.
* **`persistence.rs`**: Lector/Escritor I/O en disco para los puntajes.

---

## 🛠️ Requisitos de Instalación

Para compilar y ejecutar este juego híbrido en Linux (Debian/Ubuntu/Arch), necesitarás instalar el compilador de Rust (`cargo`), NASM, GCC y las librerías de SDL2 y ALSA:

### En Debian/Ubuntu:
```bash
sudo apt update
sudo apt install nasm build-essential libsdl2-dev libasound2-dev curl
# Instalar Rust (si no lo tienes)
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

### En Arch Linux / Manjaro:
```bash
sudo pacman -S nasm base-devel sdl2 alsa-lib rustup
rustup default stable
```

---

## ⚙️ Compilación y Ejecución

El proyecto incluye un `Makefile` automatizado que orquesta la compilación de la librería de Rust (`cargo build`), el ensamblado (`nasm`) y el enlazado estático final (`gcc`):

1. **Compilar el Proyecto**:
   ```bash
   make
   ```

2. **Ejecutar el Juego**:
   Asegúrate de que exista un archivo `config.toml` válido en el mismo directorio (Rust lo leerá al inicio):
   ```bash
   ./space_invaders
   ```

3. **Limpiar Archivos Generados**:
   ```bash
   make clean
   ```

---

## 🎮 Controles del Juego

* **Flecha Izquierda (`←`)**: Mueve la nave hacia la izquierda.
* **Flecha Derecha (`→`)**: Mueve la nave hacia la derecha.
* **Barra Espaciadora (`Space`)**: Dispara proyectiles láser.
* **Cerrar Ventana / Alt + F4**: Sale del juego guardando tu puntaje de forma segura a través de Rust.

---

## 📝 Licencia

Proyecto de código abierto. Un excelente caso de estudio para quienes deseen aprender sobre interoperabilidad de memorias extremas (FFI), llamadas de la ABI System V y cómo un lenguaje moderno como Rust puede dotar a un motor Assembly puro de capacidades modernas de I/O y asincronía.
