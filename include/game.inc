; ==============================================================================
; SPACE INVADERS - MASTER HEADER FILE (game.inc)
; Contiene todas las macros, constantes y declaraciones externas compartidas
; ==============================================================================

; ------------------------------------------------------------------------------
; SDL2 Constantes
; ------------------------------------------------------------------------------
%define SDL_INIT_VIDEO          0x00000020
%define SDL_WINDOWPOS_CENTERED  0x2FFF0000
%define SDL_WINDOW_SHOWN        0x00000040
%define SDL_RENDERER_ACCELERATED 0x00000002
%define SDL_RENDERER_PRESENTVSYNC 0x00000004
%define SDL_QUIT                0x100
%define SDL_KEYDOWN             0x300

; Códigos de teclado (Scancodes)
%define SDL_SCANCODE_RIGHT      79
%define SDL_SCANCODE_LEFT       80
%define SDL_SCANCODE_SPACE      44

; ------------------------------------------------------------------------------
; Constantes del Juego
; ------------------------------------------------------------------------------
; Ventana
%define WINDOW_WIDTH            800
%define WINDOW_HEIGHT           600

; Jugador
%define PLAYER_WIDTH            40
%define PLAYER_HEIGHT           20
%define PLAYER_SPEED            5

; Proyectil
%define BULLET_WIDTH            4
%define BULLET_HEIGHT           15
%define BULLET_SPEED            10
%define MAX_BULLETS             5
%define BULLET_COOLDOWN_FRAMES  15

; Enemigos
%define ENEMY_ROWS              5
%define ENEMY_COLS              11
%define ENEMY_WIDTH             30
%define ENEMY_HEIGHT            20
%define ENEMY_PADDING_X         15
%define ENEMY_PADDING_Y         15
%define ENEMY_START_X           50
%define ENEMY_START_Y           50
%define ENEMY_SPEED_X           2
%define ENEMY_SPEED_Y           20
%define MAX_ENEMIES             55

; Struct Enemigo (AoS de 32 bytes)
%define ENEMY_STRUC_SIZE        32
%define ENEMY_OFFSET_STATUS     0   ; 1 byte
%define ENEMY_OFFSET_PATTERN    1   ; 1 byte
%define ENEMY_OFFSET_PADDING    2   ; 2 bytes
%define ENEMY_OFFSET_X          4   ; 4 bytes (dword)
%define ENEMY_OFFSET_Y          8   ; 4 bytes (dword)
%define ENEMY_OFFSET_VEL_X      12  ; 4 bytes (dword)
%define ENEMY_OFFSET_VEL_Y      16  ; 4 bytes (dword)
%define ENEMY_OFFSET_PHASE      20  ; 4 bytes (dword)

; Patrones de Movimiento
%define PATTERN_DIAGONAL        0
%define PATTERN_SINE            1
%define PATTERN_KAMIKAZE        2

; Fuente y UI
%define FONT_PIXEL_SIZE         4
%define FONT_CHAR_WIDTH         5
%define FONT_CHAR_HEIGHT        5
%define SCORE_DRAW_X            650
%define SCORE_DRAW_Y            20

; ------------------------------------------------------------------------------
; EXTERNS: Funciones de SDL2 (Resolución dinámica)
; ------------------------------------------------------------------------------
extern SDL_Init
extern SDL_CreateWindow
extern SDL_CreateRenderer
extern SDL_PollEvent
extern SDL_SetRenderDrawColor
extern SDL_RenderClear
extern SDL_RenderPresent
extern SDL_Delay
extern SDL_DestroyRenderer
extern SDL_DestroyWindow
extern SDL_Quit
extern SDL_GetKeyboardState
extern SDL_RenderFillRect

; ------------------------------------------------------------------------------
; EXTERNS: Estado Global (Variables compartidas entre módulos)
; ------------------------------------------------------------------------------
; Desde main.asm
extern running

; Desde video.asm
extern window
extern renderer

; Desde input.asm
extern sdl_event
extern keyboard_state

; Desde player.asm
extern player_x
extern player_y
extern lives
extern bullet_active
extern bullet_x
extern bullet_y
extern bullet_cooldown

; Desde enemies.asm
extern current_wave
extern active_enemies
extern enemy_array

; Desde ui.asm
extern score
extern font_lut

; Desde physics.asm (sine_lut puede estar aquí o en enemies, lo pondremos en enemies)
extern sine_lut

; ------------------------------------------------------------------------------
; EXTERNS: Subrutinas (Funciones exportadas por módulos)
; ------------------------------------------------------------------------------
; Funciones de video.asm
extern init_video
extern cleanup_video
extern render_frame

; Funciones de input.asm
extern init_input
extern poll_events

; Funciones de player.asm
extern init_player
extern update_player
extern update_bullets

; Funciones de enemies.asm
extern init_wave
extern update_enemies

; Funciones de physics.asm
extern check_collisions

; Funciones de ui.asm
extern render_score

; Módulo de Sprites en C
extern dibujar_sprite

; Módulo de Menu y Colores en C
extern ejecutar_menu
extern obtener_color_sprite
