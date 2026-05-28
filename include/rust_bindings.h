#ifndef RUST_BINDINGS_H
#define RUST_BINDINGS_H

#include <stdint.h>

/* ── Fase 2: Persistencia ─────────────────────────────────────────── */
extern int32_t save_high_score(int32_t score);
extern int32_t load_high_score(void);

/* ── Fase 3: Configuración ───────────────────────────────────────── */
typedef struct {
    int32_t player_lives;
    float   player_speed;
    float   alien_shoot_rate;
    int32_t initial_difficulty;
} GameConfig;

extern GameConfig load_game_config(void);

#endif /* RUST_BINDINGS_H */
