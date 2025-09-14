#include "constants.h"
#include "syntax_analyzer.h"
#include "opcode_table.h"
#include "symbol_table.h"
#include "utility.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

void add_data(int data);
void process_line(char *line, Line *result, int line_number);

int is_no_operand_instruction(int opcode) {
    /* Function to check if the opcode corresponds to an instruction with no operands */

    if (opcode == get_opcode("stop") || opcode == get_opcode("rts")) {
        return 1;
    }
    return 0;
}

int is_single_operand_instruction(int opcode) {
    /* Function to check if the opcode corresponds to an instruction with a single operand */

    if (opcode == get_opcode("clr") || opcode == get_opcode("not") || opcode == get_opcode("inc") ||
        opcode == get_opcode("dec") || opcode == get_opcode("jmp") || opcode == get_opcode("bne") ||
        opcode == get_opcode("red") || opcode == get_opcode("prn") || opcode == get_opcode("jsr")) {
        return 1;
    }
    return 0;
}

void initialize_line(Line *line) {
    /* Initialize the Line structure */

    line->line_number = 0;
    line->error_count = 0;
    line->warning_count = 0;
    memset(line->errors, 0, sizeof(line->errors));
    memset(line->warnings, 0, sizeof(line->warnings));

    line->instruction.label = NULL;
    line->instruction.opcode = -1;
    line->instruction.source = NULL;
    line->instruction.destination = NULL;
    line->instruction.source_type = unknown_addressing;
    line->instruction.dest_type = unknown_addressing;

    line->directive.label = NULL;
    line->directive.directive_type = unknown_dir;
    line->directive.data_options.string = NULL;

    line->line_type = empty;

    line->next = NULL;

    memset(line->error, 0, sizeof(line->error));
}


void add_error(Line *line, int line_number, const char *message) {
Error *new_error;
int i;
    /* Check if the maximum number of errors has been reached */

    if (line->error_count >= MAX_ERRORS) {
        fprintf(stderr, "Error: Maximum number of errors (%d) reached for line %d\n", MAX_ERRORS, line->line_number);
        return; 
    }

    /* Check if the error already exists for the given line number */

    for (i = 0; i < line->error_count; i++) {
        if (line->errors[i]->line_number == line_number) {
            strncpy(line->errors[i]->message, message, MAX_LINE_LENGTH - 1);
            line->errors[i]->message[MAX_LINE_LENGTH - 1] = '\0';
            return;
        }
    }

    new_error = (Error *)calloc(1, sizeof(Error));
    if (!new_error) {
        fprintf(stderr, "Error: Memory allocation failed for new error on line %d\n", line_number);
        return; 
    }

    new_error->line_number = line_number;
    strncpy(new_error->message, message, MAX_LINE_LENGTH - 1);
    new_error->message[strlen(message)] = '\0';  
    printf("Error [line: %d]: %s\n",line_number, new_error->message);
    line->errors[line->error_count] = new_error;
    line->error_count++;
}



void add_warning(Line *line, int line_number, const char *message) {
Warning *new_warning;
    /* Check if the maximum number of warnings has been reached */
    if (line->warning_count >= MAX_WARNINGS) {
        fprintf(stderr, "Warning: Maximum number of warnings (%d) reached for line %d\n", MAX_WARNINGS, line_number);
        return; 
    }

    new_warning = (Warning *)calloc(1, sizeof(Warning));
    if (!new_warning) {
        fprintf(stderr, "Error: Memory allocation failed for new warning on line %d\n", line_number);
        return;
    }

    new_warning->line_number = line_number;
    strncpy(new_warning->message, message, MAX_LINE_LENGTH - 1);
    new_warning->message[MAX_LINE_LENGTH - 1] = '\0';

    line->warnings[line->warning_count] = new_warning;
    line->warning_count++;
}

void print_errors(const Line *line) {
int i;
    /* Print all errors associated with the line */

    for (i = 0; i < line->error_count; i++) {
        if (line->errors[i] != NULL) {
            fprintf(stderr, "Error at line %d: %s\n", line->errors[i]->line_number, line->errors[i]->message);
        }
    }
}

void print_warnings(const Line *line) {
int i;
    /* Print all warnings associated with the line */

    for (i = 0; i < line->warning_count; i++) {
        if (line->warnings[i] != NULL) {
            fprintf(stderr, "Warning at line %d: %s\n", line->warnings[i]->line_number, line->warnings[i]->message);
        }
    }
}

void free_errors(Line *line) {
int i;
    for (i = 0; i < line->error_count; i++) {
        if (line->errors[i] != NULL) {
            free(line->errors[i]);
            line->errors[i] = NULL; 
        }
    }
    line->error_count = 0; 
}


void free_warnings(Line *line) {
int i;
    for (i = 0; i < line->warning_count; i++) {
        if (line->warnings[i] != NULL) {
            free(line->warnings[i]);
            line->warnings[i] = NULL;
        }
    }
    line->warning_count = 0;
}

char *clean_opcode(char *opcode) {
    /* Clean the opcode by trimming whitespace and special characters */
    char *cleaned_opcode = trim_whitespace_and_special(opcode);
    return cleaned_opcode;
}


void print_encoded_instruction(const EncodedInstruction *encoded_instr, int word) {

    /* Print the encoded instruction details */

    printf("Opcode: %d\n", encoded_instr->opcode);
    printf("Source Mode: %d\n", encoded_instr->source_mode);
    printf("Destination Mode: %d\n", encoded_instr->dest_mode);
    printf("A, R, E: %d\n", encoded_instr->are);
}


EncodedInstruction encode_instruction(const InstructionParts *instr) { 

    /* Encode the instruction parts into an EncodedInstruction structure */

    EncodedInstruction encoded_instr = {0}; 

    encoded_instr.opcode = instr->opcode;
    encoded_instr.source_mode = instr->source_type;
    encoded_instr.dest_mode = instr->dest_type;

    encoded_instr.are = get_are_value(instr);


    return encoded_instr;
}

AddressingType get_are_value(const InstructionParts *instr) {
    /* Determine the ARE value based on the operand types */

    if (instr->source_type == immediate_addressing || instr->dest_type == immediate_addressing) {
        return ABSOLUTE;
    } else if (instr->source_type == direct_addressing || instr->dest_type == direct_addressing) {
        return RELOCATABLE;
    } else {
        return EXTERNAL;
    }
}

DirectiveType get_directive_type(const char *directive) {
    /* Identify the directive type based on the string */

    if (strcmp(directive, ".entry") == 0) {
        return entry_dir;
    } else if (strcmp(directive, ".extern") == 0) {
        return extern_dir;
    }
    return unknown_dir;
}

TokenizedLine tokenize_line(char *str) {
    /* Tokenize the input line into separate tokens */

    TokenizedLine result = {0};
    int count;
    char *token;
    char *end;

    count = 0;

    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0') {
        return result;
    }

    token = str;
    /* Split the line into tokens based on whitespace */

    while ((end = strpbrk(token, WHITESPACE)) != NULL) {
        *end = '\0';
        result.tokens[count++] = token;
        token = end + 1;
        while (isspace((unsigned char)*token)) token++;
        if (*token == '\0') {
            break;
        }
    }

    if (*token != '\0') {
        result.tokens[count++] = token;
    }

    result.token_count = count;
    return result;
}

int validate_number_after_hash(const char *str) {

    /* Validate if a number follows the '#' symbol correctly */

    if (*str != '#') {
        return 0; 
    }

    str++; 

    while (isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return 0; 
    }
    /* Check for optional sign */
    if (*str == '+' || *str == '-') {
        str++;
    }
    /* Ensure digits follow the optional sign */

    if (!isdigit((unsigned char)*str)) {
        return 0; 
    }

    while (*str != '\0' && !isspace((unsigned char)*str)) {
        if (!isdigit((unsigned char)*str)) {
            return 0; 
        }
        str++;
    }

    return 1; 
}

OperandType validate_and_get_operand_type(char *operand) {
    /* Clean the operand and determine its type */

    char cleaned_operand[MAX_LINE_LENGTH];
    OperandType type;

    strncpy(cleaned_operand, operand, MAX_LINE_LENGTH - 1);
    cleaned_operand[MAX_LINE_LENGTH - 1] = '\0';

    trim_whitespace_and_special(cleaned_operand);

    /* Check if the operand is an instruction or directive */

    if (is_instruction(cleaned_operand) || is_directive(cleaned_operand)) {
        return no_operand;
    }

    type = get_operand_type(cleaned_operand);

    return type;
} 

OperandType get_operand_type(char *operand) {
    /* Determine the type of the operand based on its format */

    char trimmed_operand[MAX_LINE_LENGTH];

    strncpy(trimmed_operand, operand, MAX_LINE_LENGTH - 1);
    trimmed_operand[MAX_LINE_LENGTH - 1] = '\0'; 

    trim_whitespace_and_special(trimmed_operand);

    if (strlen(trimmed_operand) == 0) {
        return no_operand;
    }


    if (is_register(trimmed_operand)) {
        return direct_register_addressing;
    } else if (*trimmed_operand == '*' && is_register(trimmed_operand + 1)) {
        return indirect_register_addressing;
    } else if (*trimmed_operand == '#' && validate_number_after_hash(trimmed_operand)) {
        return immediate_addressing;
    } else if (isalpha((unsigned char)trimmed_operand[0])) {
        if (strchr(trimmed_operand, '[') && strchr(trimmed_operand, ']')) {
            return validate_index_label(trimmed_operand) ? index_label_addressing : unknown_addressing;
        } else if (is_valid_label_name(trimmed_operand)) {
            return direct_addressing;
        }
    }
    fprintf(stderr, "Error: Unsupported operand type for '%s'\n", operand);
    return unknown_addressing;
}

void parse_operand(char *operand, int operand_type, Line *line) {
    /* Parse the operand and add appropriate errors if needed */

    if (operand_type == immediate_addressing) {
        if (!validate_number_after_hash(operand)) {
            add_error(line, line->line_number, "Invalid immediate operand");
        }
    } else if (operand_type == direct_addressing) {
        if (!is_valid_label_name(operand)) {
            add_error(line, line->line_number, "Invalid direct operand");
        }
    } else if (operand_type == indirect_register_addressing) {
        if (!validate_indirect_register_operand(operand)) {
            add_error(line, line->line_number, "Invalid indirect register operand");
        }
    } else if (operand_type == direct_register_addressing) {
        if (!validate_register_operand(operand)) {
            add_error(line, line->line_number, "Invalid direct register operand");
        }
    } else if (operand_type == index_constant_addressing) {
        if (!is_valid_label_name(operand)) {
            add_error(line, line->line_number, "Invalid index constant operand");
        }
    } else if (operand_type == index_label_addressing) {
        if (!validate_index_label(operand)) {
            add_error(line, line->line_number, "Invalid index label operand");
        }
    } else {
        add_error(line, line->line_number, "Unknown operand type");
    }
}

void parse_operands(char *operands, Line *line) {
    /* Parse the operands from the line and classify them */

    char operands_copy[MAX_LINE_LENGTH];
    char *token, *comma_flag;
    int operand_type;
    Operand **current_operand;
    int opcode;

    opcode = line->instruction.opcode;

    if (operands == NULL || *operands == '\0') {
        line->instruction.source_type = no_operand;
        line->instruction.dest_type = no_operand;
        return;
    }

    strncpy(operands_copy, operands, MAX_LINE_LENGTH);
    operands_copy[MAX_LINE_LENGTH - 1] = '\0';

    token = strtok(operands_copy, ",");
    current_operand = &(line->instruction.source);
    /* Handle instructions with two operands */
    switch (opcode) {
        case 0:  
        case 1:  
        case 2:  
        case 3:  
        case 4: 
            if (token != NULL) {
                comma_flag = strchr(token, ',');
                if (!comma_flag) {
                    add_error(line, line->line_number, "Missing comma between operands");
                    return;
                }
                *comma_flag = '\0';
                
                while (isspace((unsigned char)*token)) token++;
                operand_type = validate_and_get_operand_type(token);
                if (operand_type == unknown_addressing) {
                    add_error(line, line->line_number, "Invalid operand type for operand 1");
                } else {
                    *current_operand = (Operand *)calloc(1, sizeof(Operand)); 
                    if (*current_operand) {
                        (*current_operand)->value = (char *)malloc(strlen(token) + 1);
                        if ((*current_operand)->value) {
                            strcpy((*current_operand)->value, token);
                        }
                        parse_operand(token, operand_type, line);
                    }
                }

                token = comma_flag + 1;
                while (isspace((unsigned char)*token)) token++;
                
                current_operand = &(line->instruction.destination);
                operand_type = validate_and_get_operand_type(token);
                if (operand_type == unknown_addressing) {
                    add_error(line, line->line_number, "Invalid operand type for operand 2");
                } else {
                    *current_operand = (Operand *)calloc(1, sizeof(Operand)); 
                    if (*current_operand) {
                        (*current_operand)->value = (char *)malloc(strlen(token) + 1);
                        if ((*current_operand)->value) {
                            strcpy((*current_operand)->value, token);
                        }
                        parse_operand(token, operand_type, line);
                    }
                }
            }
            break;
        /* Handle instructions with a single operand */
        case 5:  
        case 6:  
        case 7:  
        case 8: 
        case 9:  
        case 10: 
        case 11: 
        case 12: 
        case 13: 
            if (token != NULL) {
                while (isspace((unsigned char)*token)) token++;
                current_operand = &(line->instruction.destination);
                operand_type = validate_and_get_operand_type(token);
                if (operand_type == unknown_addressing) {
                    add_error(line, line->line_number, "Invalid operand type for operand 1");
                } else {
                    *current_operand = (Operand *)calloc(1, sizeof(Operand)); 
                    if (*current_operand) {
                        (*current_operand)->value = (char *)malloc(strlen(token) + 1);
                        if ((*current_operand)->value) {
                            strcpy((*current_operand)->value, token);
                        }
                        parse_operand(token, operand_type, line);
                    }
                }
            }
            break;
        /* Handle instructions with no operands */
        case 14: 
        case 15: 
            line->instruction.source_type = no_operand;
            line->instruction.dest_type = no_operand;
            break;

        default:
            add_error(line, line->line_number, "Unknown opcode");
            break;
    }

    /* Set operand types to no_operand if they are not defined */
    if (line->instruction.source == NULL) {
        line->instruction.source_type = no_operand;
    }
    if (line->instruction.destination == NULL) {
        line->instruction.dest_type = no_operand;
    } else if (is_single_operand_instruction(line->instruction.opcode)) {
        line->instruction.source_type = no_operand;
    }
}

int is_number(const char *str, int min, int max, int *result) {
    /* Validate if the given string is a number within a specified range */
    char *endptr;
    long val;
    const char *ptr;

    if (*str == '\0') {
        return 1; 
    }

    /* Check for optional '+' or '-' sign */
    if (*str == '+' || *str == '-') {
        ptr = str + 1;
    } else {
        ptr = str;
    }

    /*Ensure the remaining string is not empty and contains only digits*/
    if (*str == '\0') {
        return 1;
    }

    for (ptr = str; *ptr != '\0'; ++ptr) {
        if (!isdigit(*ptr)) {
            return 1;
        }
    }

    val = strtol(str, &endptr, 10);
    if (*endptr != '\0' || endptr == str) {
        return 1; 
    }
    if (val < min || val > max) {
        return 2; 
    }
    *result = (int)val;
    return 0; 
}



int is_valid_string(const char *str) {
    /* Validate if the given string is a correctly formatted string directive */
    size_t len = strlen(str);
    size_t i;
    int in_quotes = 0;

    if (len < 2 || str[0] != '"' || str[len - 1] != '"') {
        return 0; 
    }
    /* Check that the string contains valid characters within quotes */

    for (i = 1; i < len - 1; ++i) {
        if (str[i] < 32 || str[i] > 126) {
            return 0;  
        }
    }
    /* Ensure the quotes are balanced */

    for (i = 0; i < len; ++i) {
        if (str[i] == '"') {
            in_quotes = !in_quotes;  
        } else if (!in_quotes && str[i] != ' ' && str[i] != '\t') {
            return 0;  
        }
    }

    return in_quotes ? 0 : 1;  
}

void analyze_operand(Operand *operand, char *token) {
    /* Analyze the given token and determine its type */

    char token_copy[MAX_LINE_LENGTH];
    char *p;
    char *next_token;
    OperandType type;
    strncpy(token_copy, token, MAX_LINE_LENGTH);
    token_copy[MAX_LINE_LENGTH - 1] = '\0'; 

    if (operand == NULL || token == NULL) {
        fprintf(stderr, "Error: Null pointer passed to analyze_operand\n");
        return;
    }


    p = token_copy;

    next_token = strtok(token_copy, " \t\n");
    if (next_token) {
        token = next_token;
    } else {
        p = strchr(token, '\n');
        if (p) *p = '\0';  
    }

    type = validate_and_get_operand_type(token); 
    operand->type = type;
}

/* A function to check if a given name is a register */
int is_register(const char *name) {
    const char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    int i;
    for (i = 0; i < 8; i++) {
        if (strcmp(name, registers[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* A function to check if a given name is an instruction */
int is_instruction(const char *name) {
    int i;
    for (i = 0; i < OPCODE_TABLE_SIZE; i++) {
        if (strcmp(name, opcode_table[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

/* A function to check if a given name is a directive */
int is_directive(const char *name) {
    const char *directives[] = {".data", ".string", ".entry", ".extern"};
    int i;
    for (i = 0; i < 4; i++) {
        if (strcmp(name, directives[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Update is_valid_label_name to check against instructions, registers, and directives */ 
int is_valid_label_name(const char *name) {
    size_t i;
    size_t len = strlen(name);

    if (len > MAX_LABEL_LEN) {
        return 0; 
    }
    if (!isalpha((unsigned char)name[0])) {

        return 0; 
    }
    for (i = 1; i < strlen(name); i++) {
        if (!isalnum((unsigned char)name[i])) {

            return 0; 
        }
    }
    if (is_register(name) || is_instruction(name) || is_directive(name)) {
        return 0;
    }
    return 1;
}

/* Function to validate index label addressing */
int validate_index_label(char *operand) {
    char *bracket_open = strchr(operand, '[');
    char *bracket_close = strchr(operand, ']');
    if (bracket_open && bracket_close && (bracket_open < bracket_close)) {
        char reg[4]; 
        strncpy(reg, bracket_open + 1, bracket_close - bracket_open - 1);
        reg[bracket_close - bracket_open - 1] = '\0';
        return is_register(reg);
    }
    return 0;
}

/* Function to validate direct register addressing */
int validate_register_operand(char *operand) {
    return is_register(operand);
}

/* Function to validate indirect register addressing */
int validate_indirect_register_operand(char *operand) {
    if (operand[0] == '*' && is_register(operand + 1)) {
        return 1;
    }
    return 0;
}

void cleanup_line_resources(Line *line) {
    int i;
    /* Free resources associated with the line's instruction source operand */
    if (line->instruction.source) {
        Operand *current_operand = line->instruction.source;
        while (current_operand) {
            Operand *next_operand = current_operand->next;
            if (current_operand->value) {
                free(current_operand->value);
                current_operand->value = NULL;
            }
            free(current_operand);
            current_operand = next_operand;
        }
        line->instruction.source = NULL;
    }
    /* Free resources associated with the line's instruction destination operand */
    if (line->instruction.destination) {
        Operand *current_operand = line->instruction.destination;
        while (current_operand) {
            Operand *next_operand = current_operand->next;
            if (current_operand->value) {
                free(current_operand->value);
                current_operand->value = NULL;
            }
            free(current_operand);
            current_operand = next_operand;
        }
        line->instruction.destination = NULL;
    }
    /* Free resources associated with the line's directive label */
    if (line->directive.label) {
        free(line->directive.label);
        line->directive.label = NULL;
    }
    /* Free resources associated with the line's directive data options */
    if (line->directive.data_options.string) {
        free(line->directive.data_options.string);
        line->directive.data_options.string = NULL;
    } else if (line->directive.data_options.index_option.label) {
        free(line->directive.data_options.index_option.label);
        line->directive.data_options.index_option.label = NULL;
    } else if (line->directive.data_options.numbers.value) {
        free(line->directive.data_options.numbers.value);
        line->directive.data_options.index_option.label = NULL;
    }

    for (i = 0; i < line->error_count; i++) {
        if (line->errors[i]) {
            free(line->errors[i]);
            line->errors[i] = NULL;
        }
    }
    line->error_count = 0;

    for (i = 0; i < line->warning_count; i++) {
        if (line->warnings[i]) {
            free(line->warnings[i]);
            line->warnings[i] = NULL;
        }
    }
    line->warning_count = 0;

    memset(line, 0, sizeof(Line));
}

void parse_file(FILE *input_file, Line **line_list, Error **error_list, Warning **warning_list) {
    /* Parse the input file line by line and store the results in the line list */
    char line[MAX_LINE_LENGTH];
    int line_number = 1;
    Line *result = NULL; 

    while (fgets(line, sizeof(line), input_file)) {
        if (is_blank_line(line)) {
            line_number++;
            continue;
        }

        result = (Line *)calloc(1, sizeof(Line)); 

        if (result == NULL) {
        add_error(result, line_number, "Memory allocation failed"); 
        continue; 
        }

        initialize_line(result);

        process_line(line, result, line_number);

        result->next = *line_list;
        *line_list = result;

        line_number++;
    }
    if (result != NULL) {
        free_errors(result);
        cleanup_line_resources(result);
        free(result);
    }
}

int validate_data_directive(const char *line_content, Line *line, int line_number, int *DC) {
    /* Validate the .data directive and handle numbers */

    const char *ptr = line_content;
    int expecting_number = 1;
    int found_number = 0;
    int trailing_comma = 0;
    while (*ptr == ' ' || *ptr == '\t') {
        ptr++;
    }

    if (*ptr == '\0') {
        add_error(line, line_number, "Empty .data directive");
        goto cleanup;
    }

    while (*ptr != '\0') {
        while (*ptr == ' ' || *ptr == '\t') {
            ptr++;
        }

        if (*ptr == '\0') {
            break;
        }

        if (expecting_number) {
            char *endptr;
            long number = strtol(ptr, &endptr, 10);  
            if (ptr == endptr) {
                add_error(line, line_number, "Invalid number in .data directive"); 
                goto cleanup;
            }
            add_data((int)number);  
            (*DC)++;  
            ptr = endptr;
            expecting_number = 0;
            found_number = 1;
            trailing_comma = 0;
        } else {
            if (*ptr != ',') {
                add_error(line, line_number, "Expected a comma between numbers in .data directive");
                goto cleanup;
            }
            ptr++;
            expecting_number = 1;
            trailing_comma = 1;
        }
    }
    /* Check for errors in the .data directive formatting */
    if (!found_number) {
        add_error(line, line_number, "No numbers found in .data directive");
        goto cleanup;
    }

    if (trailing_comma) {
        add_error(line, line_number, "Trailing comma in .data directive");
        goto cleanup;
    }
    return 0;

cleanup:
    cleanup_line_resources(line);
    return 1;
}

int validate_string_directive(const char *line_content, Line *line, int line_number, int *DC) {
    /* Validate the .string directive and handle its content */
    char line_copy[MAX_LINE_LENGTH];
    char *token;
    int length;
    int i;

    if (line_content == NULL) {
        add_error(line, line_number, "Null line content in .string directive");
        return 1;
    }

    strncpy(line_copy, line_content, MAX_LINE_LENGTH);
    line_copy[MAX_LINE_LENGTH - 1] = '\0';

    token = strtok(line_copy, " \t\n");
    if (token == NULL || !is_valid_string(token)) {
        add_error(line, line_number, "Invalid string in .string directive");
        return 1;
    }
    /* Process the string, skipping the surrounding quotes */
    length = strlen(token);
    for (i = 1; i < length - 1; i++) { /* Skip the surrounding quotes */
        add_data(token[i]);
        (*DC)++;
    }
    add_data(0); /* Null-terminator for the string */
    (*DC)++;

   return 0;
}

/* Function to output an encoded word */

void output_word(int word) {
     /* Here you can add the logic to output the encoded word (e.g., print to screen or write to file) */

    printf("Encoded word: %04x\n", word);
}
