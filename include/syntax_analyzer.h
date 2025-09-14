#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include <stdio.h>
#include "constants.h"
#include "opcode_table.h"
#include "symbol_table.h"
#include "utility.h"

/**
 * NAME: LINOY BITON
 * ID:211589775 */

typedef struct Error {
    int line_number;
    char message[MAX_LINE_LENGTH];
} Error;

typedef struct Warning {
    int line_number;
    char message[MAX_LINE_LENGTH];
} Warning;


typedef enum {
    immediate_addressing = 0,
    direct_addressing = 1,
    indirect_register_addressing = 2,
    direct_register_addressing = 3,
    index_constant_addressing = 4,
    index_label_addressing = 5,
    no_operand = 6,
    unknown_addressing = -1
} OperandType;

typedef enum {
    data_dir, string_dir, entry_dir, extern_dir, unknown_dir
} DirectiveType;

typedef enum {
    empty, comment, directive, instruction
} LineType;

typedef struct {
    const char *label;
    int number;
} Definition;

typedef struct Operand {
    OperandType type;
    char *value;
    char *reg;
    struct Operand *next;
} Operand;

typedef struct {
    char *label;
    union {
        char *string; 
        struct {
            char *label;
            int number;
        } index_option;
        struct 
        {
            int* value;
            int count;
        } numbers;
    } data_options;
    DirectiveType directive_type;
} Directive;

typedef struct InstructionParts {
    char *label;
    int opcode;
    Operand *source;
    Operand *destination;
    OperandType source_type; 
    OperandType dest_type; 
} InstructionParts;

typedef struct Line {
    int line_number;
    char error[MAX_LINE_LENGTH];
    int error_count;
    Error *errors[MAX_ERRORS];
    int warning_count;  
    Warning *warnings[MAX_WARNINGS];
    InstructionParts instruction;  
    Directive directive;
    LineType line_type;
    struct Line *next;
} Line;

typedef struct {
    char *tokens[80];
    int token_count;
} TokenizedLine;

typedef enum {
    ABSOLUTE = 0,
    RELOCATABLE = 2,
    EXTERNAL = 1
} AddressingType;

typedef struct {
    unsigned int opcode : 4;        
    unsigned int source_mode : 2;   
    unsigned int dest_mode : 2;     
    unsigned int are : 3;           
    unsigned int unused : 5;        
} EncodedInstruction;

/**
 * @brief Adds an error to the given line.
 * 
 * @param line Pointer to the line structure where the error should be added.
 * @param line_number The line number where the error occurred.
 * @param message The error message to add.
 */
void add_error(Line *line, int line_number, const char *message);

/**
 * @brief Adds a warning to the given line.
 * 
 * @param line Pointer to the line structure where the warning should be added.
 * @param line_number The line number where the warning occurred.
 * @param message The warning message to add.
 */
void add_warning(Line *line, int line_number, const char *message);

/**
 * @brief Prints all errors associated with a line.
 * 
 * @param line Pointer to the line structure containing the errors.
 */
void print_errors(const Line *line);


/**
 * @brief Prints all warnings associated with a line.
 * 
 * @param line Pointer to the line structure containing the warnings.
 */
void print_warnings(const Line *line);

/**
 * @brief Frees memory allocated for errors in a line.
 * 
 * @param line Pointer to the line structure containing the errors.
 */
void free_errors(Line *line);

/**
 * @brief Frees memory allocated for warnings in a line.
 * 
 * @param line Pointer to the line structure containing the warnings.
 */
void free_warnings(Line *line);

/**
 * @brief Cleans an opcode string by trimming whitespace and special characters.
 * 
 * @param opcode The opcode string to clean.
 * @return A pointer to the cleaned opcode string.
 */
char *clean_opcode(char *opcode);

/**
 * @brief Tokenizes a line of text into individual tokens.
 * 
 * @param str The string to tokenize.
 * @return A structure containing the tokens and their count.
 */
TokenizedLine tokenize_line(char *str);

/**
 * @brief Validates if a string after a hash is a valid number.
 * 
 * @param str The string to validate.
 * @return 1 if valid, 0 otherwise.
 */
int validate_number_after_hash(const char *str);

/**
 * @brief Gets the type of an operand based on its format.
 * 
 * @param operand The operand string to evaluate.
 * @return The determined operand type.
 */
OperandType get_operand_type(char *operand);

/**
 * @brief Parses a single operand and validates its type.
 * 
 * @param operand The operand string to parse.
 * @param operand_type The type of the operand.
 * @param line Pointer to the line structure where errors will be recorded.
 */
void parse_operand(char *operand, OperandType operand_type, Line *line);

/**
 * @brief Parses operands for a given line, identifying source and destination.
 * 
 * @param operands The string containing operands.
 * @param line Pointer to the line structure where operands will be parsed.
 */
void parse_operands(char *operands, Line *line);

/**
 * @brief Validates if a string is a valid number within a specified range.
 * 
 * @param str The string to validate.
 * @param max The maximum allowed value.
 * @param min The minimum allowed value.
 * @param result Pointer to store the numeric result.
 * @return 0 if valid, 1 if not a number, 2 if out of range.
 */
int is_number(const char *str, int max, int min, int *result);


/**
 * @brief Checks if a name is a valid register.
 * 
 * @param name The name to check.
 * @return 1 if the name is a register, 0 otherwise.
 */
int is_register(const char *name);

/**
 * @brief Checks if a name matches an instruction.
 * 
 * @param name The name to check.
 * @return 1 if the name is an instruction, 0 otherwise.
 */
int is_instruction(const char *name);

/**
 * @brief Checks if a name matches a directive.
 * 
 * @param name The name to check.
 * @return 1 if the name is a directive, 0 otherwise.
 */
int is_directive(const char *name);

/**
 * @brief Checks if a name is a valid label.
 * 
 * @param name The label name to validate.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_label_name(const char *name);

/**
 * @brief Checks if a string is a valid string directive format.
 * 
 * @param str The string to validate.
 * @return 1 if valid, 0 otherwise.
 */
int is_valid_string(const char *str);

/**
 * @brief Parses a file into a linked list of lines, along with errors and warnings.
 * 
 * @param input_file The input file to parse.
 * @param line_list Pointer to the head of the linked list of lines.
 * @param error_list Pointer to the error list.
 * @param warning_list Pointer to the warning list.
 */
void parse_file(FILE *input_file, Line **line_list, Error **error_list, Warning **warning_list);

/**
 * @brief Outputs a word, typically to a file or console.
 * 
 * @param word The word to output.
 */
void output_word(int word);

/**
 * @brief Encodes an instruction into a machine code format.
 * 
 * @param instr Pointer to the instruction parts to encode.
 * @return The encoded instruction.
 */
EncodedInstruction encode_instruction(const InstructionParts *instr); 


/**
 * @brief Determines the A, R, E field value of an instruction.
 * 
 * @param instr Pointer to the instruction parts.
 * @return The addressing type (ABSOLUTE, RELOCATABLE, EXTERNAL).
 */
AddressingType get_are_value(const InstructionParts *instr);

/**
 * @brief Prints an encoded instruction in a readable format.
 * 
 * @param encoded_instr Pointer to the encoded instruction.
 * @param word The word value associated with the instruction.
 */
void print_encoded_instruction(const EncodedInstruction *encoded_instr, int word);

/**
 * @brief Gets the directive type based on the directive string.
 * 
 * @param directive The directive string to evaluate.
 * @return The directive type.
 */
DirectiveType get_directive_type(const char *directive);

/**
 * @brief Validates a .data directive line.
 * 
 * @param line_content The content of the line to validate.
 * @param line Pointer to the line structure where errors will be recorded.
 * @param line_number The line number for error reporting.
 * @param DC Pointer to the data counter.
 * @return 0 if valid, 1 if invalid.
 */
int validate_data_directive(const char *line_content, Line *line, int line_number, int *DC);

/**
 * @brief Validates a .string directive line.
 * 
 * @param line_content The content of the line to validate.
 * @param line Pointer to the line structure where errors will be recorded.
 * @param line_number The line number for error reporting.
 * @param DC Pointer to the data counter.
 * @return 0 if valid, 1 if invalid.
 */
int validate_string_directive(const char *line_content, Line *line, int line_number, int *DC);

/**
 * @brief Validates if an operand is an index label.
 * 
 * @param operand The operand to validate.
 * @return 1 if valid, 0 otherwise.
 */
int validate_index_label(char *operand);

/**
 * @brief Validates if an operand is a direct register.
 * 
 * @param operand The operand to validate.
 * @return 1 if valid, 0 otherwise.
 */
int validate_register_operand(char *operand);


/**
 * @brief Validates if an operand is an indirect register.
 * 
 * @param operand The operand to validate.
 * @return 1 if valid, 0 otherwise.
 */
int validate_indirect_register_operand(char *operand);

/**
 * @brief Cleans up and frees resources associated with a line.
 * 
 * @param result Pointer to the line structure to clean up.
 */
void cleanup_line_resources(Line *result);

/**
 * @brief Initializes a line structure with default values.
 * 
 * @param line Pointer to the line structure to initialize.
 */
void initialize_line(Line *line);

/**
 * @brief Analyzes an operand and sets its type.
 * 
 * @param operand Pointer to the operand structure to analyze.
 * @param token The string representing the operand.
 */
void analyze_operand(Operand *operand, char *token);


/**
 * @brief Checks if an instruction requires a single operand.
 * 
 * @param opcode The opcode of the instruction.
 * @return 1 if it requires a single operand, 0 otherwise.
 */
int is_single_operand_instruction(int opcode);

/**
 * @brief Checks if an instruction requires no operands.
 * 
 * @param opcode The opcode of the instruction.
 * @return 1 if it requires no operands, 0 otherwise.
 */
int is_no_operand_instruction(int opcode);


#endif
