#include "game_state.h"
#include "difficulty.h"
#include "rust_bindings.h"

// --- Declaración de variables globales definidas en el código ASM ---
// El compilador de C y el enlazador (LD/GCC) resolverán estos símbolos a las
// mismas direcciones de memoria utilizadas por tus archivos .asm.
extern int32_t score;        // Definido en ui.asm (dword / dd)
extern int32_t lives;        // Definido en player.asm (dword / dd)
extern int32_t current_wave;  // Definido en enemies.asm (dword / resd)

// Instancia global en C para cumplir con la interfaz
GameState g_game_state = {
    .score = 0,
    .lives = 3,
    .current_round = 1,
    .enemy_speed = 2.0f,
    .enemy_fire_rate = 2.0f
};

// Variable global de configuración expuesta para ASM
GameConfig g_config;

void inicializar_configuracion(void) {
    g_config = load_game_config();
}

void inicializar_juego(void) {
    // Sincronizar ASM
    score = 0;
    current_wave = 0;
    
    // Sincronizar C
    g_game_state.score = 0;
    g_game_state.current_round = 1;
    
    // Aplicar los valores de la dificultad actual (lives, speed, fire_rate)
    // Usar la dificultad de g_config si el menu no lo hizo
    aplicar_dificultad(obtener_dificultad_actual());
    
    // Sobrescribir vidas iniciales con el valor del archivo TOML
    lives = g_config.player_lives;
    g_game_state.lives = lives;
}

void avanzar_ronda(void) {
    // 1. Cargar estado actual de las variables de ASM
    g_game_state.score = score;
    g_game_state.lives = lives;
    g_game_state.current_round = current_wave + 1; // Ronda es 1-indexed, wave es 0-indexed
    
    // 2. Incrementar ronda y dificultad matemáticamente
    g_game_state.current_round++;
    g_game_state.enemy_speed *= 1.20f;
    g_game_state.enemy_fire_rate /= 1.15f;
    
    // 3. Escribir cambios de vuelta a ASM
    current_wave = g_game_state.current_round - 1;
}

void sumar_puntos(int32_t puntos) {
    score += puntos;
    g_game_state.score = score;
}

void perder_vida(void) {
    if (lives > 0) {
        lives--;
    }
    g_game_state.lives = lives;
}

// --- Getters que sincronizan dinámicamente con ASM ---

int32_t obtener_puntuacion(void) {
    g_game_state.score = score;
    return score;
}

int32_t obtener_vidas(void) {
    g_game_state.lives = lives;
    return lives;
}

int32_t obtener_ronda(void) {
    g_game_state.current_round = current_wave + 1;
    return g_game_state.current_round;
}

float obtener_velocidad_enemigos(void) {
    return g_game_state.enemy_speed;
}

float obtener_tasa_disparo(void) {
    return g_game_state.enemy_fire_rate;
}

int32_t obtener_cooldown_frames_alien(void) {
    // Convierte el multiplicador de fire_rate a frames (60fps). 
    // Ej: 2.0 = 120 frames, 1.0 = 60 frames.
    return (int32_t)(g_game_state.enemy_fire_rate * 60.0f);
}

void check_and_save_high_score(void) {
    int current_best = load_high_score();
    if (score > current_best) {
        save_high_score(score);
    }
}
