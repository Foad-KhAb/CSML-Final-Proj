section .data
    PI dd 3.1415926535
    THREE dd 3.0        ; Add this constant as a float

section .text
    global SE
    SE: ; Segment(rdi -> int i) - Calculates segment angle in radians
        push rbp
        mov rbp, rsp
        
        ; Convert integer input to float
        cvtsi2ss xmm0, edi     ; Convert input angle to float
        
        ; Calculate: (input * PI) / 3
        mulss xmm0, [PI]       ; Multiply by PI
        movss xmm1, [THREE]    ; Load 3.0 into xmm1
        divss xmm0, xmm1       ; Divide by 3 to get segment angle
        
        leave
        ret

section	.note.GNU-stack