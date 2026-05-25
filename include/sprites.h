#ifndef SPRITES_H
#define SPRITES_H

#include <stdint.h>

// Identificadores de Sprite para el código ASM y C
#define SPRITE_PLAYER      0
#define SPRITE_ALIEN_A     1
#define SPRITE_ALIEN_B     2
#define SPRITE_BULLET      3

/**
 * @brief Estructura que contiene las dimensiones y el puntero al arte
 * binario del sprite.
 */
typedef struct {
    int32_t width;                  // Ancho en píxeles (4 bytes, dword)
    int32_t height;                 // Alto en píxeles (4 bytes, dword)
    const unsigned char *data;      // Puntero a los bytes de píxeles (8 bytes, qword)
} Sprite;

/**
 * @brief Obtiene el puntero de memoria del buffer estático que almacena el sprite.
 * @param sprite_id ID del sprite a buscar. Se pasa en el registro RDI.
 * @return Puntero unsigned char* retornado en RAX (0/NULL si el ID es inválido).
 */
const unsigned char* obtener_sprite_data(int32_t sprite_id);

/**
 * @brief Obtiene el ancho del sprite especificado.
 * @param sprite_id ID del sprite. Se pasa en RDI.
 * @return Ancho en píxeles, retornado en EAX.
 */
int32_t obtener_sprite_width(int32_t sprite_id);

/**
 * @brief Obtiene el alto del sprite especificado.
 * @param sprite_id ID del sprite. Se pasa en RDI.
 * @return Alto en píxeles, retornado en EAX.
 */
int32_t obtener_sprite_height(int32_t sprite_id);

/**
 * @brief Dibuja un sprite en pantalla pixel por pixel con escalamiento automático.
 * Accede internamente a la variable global 'renderer' de ASM.
 * @param sprite_id ID del sprite (RDI)
 * @param x Posición X en pantalla (RSI)
 * @param y Posición Y en pantalla (RDX)
 * @param dest_w Ancho final del sprite (RCX)
 * @param dest_h Alto final del sprite (R8)
 * @param color_rgba Color del sprite en formato RGBA de 32 bits (R9)
 */
void dibujar_sprite(int32_t sprite_id, int32_t x, int32_t y, int32_t dest_w, int32_t dest_h, uint32_t color_rgba);

#endif // SPRITES_H
