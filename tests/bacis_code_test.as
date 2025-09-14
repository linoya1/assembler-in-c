;Test case- 3
;Description- Basic assembly code without macros

START: mov r3, SIZE
CYCLE: jmp LABEL1
prn #24
LABEL1: inc COUNTER
bne CYCLE
FINISH: stop
SIZE: .data 8, -12, 20
COUNTER: .data 30
TEXT: .string "ghijkl"

