#ifndef POWERUPS_H
#define POWERUPS_H

#include <stdint.h>

#define MAX_POWERUPS 5
#define MAX_EXPLOSIONS 5

// Tipos de powerups
#define POWERUP_HEALTH     0
#define POWERUP_MULTISHOT  1
#define POWERUP_EXPLOSIVE  2
#define POWERUP_SHIELD     3

typedef struct {
    int32_t active;
    int32_t x;
    int32_t y;
    int32_t type;
} Powerup;

typedef struct {
    int32_t active;
    int32_t x;
    int32_t y;
    int32_t timer;
} Explosion;

// Estado expuesto
extern int32_t player_has_multishot;
extern int32_t player_has_explosive;
extern int32_t player_has_shield;

// Funciones
void init_powerups_c(void);
void update_powerups_c(void);
void render_powerups_c(void);
void try_spawn_powerup_c(int32_t x, int32_t y);
void check_powerup_collisions_c(void);
void trigger_explosion_c(int32_t cx, int32_t cy);

void add_explosion_c(int32_t x, int32_t y);
void update_explosions_c(void);
void render_explosions_c(void);

#endif
