default rel
%include "game.inc"

section .bss
    
    sdl_event resb 64       ; Estructura SDL_Event (mínimo 56 bytes)
    keyboard_state resq 1   ; Puntero al arreglo de estado del teclado

section .text

; ------------------------------------------------------------------------------
; init_input
; ------------------------------------------------------------------------------
init_input:
    push rbp
    mov rbp, rsp
    
    mov rdi, 0
    call SDL_GetKeyboardState
    mov [keyboard_state], rax
    
    pop rbp
    ret

; ------------------------------------------------------------------------------
; poll_events
; ------------------------------------------------------------------------------
poll_events:
    push rbp
    mov rbp, rsp

.poll_loop:
    lea rdi, [rel sdl_event]
    call SDL_PollEvent
    cmp eax, 0
    je .done            ; Si no hay eventos, terminar
    
    lea rcx, [rel sdl_event]
    mov eax, dword [rcx]
    
    cmp eax, SDL_QUIT
    je .handle_quit

    jmp .poll_loop

.handle_quit:
    mov byte [running], 0
    jmp .poll_loop

.done:
    pop rbp
    ret
