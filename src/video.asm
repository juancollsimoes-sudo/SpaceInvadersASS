default rel
%include "game.inc"

section .bss
    window resq 1           ; Puntero a la ventana (8 bytes)
    renderer resq 1         ; Puntero al renderer (8 bytes)
    
    ; Variable temporal interna para dibujar rectángulos
    rect_tmp resd 4

section .text

; ------------------------------------------------------------------------------
; init_video
; In: rdi = char* window_title
; Out: rax = 0 si OK, -1 si Error
; ------------------------------------------------------------------------------
init_video:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    
    mov r12, rdi            ; Guardar window_title
    
    mov rdi, SDL_INIT_VIDEO
    call SDL_Init
    cmp eax, 0
    jl .error
    
    mov rdi, r12
    mov rsi, SDL_WINDOWPOS_CENTERED
    mov rdx, SDL_WINDOWPOS_CENTERED
    mov rcx, WINDOW_WIDTH
    mov r8, WINDOW_HEIGHT
    mov r9, SDL_WINDOW_SHOWN
    call SDL_CreateWindow
    mov [window], rax
    cmp rax, 0
    je .error

    mov rdi, [window]
    mov rsi, -1
    mov rdx, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    call SDL_CreateRenderer
    mov [renderer], rax
    cmp rax, 0
    je .error

    mov eax, 0
    jmp .exit

.error:
    mov eax, -1
.exit:
    pop r12
    pop rbx
    pop rbp
    ret

; ------------------------------------------------------------------------------
; cleanup_video
; ------------------------------------------------------------------------------
cleanup_video:
    push rbp
    mov rbp, rsp

    mov rdi, [renderer]
    cmp rdi, 0
    je .skip_renderer
    call SDL_DestroyRenderer
.skip_renderer:

    mov rdi, [window]
    cmp rdi, 0
    je .skip_window
    call SDL_DestroyWindow
.skip_window:

    call SDL_Quit
    pop rbp
    ret

; ------------------------------------------------------------------------------
; render_frame
; ------------------------------------------------------------------------------
render_frame:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8              ; Alinear pila a 16 bytes (System V ABI)

    ; Limpiar fondo (Negro)
    mov rdi, [renderer]
    mov rsi, 0
    mov rdx, 0
    mov rcx, 0
    mov r8, 255
    call SDL_SetRenderDrawColor

    mov rdi, [renderer]
    call SDL_RenderClear

    ; --- Dibujar Jugador (Verde) ---
    mov rdi, [renderer]
    mov rsi, 0
    mov rdx, 255
    mov rcx, 0
    mov r8, 255
    call SDL_SetRenderDrawColor

    mov eax, dword [player_x]
    mov dword [rect_tmp], eax
    mov eax, dword [player_y]
    mov dword [rect_tmp + 4], eax
    mov dword [rect_tmp + 8], PLAYER_WIDTH
    mov dword [rect_tmp + 12], PLAYER_HEIGHT

    mov rdi, [renderer]
    lea rsi, [rel rect_tmp]
    call SDL_RenderFillRect

    ; --- Dibujar Proyectiles (Amarillo) ---
    mov rdi, [renderer]
    mov rsi, 255
    mov rdx, 255
    mov rcx, 0
    mov r8, 255
    call SDL_SetRenderDrawColor

    mov r12, 0      ; rcx clobbered by calls, use r12
.render_bullet_loop:
    cmp r12, MAX_BULLETS
    jge .render_enemies_start
    
    lea rbx, [rel bullet_active]
    cmp byte [rbx + r12], 0
    je .next_render_bullet

    lea rbx, [rel bullet_x]
    mov eax, dword [rbx + r12*4]
    mov dword [rect_tmp], eax
    
    lea rbx, [rel bullet_y]
    mov eax, dword [rbx + r12*4]
    mov dword [rect_tmp + 4], eax
    
    mov dword [rect_tmp + 8], BULLET_WIDTH
    mov dword [rect_tmp + 12], BULLET_HEIGHT

    mov rdi, [renderer]
    lea rsi, [rel rect_tmp]
    call SDL_RenderFillRect

.next_render_bullet:
    inc r12
    jmp .render_bullet_loop

.render_enemies_start:
    ; --- Dibujar Enemigos (Rojo) ---
    mov rdi, [renderer]
    mov rsi, 255
    mov rdx, 0
    mov rcx, 0
    mov r8, 255
    call SDL_SetRenderDrawColor

    mov r12, 0      ; Índice actual
.render_enemy_loop:
    cmp r12d, dword [active_enemies]
    jge .render_score_call
    
    mov rax, r12
    imul rax, ENEMY_STRUC_SIZE
    lea r13, [rel enemy_array]
    add r13, rax
    
    mov al, byte [r13 + ENEMY_OFFSET_STATUS]
    cmp al, 1
    je .do_render
    cmp al, 2
    je .do_render
    jmp .render_next_enemy
.do_render:
    
    mov r10d, dword [r13 + ENEMY_OFFSET_X]
    mov r11d, dword [r13 + ENEMY_OFFSET_Y]
    
    cmp byte [r13 + ENEMY_OFFSET_PATTERN], PATTERN_SINE
    jne .no_sine_render
    
    lea r15, [rel sine_lut]
    mov ebx, dword [r13 + ENEMY_OFFSET_PHASE]
    mov eax, dword [r15 + rbx*4]
    add r10d, eax
    
.no_sine_render:
    mov dword [rect_tmp], r10d
    mov dword [rect_tmp + 4], r11d
    mov dword [rect_tmp + 8], ENEMY_WIDTH
    mov dword [rect_tmp + 12], ENEMY_HEIGHT
    
    mov rdi, [renderer]
    lea rsi, [rel rect_tmp]
    call SDL_RenderFillRect
    
.render_next_enemy:
    inc r12
    jmp .render_enemy_loop

.render_score_call:
    mov rdi, [renderer]
    call render_score

.present_screen:
    mov rdi, [renderer]
    call SDL_RenderPresent

    ; Retraso para ~60 FPS
    mov rdi, 16
    call SDL_Delay

    add rsp, 8              ; Restaurar alineación de la pila
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
