section .data
    constant dd 0.5
    radius dd 128.0  ; Changed to dd and added .0 to make it a float
    max_color dd 255.0  ; Maximum color value as float

section .text
    global G
    G:  ; Gradient(rdi -> color component, xmm0 -> i)
        push rbp
        mov rbp, rsp
        
        ; Load radius from data section instead of using rsi
        movss xmm1, [rel radius]  ; Load radius directly as float
        
        ; Convert color to float
        cvtsi2ss xmm2, edi        ; color component to float in xmm2
        
        ; Calculate (radius - i)
        subss xmm1, xmm0          ; xmm1 = radius - i
        
        ; Multiply by 0.5
        mulss xmm1, [rel constant] ; xmm1 = (radius - i) * 0.5
        
        ; Add to original color
        addss xmm1, xmm2          ; xmm1 = color + (radius - i) * 0.5
        
        ; Compare with 255 and take minimum
        minss xmm1, [rel max_color]
        
        ; Convert result back to integer
        cvttss2si eax, xmm1       ; Convert float to integer with truncation
        
        leave
        ret

section	.note.GNU-stack