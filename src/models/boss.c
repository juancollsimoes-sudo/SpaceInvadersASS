#include <stdint.h>
#include "sprites.h" // For SPRITE_BOSS, SPRITE_BULLET, dibujar_sprite, obtener_color_sprite

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

#define MAX_ALIEN_BULLETS 10
uint8_t alien_bullet_active_c[MAX_ALIEN_BULLETS] = {0};
int32_t alien_bullet_x_c[MAX_ALIEN_BULLETS] = {0};
int32_t alien_bullet_y_c[MAX_ALIEN_BULLETS] = {0};

void init_boss_c(void) {
    boss_active_c = 1;
    boss_hp_c = (current_wave + 1) * 3;
    boss_x_c = 350;
    boss_y_c = 50;
    boss_vel_x_c = 2;
    boss_shoot_timer_c = 45;
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        alien_bullet_active_c[i] = 0;
    }
}

void update_boss_c(void) {
    // 1. Update Alien Bullets unconditionally
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (alien_bullet_active_c[i]) {
            alien_bullet_y_c[i] += 7; // ALIEN_BULLET_SPEED
            if (alien_bullet_y_c[i] >= 600) { // WINDOW_HEIGHT
                alien_bullet_active_c[i] = 0;
            }
        }
    }

    if (!boss_active_c) return;

    // 2. Boss Movement
    boss_x_c += boss_vel_x_c;
    if (boss_x_c <= 0 || boss_x_c >= 800 - 60) {
        boss_vel_x_c = -boss_vel_x_c;
    }

    // 3. Boss Shooting
    boss_shoot_timer_c--;
    if (boss_shoot_timer_c <= 0) {
        boss_shoot_timer_c = 45;
        // Find free bullet
        for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
            if (!alien_bullet_active_c[i]) {
                alien_bullet_active_c[i] = 1;
                alien_bullet_x_c[i] = boss_x_c + 30;
                alien_bullet_y_c[i] = boss_y_c + 40;
                break;
            }
        }
    }
}

void check_boss_collisions_c(void) {
    // 1. Check Player Bullets vs Boss
    if (boss_active_c) {
        for (int i = 0; i < 5; i++) {
            if (bullet_active[i]) {
                if (bullet_x[i] < boss_x_c + 45 &&
                    bullet_x[i] + 4 > boss_x_c &&
                    bullet_y[i] < boss_y_c + 45 &&
                    bullet_y[i] + 15 > boss_y_c) {
                    
                    bullet_active[i] = 0;
                    boss_hp_c--;
                    
                    if (boss_hp_c <= 0) {
                        boss_active_c = 0;
                        score += 100;
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

void render_boss_c(void) {
    if (boss_active_c) {
        uint32_t color = obtener_color_sprite(4); // SPRITE_BOSS = 4
        dibujar_sprite(4, boss_x_c, boss_y_c, 45, 45, color);
    }

    // Render Alien Bullets
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (alien_bullet_active_c[i]) {
            dibujar_sprite(3, alien_bullet_x_c[i], alien_bullet_y_c[i], 4, 15, 0xFF0000FF);
        }
    }
}
