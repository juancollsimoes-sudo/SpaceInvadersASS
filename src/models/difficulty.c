#include "difficulty.h"
#include "game_state.h"

// Variables de estado del juego (asm) que la dificultad puede afectar inicialmente
extern int32_t lives;

static DifficultyLevel current_difficulty = DIFF_NORMAL;

void aplicar_dificultad(DifficultyLevel nivel) {
    current_difficulty = nivel;
    
    // Configuramos valores base de la dificultad en la estructura global
    // Se sobreescriben en inicializar_juego() así que la lógica final 
    // debe estar sincronizada allí.
    switch (nivel) {
        case DIFF_EASY:
            g_game_state.lives = 5;
            g_game_state.enemy_speed = 1.0f;
            g_game_state.enemy_fire_rate = 3.0f;
            break;
        case DIFF_NORMAL:
            g_game_state.lives = 3;
            g_game_state.enemy_speed = 2.0f;
            g_game_state.enemy_fire_rate = 2.0f;
            break;
        case DIFF_HARD:
            g_game_state.lives = 1;
            g_game_state.enemy_speed = 3.0f;
            g_game_state.enemy_fire_rate = 1.0f;
            break;
    }
    
    // Sincronizar asm lives
    lives = g_game_state.lives;
}

DifficultyLevel obtener_dificultad_actual(void) {
    return current_difficulty;
}
