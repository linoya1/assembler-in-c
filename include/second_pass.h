#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include "constants.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "syntax_analyzer.h"

/**
 * NAME: LINOY BITON
 * ID:211589775 */

typedef struct {
    int code_image[MAX_MEM_SIZE];
    int IC;
    int data_image[MAX_MEM_SIZE];
    int DC;
    Symbol *symbol_table;
    int symCount;
} TranslationUnit;

/**
 * @brief Processes a single line of assembly code and updates the Line struct.
 * 
 * @param line The line of assembly code to process.
 * @param result Pointer to a Line struct to store the parsed result.
 * @param line_number The line number in the source file, used for error reporting.
 */
void process_line(char *line, Line *result, int line_number);

/**
 * @brief Creates a duplicate of the input string.
 * 
 * @param str The input string to duplicate.
 * @return A pointer to the newly allocated string or NULL if allocation fails.
 */
char *duplicate_string(const char *str);

/**
 * @brief Adds or updates a symbol's address in the symbol table.
 * 
 * @param symbol_table The symbol table to update.
 * @param name The name of the symbol.
 * @param address The address to assign to the symbol.
 * @param type The type of the symbol (code, data, extern, entry).
 * @return 0 on success, or an error code on failure.
 */
int add_symbol_address(Symbol **symbol_table, const char *name, int address, SymbolType type);

/**
 * @brief Converts an integer value to a binary string representation.
 * 
 * @param value The integer value to convert.
 * @param numBits The number of bits to represent the value.
 * @param result The string to store the binary representation (must be large enough).
 */
void toBinary(int value, int numBits, char *result);

/**
 * @brief Generates the binary representation of an operand.
 * 
 * @param op The operand to represent.
 * @param symbol_table The symbol table used for resolving symbols.
 * @param IC The current instruction counter value.
 * @param is_dest Flag indicating if the operand is a destination (1) or source (0).
 * @return The binary representation of the operand as an integer.
 */
int get_operand_binary_representation(Operand *op, Symbol *symbol_table, int IC, int is_dest);

/**
 * @brief Writes the binary representation of an instruction to the output file.
 * 
 * @param instruction The instruction to encode.
 * @param symbol_table The symbol table used for resolving operands.
 * @param IC Pointer to the instruction counter; updated during the encoding process.
 * @param output_file The file to write the encoded instruction to.
 */
void write_instruction_binary(InstructionParts *instruction, Symbol *symbol_table, int *IC, FILE *output_file);

/**
 * @brief Parses an instruction line and generates its binary representation.
 * 
 * @param line The Line struct containing the instruction to parse.
 * @param symbol_table The symbol table used for resolving operands.
 * @param IC Pointer to the instruction counter; updated during the encoding process.
 * @param output_file The file to write the encoded instruction to.
 */
void parse_instruction(Line *line, Symbol *symbol_table, int *IC, FILE *output_file);

/**
 * @brief Processes the second pass of the assembly, generating the final code image.
 * 
 * @param input_file The input file containing the assembly code.
 * @param output_file The output file to write the binary code to.
 * @param unit The TranslationUnit containing the code and symbol tables.
 * @return 0 on success, or an error code on failure.
 */
int process_second_pass(FILE *input_file, FILE *output_file, TranslationUnit *unit);

/**
 * @brief Writes the output files (.ent and .ext) based on the processed symbols.
 * 
 * @param unit The TranslationUnit containing the symbol table and addresses.
 * @param basename The base name for the output files (without extension).
 * @return 0 on success, or an error code on failure.
 */
int write_output_files(TranslationUnit *unit, const char *basename);

/**
 * @brief Creates and initializes a new TranslationUnit.
 * 
 * @return A pointer to the newly created TranslationUnit, or NULL if allocation fails.
 */
TranslationUnit *create_translation_unit();


/**
 * @brief Frees the resources associated with a TranslationUnit.
 * 
 * @param unit The TranslationUnit to free.
 */
void free_translation_unit(TranslationUnit *unit);

/**
 * @brief Executes the second pass of the assembler, generating binary code and writing output files.
 * 
 * @param input_file The input file containing the assembly code.
 * @param basename The base name for the output files (without extension).
 * @param symbol_table The symbol table generated during the first pass.
 * @param output_file The file to write the encoded binary instructions.
 * @return 0 on success, or an error code on failure.
 */
int second_pass(FILE *input_file, const char *basename, Symbol *symbol_table, FILE *output_file);

/**
 * @brief Converts binary machine code from a file into octal format.
 * 
 * @param binary_filename The name of the file containing the binary code.
 * @param octal_filename The name of the file to write the octal code to.
 */
void convert_binary_to_octal(const char *binary_filename, const char *octal_filename);

/**
 * @brief Processes a directive in the assembly code, updating the translation unit as needed.
 * 
 * @param line The Line struct containing the directive to process.
 * @param symbol_table The symbol table for resolving symbols.
 * @param unit The TranslationUnit used for managing the code and data segments.
 * @param IC Pointer to the instruction counter; updated during processing.
 * @param output_file The file to write any relevant output to.
 */
void process_directive(Line *line, Symbol *symbol_table, TranslationUnit *unit, int* IC, FILE *output_file);

#endif

