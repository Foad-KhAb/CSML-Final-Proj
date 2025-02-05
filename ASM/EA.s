section .data
    PI dd 3.1415926535
    THREE dd 3.0        ; Add this constant as a float

section .text
    global EA
    EA:  ; EndAngle(xmm0 -> float rotationAngle, xmm1 -> float segment)
        push rbp
        mov rbp, rsp

        ; Add the two float arguments (rotationAngle + segment)
        addss xmm0, xmm1      ; Add scalar single-precision float values
        
        ; Calculate PI/3
        movss xmm2, [PI]      ; Load PI (32-bit float)
        movss xmm3, [THREE]   ; Load 3.0 into xmm3
        divss xmm2, xmm3      ; PI/3
        
        ; Add PI/3 to the previous sum
        addss xmm0, xmm2      ; result = rotationAngle + segment + (PI/3)

        leave
        ret

section	.note.GNU-stack