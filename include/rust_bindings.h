#ifndef RUST_BINDINGS_H
#define RUST_BINDINGS_H

#include <stdint.h>

/* ── Fase 2: Persistencia ─────────────────────────────────────────── */
extern int32_t save_high_score(int32_t score);
extern int32_t load_high_score(void);

#endif /* RUST_BINDINGS_H */
