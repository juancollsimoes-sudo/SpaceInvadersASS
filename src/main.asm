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
    
    ; 1. Inicialización de sistemas delegada
    mov rdi, window_title
    call init_video
    cmp rax, 0
    jl .error

    extern inicializar_configuracion
    call inicializar_configuracion

    call init_input
    call init_player
    
    ; Fase 2: Cargar high score al iniciar
    extern load_high_score
    call load_high_score
    
    ; 1.5. Pantalla de Inicio (Menu) en C
    call ejecutar_menu
    cmp rax, 0
    jl .cleanup
    
    mov dword [current_wave], 0
    call init_wave
    
    mov byte [running], 1

.game_loop:
    cmp byte [running], 0
    je .cleanup

    ; --- 2. Bucle principal del juego delegando responsabilidades ---
    call poll_events
    
    call update_player
    call update_bullets
    call update_enemies
    
    call check_collisions
    
    call render_frame
    
    jmp .game_loop

.error:
    mov eax, 1
    jmp .exit

.cleanup:
    ; Fase 2: Guardar high score al terminar
    extern check_and_save_high_score
    call check_and_save_high_score
    call cleanup_video
    mov eax, 0

.exit:
    pop rbp
    ret
