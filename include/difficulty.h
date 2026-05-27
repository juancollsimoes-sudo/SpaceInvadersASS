#ifndef DIFFICULTY_H
#define DIFFICULTY_H

#include <stdint.h>

typedef enum {
    DIFF_EASY,
    DIFF_NORMAL,
    DIFF_HARD
} DifficultyLevel;

/**
 * @brief Aplica la dificultad seleccionada a las variables globales del juego.
 * Modifica la estructura de estado C y también puede actualizar variables ASM si es necesario.
 * @param nivel El nivel de dificultad deseado.
 */
void aplicar_dificultad(DifficultyLevel nivel);

/**
 * @brief Devuelve la dificultad actual configurada.
 */
DifficultyLevel obtener_dificultad_actual(void);

#endif // DIFFICULTY_H
