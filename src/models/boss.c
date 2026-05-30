#include <stdint.h>
#include "sprites.h" // For SPRITE_BOSS, SPRITE_BULLET, SPRITE_ULTRABOSS, dibujar_sprite, obtener_color_sprite

// Expose these from ASM
extern int32_t current_wave;
extern int32_t score;
extern int32_t player_x;
extern int32_t player_y;
extern int32_t lives;
extern uint8_t running;
extern uint8_t bullet_active[5]; // MAX_BULLETS = 5
extern int32_t bullet_x[5];
extern int32_t bullet_y[5];
extern int32_t player_has_shield;

extern void rust_play_sound(int32_t sound_id);
extern void init_wave(void);
extern uint32_t obtener_color_sprite(uint8_t sprite_id);

// Boss State
int32_t boss_active_c = 0;
int32_t boss_hp_c = 0;
int32_t boss_x_c = 0;
int32_t boss_y_c = 0;
int32_t boss_vel_x_c = 0;
int32_t boss_shoot_timer_c = 0;

int32_t boss_type_c = 0; // 0 = Normal, 1 = Ultra Boss
int32_t boss_shot_count_c = 1;
int32_t ultra_boss_pattern_timer = 0;

#define MAX_ALIEN_BULLETS 100
uint8_t alien_bullet_active_c[MAX_ALIEN_BULLETS] = {0};
int32_t alien_bullet_x_c[MAX_ALIEN_BULLETS] = {0};
int32_t alien_bullet_y_c[MAX_ALIEN_BULLETS] = {0};
int32_t alien_bullet_vx_c[MAX_ALIEN_BULLETS] = {0};

void init_boss_c(void) {
    boss_active_c = 1;
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        alien_bullet_active_c[i] = 0;
    }

    if ((current_wave + 1) % 10 == 0) {
        // Ultra Boss
        boss_type_c = 1;
        boss_hp_c = 75; // Reducido de 250 para evitar que parezca inmortal
        boss_x_c = 325; // Centered (800 - 150) / 2
        boss_y_c = 30;
        boss_vel_x_c = 0; // Does not move
        boss_shoot_timer_c = 10;
        ultra_boss_pattern_timer = 0;
    } else {
        // Normal Boss
        boss_type_c = 0;
        boss_hp_c = (current_wave + 1) * 3;
        boss_x_c = 350;
        boss_y_c = 50;
        boss_vel_x_c = 2;
        boss_shoot_timer_c = 45;
        
        boss_shot_count_c = 1 + (current_wave / 5);
        if (boss_shot_count_c > 7) boss_shot_count_c = 7;
    }
}

void update_boss_c(void) {
    // 1. Update Alien Bullets unconditionally
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (alien_bullet_active_c[i]) {
            alien_bullet_y_c[i] += 7; // ALIEN_BULLET_SPEED
            alien_bullet_x_c[i] += alien_bullet_vx_c[i];
            if (alien_bullet_y_c[i] >= 600 || alien_bullet_x_c[i] < -10 || alien_bullet_x_c[i] > 810) {
                alien_bullet_active_c[i] = 0;
            }
        }
    }

    if (!boss_active_c) return;

    // 2. Boss Movement (Only for Normal Boss)
    if (boss_type_c == 0) {
        boss_x_c += boss_vel_x_c;
        if (boss_x_c <= 0 || boss_x_c >= 800 - 45) {
            boss_vel_x_c = -boss_vel_x_c;
        }
    }

    // 3. Boss Shooting
    if (boss_type_c == 1) {
        ultra_boss_pattern_timer++;
    }

    boss_shoot_timer_c--;
    if (boss_shoot_timer_c <= 0) {
        if (boss_type_c == 0) {
            boss_shoot_timer_c = 45;
            int spawned = 0;
            for (int i = 0; i < MAX_ALIEN_BULLETS && spawned < boss_shot_count_c; i++) {
                if (!alien_bullet_active_c[i]) {
                    alien_bullet_active_c[i] = 1;
                    alien_bullet_x_c[i] = boss_x_c + 20; // center of 45x45
                    alien_bullet_y_c[i] = boss_y_c + 40;
                    
                    int offset = spawned - (boss_shot_count_c / 2);
                    alien_bullet_vx_c[i] = offset * 2;
                    
                    spawned++;
                }
            }
        } else {
            // Ultra Boss Patterns
            int pattern = (ultra_boss_pattern_timer / 90) % 3; // Cambia cada 1.5 segundos (90 frames a 60fps)
            
            if (pattern == 0) {
                // Patron 1: Abanico ancho y estrecho alternado
                boss_shoot_timer_c = 25; 
                int num_shots = ((ultra_boss_pattern_timer / 25) % 2 == 0) ? 9 : 8;
                int spawned = 0;
                for (int i = 0; i < MAX_ALIEN_BULLETS && spawned < num_shots; i++) {
                    if (!alien_bullet_active_c[i]) {
                        alien_bullet_active_c[i] = 1;
                        alien_bullet_x_c[i] = boss_x_c + 75; // center of 150x110
                        alien_bullet_y_c[i] = boss_y_c + 100;
                        
                        int offset = spawned - (num_shots / 2);
                        alien_bullet_vx_c[i] = offset * 2;
                        spawned++;
                    }
                }
            } else if (pattern == 1) {
                // Patron 2: Rafaga directa al jugador
                boss_shoot_timer_c = 12;
                int spawned = 0;
                for (int i = 0; i < MAX_ALIEN_BULLETS && spawned < 2; i++) {
                    if (!alien_bullet_active_c[i]) {
                        alien_bullet_active_c[i] = 1;
                        alien_bullet_x_c[i] = boss_x_c + 75; 
                        alien_bullet_y_c[i] = boss_y_c + 100;
                        
                        // Calculo simple para apuntar
                        int dx = player_x + 20 - alien_bullet_x_c[i];
                        int vx = dx / 40; 
                        if (vx > 5) vx = 5;
                        if (vx < -5) vx = -5;
                        
                        alien_bullet_vx_c[i] = vx + (spawned == 0 ? -1 : 1);
                        spawned++;
                    }
                }
            } else if (pattern == 2) {
                // Patron 3: Lluvia vertical de lado a lado
                boss_shoot_timer_c = 6;
                int spawned = 0;
                for (int i = 0; i < MAX_ALIEN_BULLETS && spawned < 1; i++) {
                    if (!alien_bullet_active_c[i]) {
                        alien_bullet_active_c[i] = 1;
                        // Movimiento de lado a lado usando el timer
                        int sweep = (ultra_boss_pattern_timer * 3) % 300;
                        if (sweep > 150) sweep = 300 - sweep; // zigzag
                        
                        alien_bullet_x_c[i] = boss_x_c + sweep;
                        alien_bullet_y_c[i] = boss_y_c + 100;
                        alien_bullet_vx_c[i] = 0; // Caen rectos
                        spawned++;
                    }
                }
            }
        }
    }
}

void check_boss_collisions_c(void) {
    // 1. Check Player Bullets vs Boss
    if (boss_active_c) {
        int w = (boss_type_c == 0) ? 45 : 150;
        int h = (boss_type_c == 0) ? 45 : 110;

        for (int i = 0; i < 5; i++) {
            if (bullet_active[i]) {
                if (bullet_x[i] < boss_x_c + w &&
                    bullet_x[i] + 4 > boss_x_c &&
                    bullet_y[i] < boss_y_c + h &&
                    bullet_y[i] + 15 > boss_y_c) {
                    
                    bullet_active[i] = 0;
                    boss_hp_c--;
                    
                    if (boss_hp_c <= 0) {
                        boss_active_c = 0;
                        score += (boss_type_c == 0) ? 100 : 1000;
                        rust_play_sound(1); // SOUND_EXPLOSION
                        // Proceed to next wave using asm callback
                        init_wave(); 
                    } else {
                        rust_play_sound(1); // SOUND_EXPLOSION
                    }
                }
            }
        }
    }

    // 2. Check Alien Bullets vs Player
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (alien_bullet_active_c[i]) {
            if (player_x < alien_bullet_x_c[i] + 4 &&
                player_x + 40 > alien_bullet_x_c[i] && // PLAYER_WIDTH
                player_y < alien_bullet_y_c[i] + 15 && // BULLET_HEIGHT
                player_y + 20 > alien_bullet_y_c[i]) { // PLAYER_HEIGHT
                
                alien_bullet_active_c[i] = 0;
                
                if (player_has_shield) {
                    player_has_shield = 0;
                    rust_play_sound(1); // SOUND_EXPLOSION para indicar que el escudo absorbio
                } else {
                    lives--;
                    rust_play_sound(2); // SOUND_PLAYER_DEATH
                    
                    if (lives > 0) {
                        player_x = 380;
                    } else {
                        running = 0;
                    }
                }
            }
        }
    }
}

void render_boss_c(void) {
    if (boss_active_c) {
        if (boss_type_c == 0) {
            uint32_t color = obtener_color_sprite(4); // SPRITE_BOSS = 4
            dibujar_sprite(4, boss_x_c, boss_y_c, 45, 45, color);
        } else {
            uint32_t color = 0x8A2BE2FF; // Purple (Blue Violet)
            dibujar_sprite(6, boss_x_c, boss_y_c, 150, 110, color); // SPRITE_ULTRABOSS = 6
        }
    }

    // Render Alien Bullets
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (alien_bullet_active_c[i]) {
            dibujar_sprite(3, alien_bullet_x_c[i], alien_bullet_y_c[i], 4, 15, 0xFF0000FF);
        }
    }
}
