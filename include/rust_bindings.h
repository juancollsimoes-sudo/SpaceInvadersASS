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

/* ── Fase 4: Audio ───────────────────────────────────────────────── */
#define SOUND_SHOOT         0
#define SOUND_EXPLOSION     1
#define SOUND_PLAYER_DEATH  2
#define SOUND_BACKGROUND    3

extern int32_t rust_init_audio(void);
extern void rust_play_sound(int32_t sound_id);

/* ── Fase 5: IA Procedimental ────────────────────────────────────── */
extern void rust_update_enemy(void* enemy, int32_t player_x);

#endif /* RUST_BINDINGS_H */
