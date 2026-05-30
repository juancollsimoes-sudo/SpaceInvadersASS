#include "sprite_colors.h"
#include "sprites.h"

static const uint32_t paleta_colores[] = {
    COLOR_RETRO_GREEN,
    COLOR_CYAN,
    COLOR_ORANGE,
    COLOR_RED,
    COLOR_WHITE,
    COLOR_PURPLE
};
#define NUM_COLORES (sizeof(paleta_colores) / sizeof(paleta_colores[0]))

// Estado de colores de los sprites. Los inicializamos a un default.
static uint32_t colores_sprites[5] = {
    COLOR_RETRO_GREEN, // SPRITE_PLAYER
    COLOR_WHITE,       // SPRITE_ALIEN_A
    COLOR_WHITE,       // SPRITE_ALIEN_B
    COLOR_RED,         // SPRITE_BULLET
    COLOR_PURPLE       // SPRITE_BOSS
};

// Índices actuales en la paleta para poder ciclar
static int indices_paleta[5] = {0, 4, 4, 3, 5}; 

void inicializar_colores(void) {
    colores_sprites[SPRITE_PLAYER] = paleta_colores[0]; // Retro Green
    colores_sprites[SPRITE_ALIEN_A] = paleta_colores[4]; // White
    colores_sprites[SPRITE_ALIEN_B] = paleta_colores[4]; // White
    colores_sprites[SPRITE_BULLET] = paleta_colores[3];  // Red
    colores_sprites[SPRITE_BOSS] = paleta_colores[5];    // Purple
    
    indices_paleta[SPRITE_PLAYER] = 0;
    indices_paleta[SPRITE_ALIEN_A] = 4;
    indices_paleta[SPRITE_ALIEN_B] = 4;
    indices_paleta[SPRITE_BULLET] = 3;
    indices_paleta[SPRITE_BOSS] = 5;
}

void establecer_color_sprite(int32_t sprite_id, uint32_t color_rgba) {
    if (sprite_id >= 0 && sprite_id < 5) {
        colores_sprites[sprite_id] = color_rgba;
    }
}

uint32_t obtener_color_sprite(int32_t sprite_id) {
    if (sprite_id >= 0 && sprite_id < 5) {
        return colores_sprites[sprite_id];
    }
    return COLOR_WHITE; // Fallback
}

void ciclar_paleta_sprite(int32_t sprite_id) {
    if (sprite_id >= 0 && sprite_id < 5) {
        indices_paleta[sprite_id] = (indices_paleta[sprite_id] + 1) % NUM_COLORES;
        colores_sprites[sprite_id] = paleta_colores[indices_paleta[sprite_id]];
        
        // Si ciclas alien A, cicla alien B para que estén en sincronía por simplicidad
        if (sprite_id == SPRITE_ALIEN_A) {
            indices_paleta[SPRITE_ALIEN_B] = indices_paleta[sprite_id];
            colores_sprites[SPRITE_ALIEN_B] = colores_sprites[sprite_id];
        }
    }
}
