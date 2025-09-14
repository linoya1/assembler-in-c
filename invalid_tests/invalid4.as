MAIN: add r3,LIST 
LOOP: prn #100

cmp r3, #-4
bne END 
endmacr
lea STR, r6
inc r6
mov *r6,K 
sub r1, r4 
m_macr
dec K
jmp LOOP 
END: stop
STR: .string "lpoy"
LIST: .data 6, -9
	.data -100
K:	.data  31

