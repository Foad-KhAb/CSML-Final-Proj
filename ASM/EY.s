section .text
    global EY
    EY:  ; EndY(xmm0 -> float positionY, xmm1 -> float radius, xmm2 -> float startAngle)
        push rbp
        mov rbp, rsp
        sub rsp, 8        ; Reserve space for local variables
        
        ; Store positionY temporarily
        movss [rsp], xmm0
        
        ; Convert startAngle (xmm2) to x87 for FCOS
        movss [rsp+4], xmm2
        fld dword [rsp+4]         ; Load angle to FPU stack
        fsin                      ; Calculate sine instead of sine
        fstp dword [rsp+4]        ; Store result back
        movss xmm0, [rsp+4]       ; Load sine result to XMM register
        
        ; Multiply by radius (xmm1)
        mulss xmm0, xmm1
        
        ; Add positionY
        addss xmm0, [rsp]
        
        add rsp, 8                ; Restore stack
        leave
        ret

section	.note.GNU-stack