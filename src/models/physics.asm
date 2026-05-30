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
    sub rsp, 8              ; Alinear pila a 16 bytes (System V ABI)

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
    add dword [score], 10
    
    extern player_has_explosive
    extern trigger_explosion_c
    extern try_spawn_powerup_c
    
    cmp dword [player_has_explosive], 1
    jne .no_explosion
    
    push rcx
    push r9
    push r10
    push r11
    push r14
    
    mov edi, r10d
    mov esi, r11d
    call trigger_explosion_c
    
    pop r14
    pop r11
    pop r10
    pop r9
    pop rcx
    jmp .after_explosion

.no_explosion:
    extern rust_play_sound
    push rcx
    push r9
    push r10
    push r11
    push r14
    
    mov edi, 1 ; SOUND_EXPLOSION
    call rust_play_sound
    
    pop r14
    pop r11
    pop r10
    pop r9
    pop rcx

.after_explosion:
    push rcx
    push r9
    push r10
    push r11
    push r14
    
    mov edi, r10d
    mov esi, r11d
    call try_spawn_powerup_c
    
    pop r14
    pop r11
    pop r10
    pop r9
    pop rcx
    
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
    extern player_has_shield
    cmp dword [player_has_shield], 1
    jne .take_damage
    
    ; Tiene escudo: destruye escudo y nave enemiga sin perder vida
    mov dword [player_has_shield], 0
    mov byte [r9 + ENEMY_OFFSET_STATUS], 0
    
    extern rust_play_sound
    push rcx
    push r9
    push r10
    push r11
    mov edi, 1 ; SOUND_EXPLOSION
    call rust_play_sound
    pop r11
    pop r10
    pop r9
    pop rcx
    jmp .next_enemy
    
.take_damage:
    sub dword [lives], 1
    
    extern rust_play_sound
    push rcx
    push r9
    push r10
    push r11

    mov edi, 2 ; SOUND_PLAYER_DEATH
    call rust_play_sound
    
    pop r11
    pop r10
    pop r9
    pop rcx

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
    ; Boss and alien bullet collisions are now handled in C
.exit_cols:
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
