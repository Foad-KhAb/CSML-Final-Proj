section .data
    FPS dd 60.0           ; Define FPS as a 32-bit float
    frequency dd 0.05     ; Define frequency constant

section .text
    global S
    S:  ; SinPath(rdi -> int velocity, rsi -> int time)
        push rbp
        mov rbp, rsp
        sub rsp, 8        ; Reserve space for local variables
        
        ; Calculate baseMovement (velocity / FPS)
        cvtsi2ss xmm0, edi        ; Convert velocity to float
        divss xmm0, [rel FPS]     ; Divide by FPS
        movss [rsp], xmm0         ; Store baseMovement
        
        ; Calculate angle = frequency * time
        cvtsi2ss xmm0, esi        ; Convert time to float
        mulss xmm0, [rel frequency] ; Multiply by frequency
        
        ; Convert from SSE to x87 for FSIN
        movss [rsp+4], xmm0
        fld dword [rsp+4]         ; Load angle to FPU stack
        fsin                      ; Calculate sine
        fstp dword [rsp+4]        ; Store result back
        movss xmm0, [rsp+4]       ; Load result to XMM register
        
        ; Multiply result by baseMovement
        mulss xmm0, [rsp]         ; Multiply by stored baseMovement
        
        add rsp, 8                ; Restore stack
        leave
        ret

section	.note.GNU-stack