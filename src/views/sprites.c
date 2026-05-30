#include "sprites.h"
#include <stddef.h>
#include <SDL2/SDL.h>

// --- Variable global de renderizado definida en video.asm (renderer resq 1) ---
extern void *renderer;

/**
 * Representaciones visuales de los Sprites sencillos.
 * 0 = Píxel vacío / transparente.
 * 1 = Píxel relleno de color.
 *
 * Utilizan dimensiones compactas (5x5 para entidades y 3x5 para balas)
 * que facilitan una lógica de rasterizado muy simple en ensamblador x86_64.
 */

// Nave del jugador (5x5) - Forma de pirámide/nave retro
static const unsigned char player_data[] = {
    0,0,1,0,0,
    0,1,1,1,0,
    1,1,1,1,1,
    1,0,0,0,1,
    1,0,0,0,1
};

// Alien tipo A (5x5) - Forma de cangrejo/invasor con antenas
static const unsigned char alien_a_data[] = {
    1,0,0,0,1,
    0,1,1,1,0,
    1,1,1,1,1,
    0,1,1,1,0,
    1,0,0,0,1
};

// Alien tipo B (5x5) - Invasor alternativo con tentáculos hacia abajo
static const unsigned char alien_b_data[] = {
    0,1,1,1,0,
    1,0,1,0,1,
    1,1,1,1,1,
    0,1,0,1,0,
    1,0,0,0,1
};

// Proyectil láser (3x5) - Una línea vertical simple
static const unsigned char bullet_data[] = {
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0
};

// Boss (9x9) - Forma de escudo/calavera
static const unsigned char boss_data[] = {
    0,0,1,1,1,1,1,0,0,
    0,1,0,0,1,0,0,1,0,
    1,0,1,0,1,0,1,0,1,
    1,0,0,0,1,0,0,0,1,
    1,1,1,1,0,1,1,1,1,
    1,0,0,0,1,0,0,0,1,
    1,0,1,0,1,0,1,0,1,
    0,1,0,0,1,0,0,1,0,
    0,0,1,1,1,1,1,0,0
};

// Powerup (5x5) - Cubo solido
static const unsigned char powerup_data[] = {
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1,
    1,1,1,1,1
};

// Ultra Boss (15x11) - Nave enorme
static const unsigned char ultraboss_data[] = {
    0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
    0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,
    0,0,0,1,1,1,0,1,0,1,1,1,0,0,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,1,1,1,0,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
    0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,
    0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
    0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,
    0,0,0,0,0,1,1,0,1,1,0,0,0,0,0
};

// Explosion (7x7)
static const unsigned char explosion_data[] = {
    0,0,1,0,1,0,0,
    0,1,0,0,0,1,0,
    1,0,1,1,1,0,1,
    0,0,1,1,1,0,0,
    1,0,1,1,1,0,1,
    0,1,0,0,0,1,0,
    0,0,1,0,1,0,0
};

// Shield (7x5) - Hollow box to wrap player
static const unsigned char shield_data[] = {
    0,1,1,1,1,1,0,
    1,0,0,0,0,0,1,
    1,0,0,0,0,0,1,
    1,0,0,0,0,0,1,
    0,1,1,1,1,1,0
};

// Tabla de Sprites indexada directamente por Sprite ID
static const Sprite sprites[] = {
    [SPRITE_PLAYER]  = { .width = 5, .height = 5, .data = player_data },
    [SPRITE_ALIEN_A] = { .width = 5, .height = 5, .data = alien_a_data },
    [SPRITE_ALIEN_B] = { .width = 5, .height = 5, .data = alien_b_data },
    [SPRITE_BULLET]  = { .width = 3, .height = 5, .data = bullet_data },
    [SPRITE_BOSS]    = { .width = 9, .height = 9, .data = boss_data },
    [SPRITE_POWERUP] = { .width = 5, .height = 5, .data = powerup_data },
    [SPRITE_ULTRABOSS] = { .width = 15, .height = 11, .data = ultraboss_data },
    [SPRITE_EXPLOSION] = { .width = 7, .height = 7, .data = explosion_data },
    [SPRITE_SHIELD]    = { .width = 7, .height = 5, .data = shield_data }
};

#define TOTAL_SPRITES ((int32_t)(sizeof(sprites) / sizeof(sprites[0])))

// --- Funciones de acceso conformes con la System V AMD64 ABI ---

const unsigned char* obtener_sprite_data(int32_t sprite_id) {
    if (sprite_id < 0 || sprite_id >= TOTAL_SPRITES) {
        return NULL;
    }
    return sprites[sprite_id].data;
}

int32_t obtener_sprite_width(int32_t sprite_id) {
    if (sprite_id < 0 || sprite_id >= TOTAL_SPRITES) {
        return 0;
    }
    return sprites[sprite_id].width;
}

int32_t obtener_sprite_height(int32_t sprite_id) {
    if (sprite_id < 0 || sprite_id >= TOTAL_SPRITES) {
        return 0;
    }
    return sprites[sprite_id].height;
}

void dibujar_sprite(int32_t sprite_id, int32_t x, int32_t y, int32_t dest_w, int32_t dest_h, uint32_t color_rgba) {
    const unsigned char *data = obtener_sprite_data(sprite_id);
    if (!data || !renderer) return;
    
    int32_t w = obtener_sprite_width(sprite_id);
    int32_t h = obtener_sprite_height(sprite_id);
    if (w <= 0 || h <= 0) return;
    
    // Descomponer color RGBA
    uint8_t r = (color_rgba >> 24) & 0xFF;
    uint8_t g = (color_rgba >> 16) & 0xFF;
    uint8_t b = (color_rgba >> 8) & 0xFF;
    uint8_t a = color_rgba & 0xFF;
    
    // Guardar el color actual del renderer para restaurarlo después de dibujar
    uint8_t orig_r, orig_g, orig_b, orig_a;
    SDL_GetRenderDrawColor((SDL_Renderer*)renderer, &orig_r, &orig_g, &orig_b, &orig_a);
    
    // Establecer el color del sprite
    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, r, g, b, a);
    
    // Ancho y alto de cada sub-píxel en pantalla para escalado exacto
    float pixel_w = (float)dest_w / w;
    float pixel_h = (float)dest_h / h;
    
    for (int32_t row = 0; row < h; row++) {
        for (int32_t col = 0; col < w; col++) {
            if (data[row * w + col] != 0) {
                SDL_Rect rect;
                rect.x = x + (int32_t)(col * pixel_w);
                rect.y = y + (int32_t)(row * pixel_h);
                // Técnica de escalado continuo para evitar costuras de redondeo flotante
                rect.w = (int32_t)((col + 1) * pixel_w) - (int32_t)(col * pixel_w);
                rect.h = (int32_t)((row + 1) * pixel_h) - (int32_t)(row * pixel_h);
                
                SDL_RenderFillRect((SDL_Renderer*)renderer, &rect);
            }
        }
    }
    
    // Restaurar el color original del renderer
    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, orig_r, orig_g, orig_b, orig_a);
}
