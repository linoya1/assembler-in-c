#include "opcode_table.h"
#include "constants.h"
#include <string.h>
#include <stdio.h>

int get_opcode(const char *name) {
    int i;	
    /* Check if the name ends with ':' which indicates it might be a label, not an opcode */
if (name[strlen(name) - 1] == ':') {
        return -1;  
    }

    /* Search through the opcode table to find a match by name */
    for (i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (strcmp(opcode_table[i].name, name) == 0) {
            /* Return the corresponding opcode code if found */
            return opcode_table[i].code;
        }
    }
    /* Return -1 if the opcode name is not found in the table */
    return -1; 
}

int get_register_code(const char *reg) {
    /* Check each register name against the input and return the corresponding code */
    if (strcmp(reg, "r0") == 0 || (reg[0] == 'r' && reg[1] == '0' && reg[2] == '\0')) return 0;
    if (strcmp(reg, "r1") == 0 || (reg[0] == 'r' && reg[1] == '1' && reg[2] == '\0')) return 1;
    if (strcmp(reg, "r2") == 0 || (reg[0] == 'r' && reg[1] == '2' && reg[2] == '\0')) return 2;
    if (strcmp(reg, "r3") == 0 || (reg[0] == 'r' && reg[1] == '3' && reg[2] == '\0')) return 3;
    if (strcmp(reg, "r4") == 0 || (reg[0] == 'r' && reg[1] == '4' && reg[2] == '\0')) return 4;
    if (strcmp(reg, "r5") == 0 || (reg[0] == 'r' && reg[1] == '5' && reg[2] == '\0')) return 5;
    if (strcmp(reg, "r6") == 0 || (reg[0] == 'r' && reg[1] == '6' && reg[2] == '\0')) return 6;
    if (strcmp(reg, "r7") == 0 || (reg[0] == 'r' && reg[1] == '7' && reg[2] == '\0')) return 7;
    return -1; /* Invalid register - return -1 if no valid register match */
}

