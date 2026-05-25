#include "input_time.h"
#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

int32_t verificar_teclado(void) {
    int32_t mask = 0;
    
    // Recupera el puntero de estado del teclado administrado por SDL2.
    // La convención en el bucle de juego es llamar a esta función después de
    // bombear/procesar los eventos de la ventana (SDL_PollEvent) en ASM.
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (!state) {
        return 0;
    }
    
    // Escaneo de tecla de movimiento a la IZQUIERDA (Flecha izquierda o tecla 'A')
    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
        mask |= KEY_LEFT;
    }
    
    // Escaneo de tecla de movimiento a la DERECHA (Flecha derecha o tecla 'D')
    if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
        mask |= KEY_RIGHT;
    }
    
    // Escaneo de tecla de DISPARO (Barra espaciadora)
    if (state[SDL_SCANCODE_SPACE]) {
        mask |= KEY_SPACE;
    }
    
    // Escaneo de tecla de SALIDA (Tecla Escape)
    if (state[SDL_SCANCODE_ESCAPE]) {
        mask |= KEY_QUIT;
    }
    
    return mask; // El valor de retorno entero de 32 bits se coloca en EAX/RAX
}

void regular_framerate(int32_t fps) {
    if (fps <= 0) {
        return;
    }
    
    // Mantener estado estático de la medición del tiempo entre invocaciones
    static struct timespec last_time;
    static int initialized = 0;
    
    // Tiempo objetivo por fotograma en microsegundos (ej: 60 FPS -> 16,666 µs)
    long target_us = 1000000L / fps;
    struct timespec current_time;
    
    if (!initialized) {
        // En la primera llamada, capturamos el tiempo base y salimos
        clock_gettime(CLOCK_MONOTONIC, &last_time);
        initialized = 1;
        return;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    
    // Calcular el tiempo transcurrido en microsegundos (segundos * 1M + nanosegundos / 1000)
    long elapsed_us = (current_time.tv_sec - last_time.tv_sec) * 1000000L +
                      (current_time.tv_nsec - last_time.tv_nsec) / 1000L;
                      
    long sleep_us = target_us - elapsed_us;
    
    if (sleep_us > 0) {
        // Dormimos pasivamente liberando la CPU a otros procesos del sistema
        usleep((useconds_t)sleep_us);
    }
    
    // Re-registrar el inicio del nuevo ciclo
    clock_gettime(CLOCK_MONOTONIC, &last_time);
}
