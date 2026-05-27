default rel
%include "game.inc"

section .rodata
    sine_lut dd 0, 3, 6, 9, 11, 14, 17, 19, 21, 23, 25, 27, 28, 29, 30, 30
             dd 30, 30, 29, 28, 27, 25, 23, 21, 19, 17, 14, 11, 9, 6, 3, 0
             dd 0, -3, -6, -9, -11, -14, -17, -19, -21, -23, -25, -27, -28, -29, -30, -30
             dd -30, -30, -29, -28, -27, -25, -23, -21, -19, -17, -14, -11, -9, -6, -3, 0

    ; Jump Table para patrones de movimiento
    pattern_jump_table dq update_enemies.pattern_diagonal, update_enemies.pattern_sine, update_enemies.pattern_kamikaze

section .bss
    ; Alien bullets
    alien_bullet_active resb MAX_ALIEN_BULLETS
    alien_bullet_x resd MAX_ALIEN_BULLETS
    alien_bullet_y resd MAX_ALIEN_BULLETS
    current_wave resd 1
    active_enemies resd 1
    enemy_array resb (MAX_ENEMIES * ENEMY_STRUC_SIZE)
    attack_timer resd 1
    spawn_timer resd 1

section .text

; ------------------------------------------------------------------------------
; init_wave
; ------------------------------------------------------------------------------
init_wave:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    ; Calcular filas y columnas dinámicamente según la wave
    ; Progreso suave que empieza con poca cantidad (6 enemigos: 2 filas x 3 columnas)
    mov eax, dword [current_wave]
    mov edx, 0
    mov ebx, 3
    div ebx                 ; eax = wave / 3 (aumento de filas), edx = wave % 3 (aumento de columnas)
    
    mov r8d, eax
    add r8d, 2              ; filas (comienza en 2)
    
    mov r9d, edx
    add r9d, 3              ; columnas (comienza en 3)
    
    mov r14d, r9d           ; Guardar columnas en r14d para el límite de rejilla
    
    ; active_enemies = filas * columnas
    mov eax, r8d
    imul eax, r9d
    cmp eax, MAX_ENEMIES
    jle .num_ok
    mov eax, MAX_ENEMIES
.num_ok:
    mov dword [active_enemies], eax
    mov r12d, eax           ; Guardar en r12d para el loop
    
    ; Inicializar temporizadores globales
    mov dword [rel attack_timer], 120   ; Ataques cada 2 segundos
    mov dword [rel spawn_timer], 0      ; Aparición inmediata del primero
    
    ; Velocidad base
    mov eax, dword [current_wave]
    shr eax, 2
    add eax, 1
    mov r13d, eax           ; Base speed
    
    mov rcx, 0      ; Índice general de enemigos
    mov r8, 0       ; Fila actual
    mov r9, 0       ; Columna actual
    lea rdi, [rel enemy_array]
.init_loop:
    cmp rcx, r12
    jge .done
    
    ; STATUS = 3 (Esperando aparición gradual)
    mov byte [rdi + ENEMY_OFFSET_STATUS], 3
    
    ; Patrones individuales alternados
    mov eax, ecx
    and eax, 1      ; 0: DIAGONAL, 1: SINE
    mov byte [rdi + ENEMY_OFFSET_PATTERN], al
    
    ; Posición X inicial según columna
    mov eax, ENEMY_WIDTH
    add eax, ENEMY_PADDING_X
    imul eax, r9d
    add eax, ENEMY_START_X
    mov dword [rdi + ENEMY_OFFSET_X], eax
    
    ; Posición Y inicial según fila
    mov eax, ENEMY_HEIGHT
    add eax, ENEMY_PADDING_Y
    imul eax, r8d
    add eax, ENEMY_START_Y
    mov dword [rdi + ENEMY_OFFSET_Y], eax
    
    ; Velocidad X individual basada en fila/columna para dar independencia
    mov eax, r13d
    mov ebx, r8d
    and ebx, 1
    add eax, ebx
    
    mov ebx, r9d
    and ebx, 1
    jz .set_vel_x
    neg eax
.set_vel_x:
    mov dword [rdi + ENEMY_OFFSET_VEL_X], eax
    
    ; Velocidad Y base
    mov dword [rdi + ENEMY_OFFSET_VEL_Y], 1
    
    ; Fase inicial para el patrón Sine
    mov eax, r9d
    imul eax, 4
    mov ebx, r8d
    imul ebx, 8
    add eax, ebx
    and eax, 63
    mov dword [rdi + ENEMY_OFFSET_PHASE], eax
    
    add rdi, ENEMY_STRUC_SIZE
    inc rcx
    
    inc r9
    cmp r9, r14     ; Limitar a las columnas calculadas dinámicamente
    jl .init_loop
    mov r9, 0
    inc r8
    jmp .init_loop
    
.done:
.clear_loop:
    cmp rcx, MAX_ENEMIES
    jge .exit_init
    mov byte [rdi + ENEMY_OFFSET_STATUS], 0
    add rdi, ENEMY_STRUC_SIZE
    inc rcx
    jmp .clear_loop
    
.exit_init:
    inc dword [current_wave]
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

; ------------------------------------------------------------------------------
; update_enemies
; ------------------------------------------------------------------------------
update_enemies:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8              ; Alinear pila a 16 bytes (System V ABI)

    ; --- Lógica de aparición gradual (Spawning Cascading) ---
    cmp dword [rel spawn_timer], 0
    jle .check_spawn
    dec dword [rel spawn_timer]
    jmp .no_spawn
    
.check_spawn:
    mov rdx, 0
    lea rax, [rel enemy_array]
.spawn_search_loop:
    cmp edx, dword [rel active_enemies]
    jge .no_spawn
    
    cmp byte [rax + ENEMY_OFFSET_STATUS], 3     ; ¿En espera?
    je .activate_enemy
    
    add rax, ENEMY_STRUC_SIZE
    inc edx
    jmp .spawn_search_loop
    
.activate_enemy:
    mov byte [rax + ENEMY_OFFSET_STATUS], 1     ; Activar en formación
    mov dword [rel spawn_timer], 12             ; Esperar 12 frames (~0.2 segundos) para el siguiente
    
.no_spawn:

    ; --- Mecánica de ataque por oleadas ---
    dec dword [rel attack_timer]
    cmp dword [rel attack_timer], 0
    jg .no_attack_trigger
    
    mov dword [rel attack_timer], 120   ; Disparar ataque cada ~2 segundos
    call launch_diver

.no_attack_trigger:
    mov rcx, 0
    lea r9, [rel enemy_array]
.enemy_loop:
    cmp ecx, dword [active_enemies]
    jge .check_wave_end
    
    ; Si está muerto (0) o esperando spawn (3), no se actualizan físicas/movimiento
    mov al, byte [r9 + ENEMY_OFFSET_STATUS]
    cmp al, 1
    je .do_update
    cmp al, 2
    je .do_update
    jmp .next_enemy
    
.do_update:
    movzx eax, byte [r9 + ENEMY_OFFSET_PATTERN]
    lea r15, [rel pattern_jump_table]
    jmp [r15 + rax*8]

.pattern_diagonal:
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_X]
    add dword [r9 + ENEMY_OFFSET_X], eax
    
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_Y]
    add dword [r9 + ENEMY_OFFSET_Y], eax
    
    mov r10d, dword [r9 + ENEMY_OFFSET_X]
    cmp r10d, 0
    jle .bounce_x
    mov eax, r10d
    add eax, ENEMY_WIDTH
    cmp eax, WINDOW_WIDTH
    jge .bounce_x
    jmp .apply_y

.bounce_x:
    neg dword [r9 + ENEMY_OFFSET_VEL_X]
    cmp r10d, 0
    jle .fix_x_left
    mov dword [r9 + ENEMY_OFFSET_X], WINDOW_WIDTH - ENEMY_WIDTH
    jmp .apply_y
.fix_x_left:
    mov dword [r9 + ENEMY_OFFSET_X], 0
    jmp .apply_y

.pattern_sine:
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_Y]
    add dword [r9 + ENEMY_OFFSET_Y], eax
    
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_X]
    add dword [r9 + ENEMY_OFFSET_X], eax
    
    mov r10d, dword [r9 + ENEMY_OFFSET_X]
    cmp r10d, 0
    jle .bounce_sine
    mov eax, r10d
    add eax, ENEMY_WIDTH
    cmp eax, WINDOW_WIDTH
    jge .bounce_sine
    jmp .apply_y
    
.bounce_sine:
    neg dword [r9 + ENEMY_OFFSET_VEL_X]
    jmp .apply_y

.pattern_kamikaze:
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_Y]
    add dword [r9 + ENEMY_OFFSET_Y], eax
    
    mov eax, dword [player_x]
    cmp eax, dword [r9 + ENEMY_OFFSET_X]
    je .apply_y
    jg .move_right
    
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_X]
    sub dword [r9 + ENEMY_OFFSET_X], eax
    jmp .apply_y
.move_right:
    mov eax, dword [r9 + ENEMY_OFFSET_VEL_X]
    add dword [r9 + ENEMY_OFFSET_X], eax

.apply_y:
    ; --- Detección de límite inferior (Screen Wrap) ---
    mov eax, dword [r9 + ENEMY_OFFSET_Y]
    cmp eax, WINDOW_HEIGHT
    jl .no_wrap
    
    ; Reaparecer arriba
    mov dword [r9 + ENEMY_OFFSET_Y], -20
    
    ; Si era un bucle de ataque (Diver), regresarlo a su comportamiento normal en la formación
    cmp byte [r9 + ENEMY_OFFSET_STATUS], 2
    jne .no_diver_reset
    
    mov byte [r9 + ENEMY_OFFSET_STATUS], 1
    
    ; Restaurar patrón original (rcx % 2) y velocidad estándar
    mov eax, ecx
    and eax, 1
    mov byte [r9 + ENEMY_OFFSET_PATTERN], al
    mov dword [r9 + ENEMY_OFFSET_VEL_Y], 1
    
    ; Velocidad X normal (restaurar a velocidad base lenta)
    mov eax, dword [rel current_wave]
    shr eax, 2
    add eax, 1
    mov ebx, ecx
    and ebx, 1
    jz .set_vel_x_reset
    neg eax
.set_vel_x_reset:
    mov dword [r9 + ENEMY_OFFSET_VEL_X], eax
    
.no_diver_reset:
    ; Randomizar la posición X en el cielo para dar dinamismo
    rdtsc
    mov edx, 0
    mov ebx, WINDOW_WIDTH - ENEMY_WIDTH
    div ebx
    mov dword [r9 + ENEMY_OFFSET_X], edx

.no_wrap:
    cmp byte [r9 + ENEMY_OFFSET_PATTERN], PATTERN_SINE
    jne .next_enemy
    
    mov eax, dword [r9 + ENEMY_OFFSET_PHASE]
    inc eax
    and eax, 63
    mov dword [r9 + ENEMY_OFFSET_PHASE], eax

.next_enemy:
    add r9, ENEMY_STRUC_SIZE
    inc rcx
    jmp .enemy_loop

.check_wave_end:
    mov rcx, 0
    mov r8d, 0
    lea r9, [rel enemy_array]
.count_loop:
    cmp ecx, dword [active_enemies]
    jge .check_count
    cmp byte [r9 + ENEMY_OFFSET_STATUS], 0      ; Cuenta como vivo si STATUS != 0 (incluyendo en espera: 3)
    je .skip_count
    inc r8d
.skip_count:
    add r9, ENEMY_STRUC_SIZE
    inc rcx
    jmp .count_loop
    
.check_count:
    cmp r8d, 0
    jg .done
    call init_wave

.done:
    add rsp, 8              ; Restaurar alineación de la pila
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

; ------------------------------------------------------------------------------
; launch_diver
; Escanea un enemigo vivo en formación y lo desprende como atacante independiente
; ------------------------------------------------------------------------------
launch_diver:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    
    mov ebx, 55
    cmp ebx, 0
    je .done
    
    ; Obtener un índice pseudo-aleatorio inicial usando el TSC
    rdtsc
    mov edx, 0
    div ebx                 ; edx = TSC % active_enemies
    
    mov ecx, 0              ; Contador de iteraciones de búsqueda
    mov r12d, edx           ; Candidato actual
    
.search_loop:
    cmp ecx, 55
    jge .done
    
    mov eax, r12d
    imul eax, ENEMY_STRUC_SIZE
    lea r13, [rel enemy_array]
    add r13, rax
    
    ; Solo desprender si está activo y en formación standard
    cmp byte [r13 + ENEMY_OFFSET_STATUS], 1
    je .launch
    
    ; Siguiente candidato
    inc r12d
    cmp r12d, 55
    jl .no_index_wrap
    mov r12d, 0
.no_index_wrap:
    inc ecx
    jmp .search_loop
    
.launch:
    ; Cambiar estado a 2 (Diver/Atacando) y patrón a Kamikaze
    mov byte [r13 + ENEMY_OFFSET_STATUS], 2
    mov byte [r13 + ENEMY_OFFSET_PATTERN], PATTERN_KAMIKAZE
    
    ; Incrementar velocidades para hacerlo agresivo e independiente (reducidas de 3 a 2 para mayor suavidad)
    mov dword [r13 + ENEMY_OFFSET_VEL_Y], 2
    mov dword [r13 + ENEMY_OFFSET_VEL_X], 2

.done:
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
