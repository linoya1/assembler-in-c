MAIN: add r3,LIST 
LOOP: prn #98
macr  
cmp r3, #-7
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
STR: .string "axw"
LIST: .data 6, -9
	.data -100
K:	.data  31

