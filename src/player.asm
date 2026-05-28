default rel
%include "game.inc"

extern g_config

section .data
    
    player_x dd 380         ; (800 - 40) / 2
    player_y dd 550
    lives dd 3

section .bss
    
    bullet_active resb MAX_BULLETS
    bullet_x resd MAX_BULLETS
    bullet_y resd MAX_BULLETS
    bullet_cooldown resd 1

section .text

; ------------------------------------------------------------------------------
; init_player
; ------------------------------------------------------------------------------
init_player:
    push rbp
    mov rbp, rsp
    
    mov dword [bullet_cooldown], 0
    mov rcx, MAX_BULLETS
.clear_bullets:
    mov byte [bullet_active + rcx - 1], 0
    loop .clear_bullets
    
    mov dword [player_x], 380
    mov dword [player_y], 550
    mov dword [lives], 3
    
    pop rbp
    ret

; ------------------------------------------------------------------------------
; update_player
; ------------------------------------------------------------------------------
update_player:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    
    mov r10, [keyboard_state]
    
    ; --- Movimiento ---
    cmp byte [r10 + SDL_SCANCODE_LEFT], 1
    jne .check_right
    
    mov eax, dword [player_x]
    cvttss2si edx, dword [g_config + 4] ; Convert float player_speed to int
    sub eax, edx
    cmp eax, 0
    jge .store_left
    mov eax, 0
.store_left:
    mov dword [player_x], eax

.check_right:
    cmp byte [r10 + SDL_SCANCODE_RIGHT], 1
    jne .check_shoot
    
    mov eax, dword [player_x]
    cvttss2si edx, dword [g_config + 4]
    add eax, edx
    mov ecx, WINDOW_WIDTH
    sub ecx, PLAYER_WIDTH
    cmp eax, ecx
    jle .store_right
    mov eax, ecx
.store_right:
    mov dword [player_x], eax

.check_shoot:
    cmp dword [bullet_cooldown], 0
    je .check_space_key
    dec dword [bullet_cooldown]

.check_space_key:
    cmp byte [r10 + SDL_SCANCODE_SPACE], 1
    jne .done

    cmp dword [bullet_cooldown], 0
    jg .done

    ; Buscar proyectil libre
    mov rcx, 0
.find_bullet:
    cmp rcx, MAX_BULLETS
    jge .done
    cmp byte [bullet_active + rcx], 0
    je .fire_bullet
    inc rcx
    jmp .find_bullet

.fire_bullet:
    extern rust_play_sound
    mov edi, 0
    call rust_play_sound

    mov byte [bullet_active + rcx], 1
    
    mov eax, dword [player_x]
    add eax, (PLAYER_WIDTH / 2) - (BULLET_WIDTH / 2)
    mov dword [bullet_x + rcx*4], eax

    mov eax, dword [player_y]
    sub eax, BULLET_HEIGHT
    mov dword [bullet_y + rcx*4], eax
    
    mov dword [bullet_cooldown], BULLET_COOLDOWN_FRAMES

.done:
    pop r12
    pop rbx
    pop rbp
    ret

; ------------------------------------------------------------------------------
; update_bullets
; ------------------------------------------------------------------------------
update_bullets:
    push rbp
    mov rbp, rsp
    
    mov rcx, 0
.loop:
    cmp rcx, MAX_BULLETS
    jge .done
    
    cmp byte [bullet_active + rcx], 0
    je .next
    
    mov eax, dword [bullet_y + rcx*4]
    sub eax, BULLET_SPEED
    mov dword [bullet_y + rcx*4], eax
    
    cmp eax, 0
    jge .next
    mov byte [bullet_active + rcx], 0
    
.next:
    inc rcx
    jmp .loop

.done:
    pop rbp
    ret
