#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include "constants.h"
#include "symbol_table.h"
#include "syntax_analyzer.h"

/**
 * NAME: LINOY BITON
 * ID:211589775 */

/* Define dynamic data structures for instructions and data */

typedef struct InstructionNode {
    int instruction;
    struct InstructionNode *next;
} InstructionNode;

typedef struct DataNode {
    int data;
    struct DataNode *next;
} DataNode;

/* Function declarations for managing dynamic data structures */

/**
 * Adds a data node to the dynamic data list.
 * 
 * @param data The data value to be added.
 */
void add_data(int data);

/**
 * Frees the memory allocated for the instructions list.
 */
void free_instructions();

/**
 * Frees the memory allocated for the data list.
 */
void free_data();

/* Functions for the first pass */

/**
 * Reads and processes an entry or extern directive from a line.
 * 
 * @param str The line to process.
 * @param symbol_table The symbol table to update.
 * @param line The current line context.
 * @return A pointer to the Directive if successful, NULL otherwise.
 */
Directive *read_entry_or_extern(char *str, Symbol **symbol_table, Line *line);

/**
 * Performs the first pass of the assembly process.
 * 
 * This function reads each line from the input assembly file, processes labels, 
 * directives, and instructions, and updates the symbol table and error lists accordingly. 
 * It sets initial values for the Instruction Counter (IC) and Data Counter (DC) 
 * and checks for errors such as invalid labels or unknown instructions.
 * 
 * @param input_file A file pointer to the assembly input file.
 * @param symbol_table A double pointer to the symbol table where labels are stored.
 * @param error_list A double pointer to the error list where errors are reported.
 * @param warning_list A double pointer to the warning list where warnings are reported.
 * @param finalIC A pointer to the final instruction counter value.
 * @param finalDC A pointer to the final data counter value.
 * @return An integer flag indicating if any errors were encountered during the pass (0 if no errors, 1 otherwise).
 */
int first_pass(FILE *input_file, Symbol **symbol_table, Error **error_list, Warning **warning_list, int* finalIC, int* finalDC);

/**
 * Saves the current symbol table to another memory table.
 * 
 * @param symbol_table The current symbol table.
 * @param symbol_memory_table The target memory table for symbols.
 */
void save_symbol_table(Symbol **symbol_table, Symbol **symbol_memory_table);

/**
 * Checks symbol definitions for errors during the first pass.
 * 
 * @param symbol_table The symbol table to check.
 * @param line The current line context.
 */
void check_symbol_definitions(Symbol *symbol_table, Line *line);

/**
 * Checks if entry labels are defined correctly in the symbol table.
 * 
 * This function iterates through the symbol table and checks if each
 * entry label has a corresponding definition or if there are inconsistencies
 * with external definitions. If an entry label is not defined correctly, 
 * an error is added to the current line's error list.
 * 
 * @param symbol_table A pointer to the symbol table to be checked.
 * @param current_line A pointer to the current line context for error reporting.
 */
void check_entry_labels(Symbol **symbol_table, Line *current_line);

/**
 * Checks if a symbol is already defined in the first pass.
 * 
 * @param symbol_name The name of the symbol to check.
 * @param symbol_table The symbol table to search.
 * @return 1 if the symbol is defined, 0 otherwise.
 */
int symbol_already_defined_in_pass(const char *symbol_name, Symbol *symbol_table);

#endif

