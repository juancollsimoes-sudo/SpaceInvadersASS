#include "menu.h"
#include "difficulty.h"
#include "sprite_colors.h"
#include "sprites.h"
#include <SDL2/SDL.h>
#include <string.h>

extern void *renderer; // Definido en ASM
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// --- Mini Renderizador de Texto 5x5 en C ---
// Solo las letras y simbolos necesarios para el menu para mantenerlo simple.
// 5 bytes por letra, 1 byte por fila.
static const unsigned char font_C[26][5] = {
    {14,17,31,17,17}, // A
    {30,17,30,17,30}, // B
    {14,17,16,17,14}, // C
    {30,17,17,17,30}, // D
    {31,16,30,16,31}, // E
    {31,16,30,16,16}, // F
    {14,17,16,23,14}, // G
    {17,17,31,17,17}, // H
    {14,4,4,4,14},    // I
    {7,2,2,18,12},    // J
    {17,18,28,18,17}, // K
    {16,16,16,16,31}, // L
    {17,27,21,17,17}, // M
    {17,25,21,19,17}, // N
    {14,17,17,17,14}, // O
    {30,17,30,16,16}, // P
    {14,17,17,18,13}, // Q
    {30,17,30,18,17}, // R
    {14,16,14,1,30},  // S
    {31,4,4,4,4},     // T
    {17,17,17,17,14}, // U
    {17,17,17,10,4},  // V
    {17,17,21,27,17}, // W
    {17,10,4,10,17},  // X
    {17,10,4,4,4},    // Y
    {31,2,4,8,31}     // Z
};

static const unsigned char font_sym[3][5] = {
    {0,0,0,0,0},      // Espacio (0)
    {8,4,2,4,8},      // < (1)
    {2,4,8,4,2}       // > (2)
};

static void dibujar_char(int32_t x, int32_t y, char c, uint32_t color, int32_t scale) {
    const unsigned char *bitmap = NULL;
    if (c >= 'A' && c <= 'Z') {
        bitmap = font_C[c - 'A'];
    } else if (c == ' ') {
        bitmap = font_sym[0];
    } else if (c == '<') {
        bitmap = font_sym[1];
    } else if (c == '>') {
        bitmap = font_sym[2];
    }
    if (!bitmap || !renderer) return;

    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = color & 0xFF;
    SDL_SetRenderDrawColor((SDL_Renderer*)renderer, r, g, b, a);

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
            if ((bitmap[row] >> (4 - col)) & 1) {
                SDL_Rect rect = { x + col * scale, y + row * scale, scale, scale };
                SDL_RenderFillRect((SDL_Renderer*)renderer, &rect);
            }
        }
    }
}

static void dibujar_texto(int32_t x, int32_t y, const char *text, uint32_t color, int32_t scale) {
    int curr_x = x;
    while (*text) {
        dibujar_char(curr_x, y, *text, color, scale);
        curr_x += 6 * scale; // 5 de ancho + 1 de espacio
        text++;
    }
}

// Opciones del menu
typedef enum {
    OPT_PLAY,
    OPT_DIFFICULTY,
    OPT_PLAYER_COLOR,
    OPT_ALIEN_COLOR,
    OPT_QUIT,
    OPT_MAX
} MenuOption;

int32_t ejecutar_menu(void) {
    SDL_Renderer *rnd = (SDL_Renderer *)renderer;
    if (!rnd) return -1;

    SDL_Event e;
    int32_t frame_count = 0;
    int seleccion = OPT_PLAY;
    DifficultyLevel diff = DIFF_NORMAL;
    
    // Inicializar colores si no lo están
    inicializar_colores();

    const char* dif_texts[] = { "< EASY >", "< NORMAL >", "< HARD >" };

    while (1) {
        // 1. Manejo de eventos
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        return -1;
                    case SDLK_UP:
                        seleccion--;
                        if (seleccion < 0) seleccion = OPT_MAX - 1;
                        break;
                    case SDLK_DOWN:
                        seleccion++;
                        if (seleccion >= OPT_MAX) seleccion = 0;
                        break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        if (seleccion == OPT_DIFFICULTY) {
                            if (e.key.keysym.sym == SDLK_RIGHT) diff = (diff + 1) % 3;
                            else diff = (diff + 2) % 3;
                        } else if (seleccion == OPT_PLAYER_COLOR) {
                            ciclar_paleta_sprite(SPRITE_PLAYER);
                        } else if (seleccion == OPT_ALIEN_COLOR) {
                            ciclar_paleta_sprite(SPRITE_ALIEN_A); // También cicla ALIEN_B
                        }
                        break;
                    case SDLK_RETURN:
                    case SDLK_SPACE:
                        if (seleccion == OPT_PLAY) {
                            aplicar_dificultad(diff);
                            return 0; // Iniciar
                        } else if (seleccion == OPT_QUIT) {
                            return -1;
                        }
                        break;
                }
            }
        }

        // 2. Render
        SDL_SetRenderDrawColor(rnd, 10, 10, 10, 255); // Fondo oscuro
        SDL_RenderClear(rnd);

        // Título
        dibujar_texto(150, 100, "SPACE INVADERS", COLOR_RETRO_GREEN, 6);

        // Animación de sprites decorativa
        int anim_offset = (frame_count / 15) % 2 == 0 ? 0 : 5;
        dibujar_sprite(SPRITE_ALIEN_A, 250, 200 + anim_offset, 40, 40, obtener_color_sprite(SPRITE_ALIEN_A));
        dibujar_sprite(SPRITE_ALIEN_B, 500, 200 - anim_offset, 40, 40, obtener_color_sprite(SPRITE_ALIEN_B));
        dibujar_sprite(SPRITE_PLAYER, 375, 200, 40, 40, obtener_color_sprite(SPRITE_PLAYER));

        // Opciones
        const char *opt_labels[] = {
            "PLAY",
            "DIFFICULTY",
            "PLAYER COLOR",
            "ALIEN COLOR",
            "QUIT"
        };

        for (int i = 0; i < OPT_MAX; i++) {
            uint32_t color = (i == seleccion) ? 0xFFFF00FF : 0x00FF00FF; // Amarillo si seleccionado, sino verde
            
            // Texto de opción
            dibujar_texto(100, 350 + i * 40, opt_labels[i], color, 3);
            
            // Valores dinámicos
            if (i == OPT_DIFFICULTY) {
                dibujar_texto(450, 350 + i * 40, dif_texts[diff], color, 3);
            } else if (i == OPT_PLAYER_COLOR || i == OPT_ALIEN_COLOR) {
                dibujar_texto(450, 350 + i * 40, "< COLOR >", color, 3);
            }
        }

        // Efecto Scanlines simples
        SDL_SetRenderDrawColor(rnd, 0, 0, 0, 100);
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
        for (int y = 0; y < WINDOW_HEIGHT; y += 4) {
            SDL_RenderDrawLine(rnd, 0, y, WINDOW_WIDTH, y);
        }
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE); // restaurar

        SDL_RenderPresent(rnd);
        SDL_Delay(16); // ~60fps
        frame_count++;
    }
    
    return 0;
}

int32_t ejecutar_pausa(void) {
    SDL_Renderer *rnd = (SDL_Renderer *)renderer;
    if (!rnd) return -1;

    SDL_Event e;
    int32_t frame_count = 0;
    int seleccion = 0; // 0 = RESUME, 1 = QUIT
    
    while (1) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return -1;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {
                    seleccion = 1 - seleccion;
                } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
                    if (seleccion == 0) return 0;
                    else return -1;
                }
            }
            if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_p || e.key.keysym.sym == SDLK_ESCAPE) {
                    return 0;
                }
            }
        }

        SDL_SetRenderDrawColor(rnd, 10, 10, 10, 255);
        SDL_RenderClear(rnd);

        dibujar_texto(250, 150, "PAUSED", COLOR_RETRO_GREEN, 6);

        uint32_t color_resume = (seleccion == 0) ? 0xFFFF00FF : COLOR_RETRO_GREEN;
        uint32_t color_quit = (seleccion == 1) ? 0xFFFF00FF : COLOR_RETRO_GREEN;

        dibujar_texto(300, 300, "RESUME", color_resume, 4);
        dibujar_texto(300, 360, "QUIT", color_quit, 4);

        int anim_offset = (frame_count / 15) % 2 == 0 ? 0 : 5;
        int pointer_y = (seleccion == 0) ? 300 : 360;
        dibujar_sprite(SPRITE_PLAYER, 240, pointer_y + anim_offset, 30, 30, obtener_color_sprite(SPRITE_PLAYER));

        SDL_SetRenderDrawColor(rnd, 0, 0, 0, 100);
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_BLEND);
        for (int y = 0; y < WINDOW_HEIGHT; y += 4) {
            SDL_RenderDrawLine(rnd, 0, y, WINDOW_WIDTH, y);
        }
        SDL_SetRenderDrawBlendMode(rnd, SDL_BLENDMODE_NONE);

        SDL_RenderPresent(rnd);
        SDL_Delay(16);
        frame_count++;
    }
    return 0;
}
