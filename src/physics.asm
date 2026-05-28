default rel
%include "game.inc"

section .text

; ------------------------------------------------------------------------------
; check_collisions
; ------------------------------------------------------------------------------
check_collisions:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rcx, 0
    lea r9, [rel enemy_array]
.enemy_loop:
    cmp ecx, dword [active_enemies]
    jge .done
    
    mov al, byte [r9 + ENEMY_OFFSET_STATUS]
    cmp al, 1
    je .do_collide
    cmp al, 2
    je .do_collide
    jmp .next_enemy
.do_collide:
    
    mov r10d, dword [r9 + ENEMY_OFFSET_X]
    mov r11d, dword [r9 + ENEMY_OFFSET_Y]
    
    cmp byte [r9 + ENEMY_OFFSET_PATTERN], PATTERN_SINE
    jne .check_bullet_cols
    
    lea r15, [rel sine_lut]
    mov ebx, dword [r9 + ENEMY_OFFSET_PHASE]
    mov eax, dword [r15 + rbx*4]
    add r10d, eax

.check_bullet_cols:
    mov r14, 0
.bullet_col_loop:
    cmp r14, MAX_BULLETS
    jge .check_player_col
    
    lea rbx, [rel bullet_active]
    cmp byte [rbx + r14], 0
    je .next_bullet_col
    
    lea rbx, [rel bullet_x]
    mov r12d, dword [rbx + r14*4]
    mov r13d, r10d
    add r13d, ENEMY_WIDTH
    cmp r12d, r13d
    jge .next_bullet_col

    mov r12d, dword [rbx + r14*4]
    add r12d, BULLET_WIDTH
    cmp r12d, r10d
    jle .next_bullet_col

    lea rbx, [rel bullet_y]
    mov r12d, dword [rbx + r14*4]
    mov r13d, r11d
    add r13d, ENEMY_HEIGHT
    cmp r12d, r13d
    jge .next_bullet_col

    mov r12d, dword [rbx + r14*4]
    add r12d, BULLET_HEIGHT
    cmp r12d, r11d
    jle .next_bullet_col

    ; Colisión detectada
    mov byte [r9 + ENEMY_OFFSET_STATUS], 0
    dec dword [active_enemies]
    add dword [score], 10

    extern rust_play_sound
    mov edi, 1 ; SOUND_EXPLOSION
    call rust_play_sound
    
    lea rbx, [rel bullet_active]
    mov byte [rbx + r14], 0
    jmp .next_enemy

.next_bullet_col:
    inc r14
    jmp .bullet_col_loop

.check_player_col:
    mov r12d, dword [player_x]
    mov r13d, r10d
    add r13d, ENEMY_WIDTH
    cmp r12d, r13d
    jge .next_enemy

    mov r12d, dword [player_x]
    add r12d, PLAYER_WIDTH
    cmp r12d, r10d
    jle .next_enemy

    mov r12d, dword [player_y]
    mov r13d, r11d
    add r13d, ENEMY_HEIGHT
    cmp r12d, r13d
    jge .next_enemy

    mov r12d, dword [player_y]
    add r12d, PLAYER_HEIGHT
    cmp r12d, r11d
    jle .next_enemy

    ; Colisión jugador
    sub dword [lives], 1
    
    extern rust_play_sound
    mov edi, 2 ; SOUND_PLAYER_DEATH
    call rust_play_sound

    cmp dword [lives], 0
    jg .reset_player
    
    mov byte [running], 0
    jmp .next_enemy
    
.reset_player:
    mov dword [player_x], 380
    
    mov rbx, 0
.clear_bullets:
    cmp rbx, MAX_BULLETS
    jge .next_enemy
    mov byte [bullet_active + rbx], 0
    inc rbx
    jmp .clear_bullets

.next_enemy:
    add r9, ENEMY_STRUC_SIZE
    inc rcx
    jmp .enemy_loop

.done:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
