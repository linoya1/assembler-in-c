#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "opcode_table.h"
#include "syntax_analyzer.h"


OperandType validate_and_get_operand_type(const char *operand);

void process_line(char *line, Line *result, int line_number) {
    TokenizedLine tokens;
    char line_copy[MAX_LINE_LENGTH];
    char *opcode_str = NULL; 
    char *data_str;
    int max_numbers;
    short *numbers = NULL;
    int count;
    char *token;

    initialize_line(result); /* Initializes the Line structure */

    result->line_number = line_number;
    result->instruction.source = NULL;
    result->instruction.destination = NULL;

    strncpy(line_copy, line, MAX_LINE_LENGTH); /* Copy the line to avoid modifying the original */
    line_copy[MAX_LINE_LENGTH - 1] = '\0';

    tokens = tokenize_line(line_copy);  /* Tokenizes the line */

    if (tokens.token_count > 0) {

        /* Checks if the first token is a directive (starts with '.') */
   if (tokens.tokens[0][0] == '.') {
        result->line_type = directive;

        if (tokens.token_count > 1) {
                /* Handle different types of directives based on the first token */
            if (strcmp(tokens.tokens[0], ".entry") == 0) {
                result->directive.directive_type = entry_dir;
                result->directive.label = duplicate_string(tokens.tokens[1]);
            } else if (strcmp(tokens.tokens[0], ".extern") == 0) {
                result->directive.directive_type = extern_dir;
                result->directive.label = duplicate_string(tokens.tokens[1]);
            } else if (strcmp(tokens.tokens[0], ".string") == 0) {
                result->directive.directive_type = string_dir;
                result->directive.data_options.string = duplicate_string(tokens.tokens[1]);
            } else if (strcmp(tokens.tokens[0], ".data") == 0) {
                    /* Handles .data directive by parsing numbers */
                result->directive.directive_type = data_dir;
                data_str = strstr(line, ".data");
                    if (data_str != NULL) {
                        data_str += 5;  
                        while (*data_str == ' ' || *data_str == '\t') {
                            data_str++;  
                        }
                    } else {
                        add_error(result, line_number, "Invalid .data directive");
                        return;
                    }
                    max_numbers = 10;  
                    numbers = malloc(max_numbers * sizeof(short));
                    count = 0;

                    /* Parse the numbers in the .data directive */
                    token = strtok(data_str, ", ");
                    while (token != NULL && count < max_numbers) {
                        numbers[count] = (short)atoi(token);
                        count++;
                        token = strtok(NULL, ", ");
                    }

                    result->directive.data_options.index_option.label = (char *)numbers;
                    result->directive.data_options.index_option.number = count;
                
                      
            } else {
                printf("Warning: Unknown directive %s\n", tokens.tokens[0]);
            }
        } else {
            printf("Warning: Directive %s has no associated label or data\n", tokens.tokens[0]);
        }
        return;
    }
        /* Check if the first token is a label and shift tokens if needed */
        if (is_label(tokens.tokens[0])) {

            memmove(tokens.tokens, tokens.tokens + 1, (tokens.token_count - 1) * sizeof(char *));
            tokens.token_count--;
        }

        if (tokens.token_count > 0) {
            if (tokens.token_count > 0 && (strcmp(tokens.tokens[0], ".string") == 0 || strcmp(tokens.tokens[0], ".data") == 0)) {
                result->line_type = directive;
                if (strcmp(tokens.tokens[0], ".string") == 0) {
                    result->directive.directive_type = string_dir;
                    if (tokens.token_count > 1) {
                        char *str = tokens.tokens[1];
                        result->directive.data_options.string = duplicate_string(str);
                    } else {
                        add_error(result, line_number, "Missing string for .string directive");
                    }

                } else if (strcmp(tokens.tokens[0], ".data") == 0) {
                    result->directive.directive_type = data_dir;
                    data_str = strstr(line, ".data");
                    if (data_str != NULL) {
                        data_str += 5;  
                        while (*data_str == ' ' || *data_str == '\t') {
                            data_str++;  
                        }
                    } else {
                        add_error(result, line_number, "Invalid .data directive");
                        return;
                    }
                    max_numbers = 10;  
                    numbers = malloc(max_numbers * sizeof(short));
                    count = 0;

                    token = strtok(data_str, ", ");
                    while (token != NULL && count < max_numbers) {
                        numbers[count] = (short)atoi(token);
                        count++;
                        token = strtok(NULL, ", ");
                    }

                    result->directive.data_options.index_option.label = (char *)numbers;
                    result->directive.data_options.index_option.number = count;
                
                               
                }
                return;
            }

        /* Process instruction line */
            result->line_type = instruction;
            opcode_str = clean_opcode(tokens.tokens[0]);
            result->instruction.opcode = get_opcode(opcode_str);  
            if (result->instruction.opcode == -1) {
                add_error(result, line_number, "Invalid opcode");
                goto cleanup;
            }
        /* Handle zero-operand instructions like "rts" and "stop" */
            if (strcmp(tokens.tokens[0], "rts") == 0 || strcmp(tokens.tokens[0], "stop") == 0) {
                result->instruction.source = NULL;
                result->instruction.destination = NULL;
                result->instruction.source_type = no_operand;
                result->instruction.dest_type = no_operand;
            } 
        /* Handle two-operand instructions like "mov", "cmp", "add" */
else if (strcmp(tokens.tokens[0], "mov") == 0 || strcmp(tokens.tokens[0], "cmp") == 0 || 
                       strcmp(tokens.tokens[0], "add") == 0 || strcmp(tokens.tokens[0], "sub") == 0 ||
                       strcmp(tokens.tokens[0], "lea") == 0) {
                if (tokens.token_count > 1) {
                    result->instruction.source = (Operand *)calloc(1, sizeof(Operand)); 
                    if (!result->instruction.source) {
                        add_error(result, line_number, "Memory allocation failed for source operand");
                        goto cleanup;
                    }
                    result->instruction.source->value = duplicate_string(tokens.tokens[1]);
                    result->instruction.source_type = validate_and_get_operand_type(tokens.tokens[1]);
                }

                if (tokens.token_count > 2) {
                    result->instruction.destination = (Operand *)calloc(1, sizeof(Operand));
                    if (!result->instruction.destination) {
                        add_error(result, line_number, "Memory allocation failed for destination operand");
                        goto cleanup;
                    }
                    result->instruction.destination->value = duplicate_string(tokens.tokens[2]);
                    result->instruction.dest_type = validate_and_get_operand_type(tokens.tokens[2]);
                }
            }
        /* Handle single-operand instructions like "clr", "not", "inc" */

              else if (strcmp(tokens.tokens[0], "clr") == 0 || strcmp(tokens.tokens[0], "not") == 0 || 
                       strcmp(tokens.tokens[0], "inc") == 0 || strcmp(tokens.tokens[0], "dec") == 0 || 
                       strcmp(tokens.tokens[0], "jmp") == 0 || strcmp(tokens.tokens[0], "bne") == 0 || 
                       strcmp(tokens.tokens[0], "red") == 0 || strcmp(tokens.tokens[0], "prn") == 0 || 
                       strcmp(tokens.tokens[0], "jsr") == 0) {
                result->instruction.source = NULL;
                result->instruction.source_type = no_operand;
                if (tokens.token_count > 1) {
                    result->instruction.destination = (Operand *)malloc(sizeof(Operand));
                    if (!result->instruction.destination) {
                        add_error(result, line_number, "Memory allocation failed for destination operand");
                        goto cleanup;
                    }
                    memset(result->instruction.destination, 0, sizeof(Operand));

                    result->instruction.destination->value = duplicate_string(tokens.tokens[1]);

                    result->instruction.dest_type = validate_and_get_operand_type(tokens.tokens[1]);
                }
            }
        }
    } else {
        result->line_type = empty; /* If no tokens, mark the line as empty */
    }


    return;
cleanup:
    cleanup_line_resources(result); /* Cleanup resources if an error occurred */
}



char *duplicate_string(const char *str) {
    char *new_str = (char *)malloc(strlen(str) + 1);
    if (new_str) {
        /* Copy the input string to the newly allocated memory */
        strcpy(new_str, str);
        new_str[strlen(str)] = '\0'; /* Ensure null-termination */
    } else {
        fprintf(stderr, "Error: Memory allocation failed in duplicate_string\n");
    }
    return new_str;
}

void toBinary(int value, int numBits, char *result) {
    int i;
    /* Convert the integer value to binary representation */
    for (i = numBits - 1; i >= 0; i--) {
        result[numBits - 1 - i] = (value & (1 << i)) ? '1' : '0';
    }
    result[numBits] = '\0'; /* Null-terminate the binary string */
}



int get_operand_binary_representation(Operand *op, Symbol *symbol_table, int IC, int is_dest) {
    int word = 0;
    int reg_num;
    Symbol *sym;

    if (op == NULL) {
        return word;  
    }
    /* Determine the type of the operand and generate its binary representation */
    switch (op->type) {
        case immediate_addressing:
            /* Immediate addressing: shift the number and set the ARE bits */
            word = (int)strtol(op->value + 1, NULL, 10) << 3;
            word |= 4; /* Set ARE to 100 (absolute) */
            break;

        case direct_addressing:
            /* Direct addressing: handle symbols and addresses */
            sym = get_symbol(symbol_table, op->value);

            if (sym != NULL) {
                /* Set the symbol address if undefined */
                if (sym->address == -1) {
                    sym->address = IC + 100;
                }
                if (sym->is_extern) {
                    /* Handle external symbols */
                    word = EXTERNAL;
                    sym->external_addresses[sym->external_address_count++] = IC;
                } else {
                    /* Shift address and set appropriate ARE bits */
                    word = sym->address << 3;
                    if(sym->type == data_symbol || sym->type == code_symbol || sym->type == entry_symbol)
                    {
                        word |= RELOCATABLE;
                    }
                    else
                    {
                        word |= 4; /*ABSOLUTE*/
                    }
                }
            } else {
                /* Add new external symbol to the table */
                add_symbol(&symbol_table, op->value, IC, 1, 0, extern_symbol, IC + 1);
                sym = get_symbol(symbol_table, op->value);
                if (sym && sym->is_extern) {
                    word = EXTERNAL;
                    sym->external_addresses[sym->external_address_count++] = IC;
                }
            }
            break;

        case direct_register_addressing:
        case indirect_register_addressing:
            /* Register addressing: determine the register number */
            reg_num = (op->value[0] == '*') ? op->value[2] - '0' : op->value[1] - '0';
            /* Set the correct bits based on whether the operand is a destination */
            if (is_dest) {
                word |= reg_num << 3;  
            } else {
                word |= reg_num << 6;  
            }

            word |= 4; 
            break;

        default:
            fprintf(stderr, "Error: Unsupported operand type %d\n", op->type);
            break;
    }

    word &= 0x7FFF;  /* Mask to 15 bits */
    return word;
}


void write_instruction_binary(InstructionParts *instruction, Symbol *symbol_table, int *IC, FILE *output_file) {
    int opcode, source_mode, dest_mode, ARE;
    int word = 0;
    int i;

    if (instruction == NULL) {
        fprintf(stderr, "Error: Instruction is NULL in write_instruction_binary\n");
        return;
    }

    opcode = instruction->opcode;
    source_mode = 0;
    if (instruction->source_type != no_operand) {
        /* Determine source operand addressing mode */
        source_mode = 1 << instruction->source_type;
    }
    
    dest_mode = 0;
    if (instruction->dest_type != no_operand) {
        /* Determine destination operand addressing mode */
        dest_mode = 1 << instruction->dest_type;
    }
        /* Combine opcode, source mode, destination mode, and ARE bits */
    word |= (opcode << 11);
    word |= (source_mode << 7);
    word |= (dest_mode << 3);
    ARE = 4;  
    word |= ARE;


    /* Reset modes for further usage */
    source_mode = instruction->source_type;
    dest_mode = instruction->dest_type;
    fprintf(output_file, "%04d %05o\n", *IC, word); /* Write the binary code to the output */
    (*IC)++;

    /* Handle case where both source and destination are registers */
    if ((source_mode == direct_register_addressing || source_mode == indirect_register_addressing) &&
        (dest_mode == direct_register_addressing || dest_mode == indirect_register_addressing)) {

        /* Combine source and destination operands into a single word */
        int combined_word = get_operand_binary_representation(instruction->source, symbol_table, *IC, 0) |
                            get_operand_binary_representation(instruction->destination, symbol_table, *IC, 1);
        fprintf(output_file, "%04d %05o\n", *IC, combined_word);
        (*IC)++;
    } else {
        /* Write each operand separately */
        Operand *op = NULL;
        for (i = 0; i < 2; i++) {
            op = (i == 0) ? instruction->source : instruction->destination;
            if (op != NULL) {
                word = get_operand_binary_representation(op, symbol_table, *IC, i);
                fprintf(output_file, "%04d %05o\n", *IC, word);
                (*IC)++;
            }
        }
    }
}


void parse_instruction(Line *line, Symbol *symbol_table, int *IC, FILE *output_file) {
int opcode;
    if (line == NULL) {
        fprintf(stderr, "Error: Line is NULL in parse_instruction\n");
        return;
    }
    /* Ensure the line type is instruction */
    if (line->line_type != instruction) {
        printf("Warning: Line type is not 'instruction'. Skipping parse_instruction.\n");
        return;
    }

    opcode = line->instruction.opcode;
    /* Handle instructions with two operands */
    if (opcode >= 0 && opcode <= 4) {
        if (line->instruction.source != NULL && line->instruction.destination != NULL) {
            line->instruction.source_type = validate_and_get_operand_type(line->instruction.source->value);
            line->instruction.dest_type = validate_and_get_operand_type(line->instruction.destination->value);
        } else {
            add_error(line, line->line_number, "Missing source or destination operand for two-operand instruction");
        }
    /* Handle single operand instructions */
    } else if (opcode >= 5 && opcode <= 13) {
        if (line->instruction.destination != NULL) {
            line->instruction.source_type = no_operand;
            line->instruction.dest_type = validate_and_get_operand_type(line->instruction.destination->value);
        } else {
            add_error(line, line->line_number, "Missing destination operand for single-operand instruction");
        }
    /* Handle no operand instructions */
    } else if (opcode >= 14 && opcode <= 15) {
        line->instruction.source_type = no_operand;
        line->instruction.dest_type = no_operand;
    } else {
        add_error(line, line->line_number, "Unknown opcode");
    }

    /* Set operand types */
    if(line->instruction.source != NULL)
    {
        line->instruction.source->type = line->instruction.source_type;
    }
    if(line->instruction.destination != NULL)
    {
        line->instruction.destination->type = line->instruction.dest_type;
    }
    /* Write the instruction to the binary output */
    write_instruction_binary(&line->instruction, symbol_table, IC, output_file);
}

void process_directive(Line *line, Symbol *symbol_table, TranslationUnit *unit, int* IC, FILE *output_file) {
    int i;
    int j;
    short *data_values;

    
    if (line->line_type != directive) {
        return;  
    }
    /* Handle different types of directives */
    switch (line->directive.directive_type) {
        case entry_dir:
            /* Process entry directive */
            if (line->directive.label != NULL) {  
                add_symbol(&symbol_table, line->directive.label, 0, 0, 1, entry_symbol, line->line_number);
            } else {
                fprintf(stderr, "Warning: NULL label in entry directive\n");
            }
            break;

        case extern_dir:
            /* Process extern directive */
            if (line->directive.label != NULL) {  
                add_symbol(&symbol_table, line->directive.label, 0, 0, 1, extern_symbol, line->line_number);
            } else {
                fprintf(stderr, "Warning: NULL label in extern directive\n");
            }
            break;

        case string_dir:
            /* Process string directive */
            if (line->directive.data_options.string != NULL) {
                /* Write each character's ASCII value */
                for (i = 0; i < strlen(line->directive.data_options.string); i++) {
                    int ascii_value = (int)line->directive.data_options.string[i]; 
                    if (line->directive.data_options.string[i] != '"' && 
                        line->directive.data_options.string[i] != '\n' && 
                        line->directive.data_options.string[i] != '\r' && 
                        line->directive.data_options.string[i] != '\t' && 
                        line->directive.data_options.string[i] != ' ') {
                        fprintf(output_file, "%04d %05o\n", *IC, ascii_value);
                        (*IC)++;
                    }
                    
                }
                /* Write null terminator */
                fprintf(output_file, "%04d %05o\n", *IC, 0);
                (*IC)++;

            } else {
                fprintf(stderr, "Warning: NULL string in string directive\n");
            }
            break;

        case data_dir:
            /* Process data directive */
            data_values = (short *)line->directive.data_options.index_option.label;
            if (data_values != NULL) {
                for (j = 0; j < line->directive.data_options.index_option.number; j++) {
                    int value = data_values[j];
                    fprintf(output_file, "%04d %05o\n", *IC, value & 077777);
                    (*IC)++;
                }
            } else {
                fprintf(stderr, "Warning: NULL data in data directive\n");
            }
            break;

        default:
            /* Handle unsupported directive types */
            fprintf(stderr, "Error: Unsupported directive type\n");
            break;
    }

    /* Free the directive label if it was allocated*/
    if (line->directive.label != NULL) {
        free(line->directive.label);
        line->directive.label = NULL;
    } 
}

int process_second_pass(FILE *input_file, FILE *output_file, TranslationUnit *unit) {
    
    Line *line = NULL;
    int IC = 100;
    char buffer[MAX_LINE_LENGTH];

    while (fgets(buffer, sizeof(buffer), input_file)) {
        line = (Line *)calloc(1, sizeof(Line));
        if (!line) {
            fprintf(stderr, "Error: Memory allocation failed for Line structure\n");
            return 1;
        }

        initialize_line(line);
        process_line(buffer, line, IC); 

        if (IC > MAX_MEM_SIZE) {
            add_error(line, line->line_number, "Memory overflow: Address exceeds RAM size");
            cleanup_line_resources(line);
            free(line);
            return 1;
        }

        if (line->line_type == directive) {
            process_directive(line, unit->symbol_table, unit, &IC, output_file);
        } else if (line->line_type == instruction) {
            parse_instruction(line, unit->symbol_table, &IC, output_file);
        }


        cleanup_line_resources(line);
        free(line);
    }
    return 0;
}
int compare_symbols_by_address(const void *a, const void *b) {
    Symbol *symbolA = *(Symbol **)a;
    Symbol *symbolB = *(Symbol **)b;
    
    if (symbolA->address < symbolB->address) return -1;
    if (symbolA->address > symbolB->address) return 1;
    return 0;
}

int write_output_files(TranslationUnit *unit, const char *basename) {
    FILE *ext_file = NULL, *ent_file = NULL;
    Symbol *current_sym = unit->symbol_table;
    int i;
    char bin_filename[MAX_FILENAME_LENGTH];
    char ent_filename[MAX_FILENAME_LENGTH];
    char ext_filename[MAX_FILENAME_LENGTH];
    int has_ent = 0, has_ext = 0;
    char *ext;
    int entry_count = 0;
    Symbol **entry_symbols = NULL;
    int index = 0;
    char clean_basename[MAX_FILENAME_LENGTH];
    strncpy(clean_basename, basename, MAX_FILENAME_LENGTH - 1);
    clean_basename[MAX_FILENAME_LENGTH - 1] = '\0'; 


    ext = strstr(clean_basename, ".as");
    if (ext != NULL) {
        *ext = '\0'; 
    }


    sprintf(bin_filename, "%s.bin", clean_basename);
    sprintf(ent_filename, "%s.ent", clean_basename);
    sprintf(ext_filename, "%s.ext", clean_basename);
    /* Count the number of entry symbols */
    current_sym = unit->symbol_table;
    while (current_sym != NULL) {
        if (current_sym->is_entry && !current_sym->is_extern && current_sym->address > 0) {
            entry_count++;
        }
        current_sym = current_sym->next;
    }

    /* Allocate an array to store entry symbols */
    if (entry_count > 0) {
        entry_symbols = (Symbol **)malloc(entry_count * sizeof(Symbol *));
        if (!entry_symbols) {
            fprintf(stderr, "Error: Memory allocation failed for entry symbols array\n");
            return 1;
        }

        /* Fill the array with entry symbols */
        current_sym = unit->symbol_table;
        while (current_sym != NULL) {
            if (current_sym->is_entry && !current_sym->is_extern && current_sym->address > 0) {
                entry_symbols[index++] = current_sym;
            }
            current_sym = current_sym->next;
        }
    }
    /* Sort entry_symbols based on the address field */
    if (entry_count > 0) {
        qsort(entry_symbols, entry_count, sizeof(Symbol *), compare_symbols_by_address);
    }
    
    /* Now you have a sorted array 'entry_symbols' containing all entry symbols */
    /* You can use this array as needed */

    /* Write sorted entry symbols to .ent file */
    if (entry_count > 0) {
        ent_file = fopen(ent_filename, "w");
        if (!ent_file) {
            fprintf(stderr, "Error: Could not open output file %s\n", ent_filename);
            free(entry_symbols);
            return 1;
        }
        has_ent = 1;
        for (i = 0; i < entry_count; i++) {
            fprintf(ent_file, "%s %04d\n", entry_symbols[i]->name, entry_symbols[i]->address);
        }

        fclose(ent_file);
    }

    /* Don't forget to free the array when you're done with it */
    if (entry_symbols) {
        free(entry_symbols);
    }

    current_sym = unit->symbol_table;
    while (current_sym != NULL) {
        if (current_sym->is_extern) {
            if (!ext_file) {
                ext_file = fopen(ext_filename, "w");
                if (!ext_file) {
                    fprintf(stderr, "Error: Could not open output file %s\n", ext_filename);
                    return 1;
                }
                has_ext = 1;
            }
            for (i = 0; i < current_sym->external_address_count; i++) {
                fprintf(ext_file, "%s %04d\n", current_sym->name, current_sym->external_addresses[i]);
            }
        }

        current_sym = current_sym->next;
    }

    if (!has_ent) {
        remove(ent_filename);
    }

    if (ext_file) {
        fclose(ext_file);
    }
    if (!has_ext) {
        remove(ext_filename);
    }

    return 0;
}

TranslationUnit *create_translation_unit() {
    TranslationUnit *unit = (TranslationUnit *)malloc(sizeof(TranslationUnit));
    if (unit == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    unit->IC = 0;
    unit->DC = 0;
    unit->symCount = 0;
    unit->symbol_table = NULL;
    return unit;
}


void free_translation_unit(TranslationUnit *unit) {
    if (unit) {
        if (unit->symbol_table) {
            free_symbol_table(unit->symbol_table);
            unit->symbol_table = NULL;
        }
        free(unit);
        unit = NULL;
    }
}



/* פונקציה להמרת קוד בינארי לאוקטלי */
void convert_binary_to_octal(const char *binary_filename, const char *octal_filename) {
    FILE *binary_file = fopen(binary_filename, "r");
    FILE *octal_file = fopen(octal_filename, "w");
    char line[16];
    char octal_code[7]; /* 6 digits + null terminator */

    if (!binary_file) {
        perror("Error opening binary file");
        return;
    }
    if (!octal_file) {
        perror("Error opening octal file");
        fclose(binary_file);
        return;
    }

    while (fgets(line, sizeof(line), binary_file)) {
        unsigned int value;
        sscanf(line, "%x", &value);
        sprintf(octal_code, "%06o", value);  
        fprintf(octal_file, "%s\n", octal_code);
    }

    fclose(binary_file);
    fclose(octal_file);
}

int second_pass(FILE *input_file, const char *basename, Symbol *symbol_table, FILE* output_file) {
    TranslationUnit *unit = create_translation_unit();
    if (unit == NULL) {
        fprintf(stderr, "Error: Failed to create translation unit\n");
        return 1;  
    }
    unit->symbol_table = symbol_table;



    if (process_second_pass(input_file, output_file, unit) != 0) {
        fprintf(stderr, "Error during second pass\n");
        free_translation_unit(unit);
        return 1;  
    }

    write_output_files(unit, basename);

        free_translation_unit(unit);
    

    /* שחרור זיכרון */

    return 0;
}

