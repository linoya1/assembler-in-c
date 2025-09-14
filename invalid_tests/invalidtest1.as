macr validMac
cmp r3, #-10
endmacr

; extra text in macro initialization
macr macrname  *
cmp r3, #-10
bne END
endmacr
; ---------------------------------

; reserved word 'macr'
macr macr
cmp r3, #-10
endmacr

; ---------------------------------

; invalid macr name - reserved word
macr mov
LIST: .data 9000,0,-1
    .data 300
endmacr
; ---------------------------------

; invalid macro name - existing macro
macr validMacro
LIST: .data 9000,0,-1
    .data 300
endmacr
; ---------------------------------

; Line is too long
A: .string "ab "  kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
; ---------------------------------

; invalid macro ending
macr hi
LIST: .data 9000,0,-1
    .data 300
endmacr          hello:)
; ---------------------------------

