#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H
#include "constants.h"
/**
 * NAME: LINOY BITON
 * ID:211589775 */

/**
 * Structure representing an opcode, including its name, code, and allowed addressing modes.
 */
typedef struct {
    char *name;
    int code;
    char *source_modes;
    char *dest_modes;
} Opcode;

/* Array containing the opcodes and their properties */
static const Opcode opcode_table[] = {
    {"mov", 0, "0123", "123"},
    {"cmp", 1, "0123", "0123"},
    {"add", 2, "0123", "123"},
    {"sub", 3, "0123", "123"},
    {"lea", 4, "012", "123"},
    {"clr", 5, "", "123"},
    {"not", 6, "", "123"},
    {"inc", 7, "", "123"},
    {"dec", 8, "", "123"},
    {"jmp", 9, "", "123"},
    {"bne", 10, "", "123"},
    {"red", 11, "", "123"},
    {"prn", 12, "0123", "123"},
    {"jsr", 13, "", "123"},
    {"rts", 14, "", ""},
    {"stop", 15, "", ""}
};

#define OPCODE_TABLE_SIZE (sizeof(opcode_table) / sizeof(opcode_table[0]))

/**
 * Retrieves the opcode code based on its name.
 * 
 * @param name The name of the opcode as a string (e.g., "mov").
 * @return The numerical code associated with the opcode if found, otherwise -1.
 */
int get_opcode(const char *name);

/**
 * Retrieves the register code based on its name.
 * 
 * @param reg The name of the register as a string (e.g., "r1").
 * @return The numerical code associated with the register if valid, otherwise -1.
 */
int get_register_code(const char *reg);

#endif
