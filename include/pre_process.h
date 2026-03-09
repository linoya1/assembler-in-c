#ifndef PRE_PROCESS_H
#define PRE_PROCESS_H

#include "constants.h"
#include <stdio.h>


/**
 * Structure representing a macro in the pre-processing stage.
 */
typedef struct Macro {
    char name[MAX_MACRO_NAME + 1];
    char *body;
    struct Macro *next;
} Macro;

/**
 * Adds a new macro to the macro list.
 * 
 * @param head A double pointer to the head of the macro list.
 * @param name The name of the macro to be added.
 * @param body The body of the macro as a string.
 */
void add_macro(Macro **head, const char *name, const char *body);

/**
 * Retrieves the body of a macro based on its name.
 * 
 * @param head A pointer to the head of the macro list.
 * @param name The name of the macro to retrieve.
 * @return A pointer to the body of the macro if found, otherwise NULL.
 */
char* get_macro_body(Macro *head, const char *name);

/**
 * Frees all macros in the list and releases allocated memory.
 * 
 * @param head A pointer to the head of the macro list to be freed.
 */
void free_macros(Macro *head);

/**
 * Checks if a given name is a valid macro name.
 * 
 * @param name The name to validate.
 * @return 1 if the name is valid, 0 otherwise.
 */
int is_valid_macro_name(const char *name);

/**
 * Checks if a given name is an instruction name.
 * 
 * @param name The name to check.
 * @return 1 if the name is an instruction, 0 otherwise.
 */
int is_instruction_name(const char *name);

/**
 * Processes an assembly file for macros and outputs the pre-processed file.
 * 
 * @param input_file A pointer to the input file.
 * @param output_file A pointer to the output file.
 * @param input_filename The name of the input file.
 * @return 0 if successful, 1 if an error occurred.
 */
int pre_process(FILE *input_file, FILE *output_file, const char *input_filename);

/**
 * Main function for the pre-processing stage.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return 0 if successful, 1 if an error occurred.
 */
int pre_process_main(int argc, char *argv[]);

#endif 
