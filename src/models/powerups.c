#include "powerups.h"
#include "sprites.h"
#include <stdlib.h>

extern int32_t player_x;
extern int32_t player_y;
extern int32_t lives;
extern int32_t score;
extern void rust_play_sound(int32_t sound_id);

#define ENEMY_STRUC_SIZE 32
#define MAX_ENEMIES 55
extern uint8_t enemy_array[MAX_ENEMIES * ENEMY_STRUC_SIZE];
extern int32_t active_enemies;

Powerup powerups[MAX_POWERUPS];
Explosion explosions[MAX_EXPLOSIONS];

int32_t player_has_multishot = 0;
int32_t player_has_explosive = 0;
int32_t player_has_shield = 0;
int32_t multishot_timer = 0;

void init_powerups_c(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = 0;
    }
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        explosions[i].active = 0;
    }
    player_has_multishot = 0;
    player_has_explosive = 0;
    player_has_shield = 0;
    multishot_timer = 0;
}

void update_powerups_c(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            powerups[i].y += 3;
            if (powerups[i].y >= 600) {
                powerups[i].active = 0;
            }
        }
    }
    
    if (player_has_multishot) {
        multishot_timer--;
        if (multishot_timer <= 0) {
            player_has_multishot = 0;
        }
    }
}

void render_powerups_c(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            uint32_t color = 0xFFFFFFFF;
            if (powerups[i].type == POWERUP_HEALTH) color = 0xFF0000FF; // Rojo
            else if (powerups[i].type == POWERUP_MULTISHOT) color = 0x00FF00FF; // Verde
            else if (powerups[i].type == POWERUP_EXPLOSIVE) color = 0x0000FFFF; // Azul
            else if (powerups[i].type == POWERUP_SHIELD) color = 0xFFFF00FF; // Amarillo
            
            dibujar_sprite(5, powerups[i].x, powerups[i].y, 15, 15, color);
        }
    }
}

void try_spawn_powerup_c(int32_t x, int32_t y) {
    if (rand() % 100 < 7) { // 7% de probabilidad (reducido de 15%)
        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (!powerups[i].active) {
                powerups[i].active = 1;
                powerups[i].x = x + 15;
                powerups[i].y = y;
                powerups[i].type = rand() % 4; // 0 a 3 (4 tipos)
                break;
            }
        }
    }
}

void check_powerup_collisions_c(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            if (player_x < powerups[i].x + 15 &&
                player_x + 40 > powerups[i].x &&
                player_y < powerups[i].y + 15 &&
                player_y + 20 > powerups[i].y) {
                
                powerups[i].active = 0;
                
                if (powerups[i].type == POWERUP_HEALTH) {
                    lives++;
                    rust_play_sound(3);
                } else if (powerups[i].type == POWERUP_MULTISHOT) {
                    player_has_multishot = 1;
                    multishot_timer = 600; // 10 segundos a 60 fps
                    rust_play_sound(3);
                } else if (powerups[i].type == POWERUP_EXPLOSIVE) {
                    player_has_explosive = 1;
                    rust_play_sound(3);
                } else if (powerups[i].type == POWERUP_SHIELD) {
                    player_has_shield = 1;
                    rust_play_sound(3);
                }
            }
        }
    }
}

void add_explosion_c(int32_t x, int32_t y) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!explosions[i].active) {
            explosions[i].active = 1;
            explosions[i].x = x - 40; // Centrar la animacion grande
            explosions[i].y = y - 40;
            explosions[i].timer = 15; // 15 frames de duracion
            break;
        }
    }
}

void update_explosions_c(void) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            explosions[i].timer--;
            if (explosions[i].timer <= 0) {
                explosions[i].active = 0;
            }
        }
    }
}

void render_explosions_c(void) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (explosions[i].active) {
            // Render explosion sprite (7) scaled to 100x100
            dibujar_sprite(7, explosions[i].x, explosions[i].y, 100, 100, 0x00FFFFFF); // Cyan claro
        }
    }
}

void trigger_explosion_c(int32_t cx, int32_t cy) {
    int radius_sq = 80 * 80;
    cx += 15;
    cy += 10;
    
    add_explosion_c(cx, cy);
    
    for (int i = 0; i < active_enemies; i++) {
        int idx = i * ENEMY_STRUC_SIZE;
        uint8_t status = enemy_array[idx];
        
        if (status == 1 || status == 2) {
            int32_t ex = *(int32_t*)(&enemy_array[idx + 4]) + 15;
            int32_t ey = *(int32_t*)(&enemy_array[idx + 8]) + 10;
            
            int dx = cx - ex;
            int dy = cy - ey;
            
            if (dx*dx + dy*dy <= radius_sq) {
                enemy_array[idx] = 0;
                score += 10;
            }
        }
    }
    rust_play_sound(1); // Explosion
}
