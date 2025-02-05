section .data
    FPS dd 60.0    ; Define FPS as a 32-bit float

section .text
    global R
    R: ; Regular(rdi -> int velocity)
        push rbp
        mov rbp, rsp
        
        ; Convert integer input to float
        cvtsi2ss xmm0, edi     ; Convert velocity to float using edi (first integer argument)
        
        ; Load FPS value and divide
        divss xmm0, [rel FPS]  ; Use RIP-relative addressing for FPS
        
        leave
        ret

section	.note.GNU-stack