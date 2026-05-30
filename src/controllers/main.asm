default rel
%include "game.inc"

section .data
    window_title db "Space Invaders - Modular x86_64", 0

section .bss
    running resb 1

section .text
    global main

main:
    push rbp
    mov rbp, rsp
    
    mov rdi, window_title
    call init_video
    cmp rax, 0
    jl .error

    extern inicializar_configuracion
    call inicializar_configuracion

    extern rust_init_audio
    call rust_init_audio

    extern init_powerups_c
    call init_powerups_c

    call init_input
    call init_player
    
    extern load_high_score
    call load_high_score
    
.show_menu:
    call ejecutar_menu
    cmp eax, 0
    jl .cleanup
    
    mov dword [current_wave], 0
    call init_wave
    
    mov byte [running], 1

.game_loop:
    cmp byte [running], 0
    je .check_death

    call poll_events
    
    mov r10, [keyboard_state]
    cmp byte [r10 + SDL_SCANCODE_P], 1
    je .do_pause
    cmp byte [r10 + SDL_SCANCODE_ESCAPE], 1
    jne .no_pause

.do_pause:
    extern ejecutar_pausa
    call ejecutar_pausa
    cmp eax, -1
    je .show_menu

.no_pause:
    
    call update_player
    call update_bullets
    call update_enemies
    
    extern update_boss_c
    call update_boss_c
    
    extern update_powerups_c
    call update_powerups_c
    
    extern update_explosions_c
    call update_explosions_c
    
    call check_collisions
    extern check_boss_collisions_c
    call check_boss_collisions_c
    
    extern check_powerup_collisions_c
    call check_powerup_collisions_c
    
    call render_frame
    
    jmp .game_loop

.check_death:
    extern lives
    cmp dword [lives], 0
    jg .cleanup
    
    ; Guardar score antes de resetear
    extern check_and_save_high_score
    call check_and_save_high_score
    
    ; Resetear el juego
    call init_player
    extern init_powerups_c
    call init_powerups_c
    
    extern score
    mov dword [score], 0
    
    extern boss_active_c
    mov dword [boss_active_c], 0
    
    ; Volver al menú
    jmp .show_menu

.error:
    mov eax, 1
    jmp .exit

.cleanup:
    extern check_and_save_high_score
    call check_and_save_high_score
    call cleanup_video
    mov eax, 0

.exit:
    pop rbp
    ret
