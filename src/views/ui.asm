default rel
%include "game.inc"

section .data
    score dd 0
    high_score dd 0

section .rodata
    font_lut db 14, 17, 17, 17, 14  ; 0
             db 4,  12, 4,  4,  14  ; 1
             db 14, 17, 2,  4,  31  ; 2
             db 30, 1,  14, 1,  30  ; 3
             db 2,  6,  10, 31, 2   ; 4
             db 31, 16, 30, 1,  30  ; 5
             db 6,  8,  30, 17, 14  ; 6
             db 31, 1,  2,  4,  4   ; 7
             db 14, 17, 14, 17, 14  ; 8
             db 14, 17, 15, 1,  14  ; 9

section .bss
    ; Variable temporal interna para dibujar rectángulos de la fuente
    ui_rect resd 4

section .text

; ------------------------------------------------------------------------------
; render_score
; In: rdi = renderer
; ------------------------------------------------------------------------------
render_score:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 56         ; Buffer local para dígitos (hasta 10) y alinear stack a 16 bytes
    
    ; Dibujar texto en color Blanco
    push rdi
    push rax            ; dummy push para alinear
    mov rsi, 255
    mov rdx, 255
    mov rcx, 255
    mov r8, 255
    call SDL_SetRenderDrawColor
    pop rax
    pop rdi
    
    ; Extraer dígitos
    mov eax, dword [score]
    mov rbx, 10
    mov r12, 0          ; Contador de dígitos
    
.extract_digit:
    mov edx, 0
    div ebx
    mov dword [rsp + r12*4], edx
    inc r12
    cmp eax, 0
    jg .extract_digit
    
    mov r13d, SCORE_DRAW_X
    
.draw_digit:
    mov r14d, r12d
    dec r14d
    mov r14d, dword [rsp + r14*4]
    
    lea r15, [rel font_lut]
    mov rax, r14
    imul rax, 5
    add r15, rax
    
    mov r9, 0
.draw_row:
    cmp r9, 5
    jge .next_digit
    
    mov al, byte [r15 + r9]
    
    mov r10, 0
.draw_col:
    cmp r10, 5
    jge .next_row
    
    mov bl, al
    mov cl, 4
    sub cl, r10b
    shr bl, cl
    and bl, 1
    cmp bl, 0
    je .skip_pixel
    
    mov r11d, r10d
    imul r11d, FONT_PIXEL_SIZE
    add r11d, r13d
    mov dword [ui_rect], r11d
    
    mov r11d, r9d
    imul r11d, FONT_PIXEL_SIZE
    add r11d, SCORE_DRAW_Y
    mov dword [ui_rect + 4], r11d
    
    mov dword [ui_rect + 8], FONT_PIXEL_SIZE
    mov dword [ui_rect + 12], FONT_PIXEL_SIZE
    
    push rdi
    push r9
    push r10
    push rax
    
    mov rsi, ui_rect
    call SDL_RenderFillRect
    
    pop rax
    pop r10
    pop r9
    pop rdi
    
.skip_pixel:
    inc r10
    jmp .draw_col
    
.next_row:
    inc r9
    jmp .draw_row

.next_digit:
    mov eax, FONT_CHAR_WIDTH
    inc eax
    imul eax, FONT_PIXEL_SIZE
    add r13d, eax
    
    dec r12
    cmp r12, 0
    jg .draw_digit

    add rsp, 56
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
