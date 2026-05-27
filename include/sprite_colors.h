#ifndef SPRITE_COLORS_H
#define SPRITE_COLORS_H

#include <stdint.h>

// Definición de paletas de color
#define COLOR_RETRO_GREEN  0x33FF33FF
#define COLOR_CYAN         0x00FFFFFF
#define COLOR_ORANGE       0xFFA500FF
#define COLOR_RED          0xFF3333FF
#define COLOR_WHITE        0xFFFFFFFF
#define COLOR_PURPLE       0x800080FF

/**
 * @brief Inicializa los colores de los sprites por defecto.
 */
void inicializar_colores(void);

/**
 * @brief Establece un color RGBA específico para un sprite ID.
 */
void establecer_color_sprite(int32_t sprite_id, uint32_t color_rgba);

/**
 * @brief Obtiene el color RGBA actual de un sprite ID.
 * Usada por ASM y C para dibujar.
 * @return Retorna el color RGBA en EAX (convención AMD64).
 */
uint32_t obtener_color_sprite(int32_t sprite_id);

/**
 * @brief Cicla el color de un sprite a través de la paleta disponible.
 */
void ciclar_paleta_sprite(int32_t sprite_id);

#endif // SPRITE_COLORS_H
