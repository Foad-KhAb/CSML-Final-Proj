section .text
    global SA
    SA:  ; StartAngle(xmm0 -> float rotationAngle, xmm1 -> float segment)
        push rbp
        mov rbp, rsp

        ; Add the two float arguments (rotationAngle + segment)
        addss xmm0, xmm1      ; Add scalar single-precision float values

        leave
        ret

section	.note.GNU-stack