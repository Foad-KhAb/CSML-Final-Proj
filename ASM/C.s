section .data
    constant dd 1000.0        ; Equivalent to const float constant = 1000
    screen_width dd 1280.0    ; Assuming SCREEN_WIDTH is 1280
    screen_height dd 720.0    ; Assuming SCREEN_HEIGHT is 720
    min_norm dd 25.0         ; Minimum norm threshold
    zero dd 0.0              ; For returning 0
    two dd 2.0              ; For division by 2

section .text
    global C
    C:  ; CurvePath(rdi -> int positionX, rsi -> int positionY)
        push rbp
        mov rbp, rsp
        
        ; Convert integer inputs to float
        cvtsi2ss xmm0, edi     ; positionX to float
        cvtsi2ss xmm1, rsi     ; positionY to float
        
        ; Calculate and subtract SCREEN_WIDTH/2 and SCREEN_HEIGHT/2
        movss xmm2, [rel screen_width]
        divss xmm2, [rel two]      ; SCREEN_WIDTH/2
        subss xmm0, xmm2           ; positionX -= SCREEN_WIDTH/2
        
        movss xmm2, [rel screen_height]
        divss xmm2, [rel two]      ; SCREEN_HEIGHT/2
        subss xmm1, xmm2           ; positionY -= SCREEN_HEIGHT/2
        
        ; Calculate norm = positionX * positionX + positionY * positionY
        movss xmm2, xmm0       ; Copy positionX
        mulss xmm2, xmm0       ; positionX * positionX
        movss xmm3, xmm1       ; Copy positionY
        mulss xmm3, xmm1       ; positionY * positionY
        addss xmm2, xmm3       ; norm = x^2 + y^2
        
        ; Check if norm < 25
        comiss xmm2, [rel min_norm]
        jb return_zero
        
        ; Calculate constant * positionY / norm
        movss xmm0, [rel constant]  ; Load 1000.0
        mulss xmm0, xmm1           ; constant * positionY
        divss xmm0, xmm2           ; Divide by norm
        
        jmp end
        
    return_zero:
        movss xmm0, [rel zero]
        
    end:
        leave
        ret

section	.note.GNU-stack