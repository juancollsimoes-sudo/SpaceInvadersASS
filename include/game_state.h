#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdint.h>

/**
 * @brief Estructura que almacena el estado global del juego.
 * Los tipos de datos se eligen para que tengan anchos de bits fijos,
 * garantizando una alineación limpia y fácil mapeo en ASM (x86_64).
 */
typedef struct {
    int32_t score;             // Puntuación actual (4 bytes, dword)
    int32_t lives;             // Vidas restantes (4 bytes, dword)
    int32_t current_round;     // Ronda actual (4 bytes, dword)
    float enemy_speed;         // Velocidad actual de los enemigos (4 bytes, float, registro xmm)
    float enemy_fire_rate;     // Tasa de disparo enemigo (4 bytes, float, registro xmm)
} GameState;

// Instancia global que puede ser accedida directamente por ASM usando offsets.
extern GameState g_game_state;

/**
 * @brief Inicializa o reinicia el estado de juego con valores predeterminados.
 */
void inicializar_juego(void);

/**
 * @brief Avanza a la siguiente ronda e incrementa la dificultad.
 * Multiplica la velocidad de los enemigos por 1.2 y reduce el tiempo
 * de disparo en un factor de 1.15.
 */
void avanzar_ronda(void);

/**
 * @brief Añade puntos al score.
 * @param puntos Cantidad de puntos a sumar. Se pasa en el registro RDI.
 */
void sumar_puntos(int32_t puntos);

/**
 * @brief Decrementa una vida del jugador si todavía le quedan vidas.
 */
void perder_vida(void);

/**
 * @brief Funciones de acceso (Getters) convenientes para ASM.
 * Facilitan la interoperabilidad al devolver los valores directamente en los
 * registros estándar de la convención de llamadas System V AMD64 ABI.
 */
int32_t obtener_puntuacion(void);       // Retorna en EAX
int32_t obtener_vidas(void);            // Retorna en EAX
int32_t obtener_ronda(void);            // Retorna en EAX
float obtener_velocidad_enemigos(void);  // Retorna en XMM0
float obtener_tasa_disparo(void);       // Retorna en XMM0

#endif // GAME_STATE_H
