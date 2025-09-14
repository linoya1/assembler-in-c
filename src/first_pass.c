#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "constants.h"
#include "first_pass.h"
#include "symbol_table.h"
#include "opcode_table.h"
#include "syntax_analyzer.h"
#include "pre_process.h"
#include "utility.h"

/* Global pointers for managing the instruction and data lists */
InstructionNode *instruction_head = NULL;
InstructionNode *instruction_tail = NULL;
DataNode *data_head = NULL;
DataNode *data_tail = NULL;


void add_data(int data) {
    /* Allocate memory for a new data node */
    DataNode *new_node = (DataNode *)malloc(sizeof(DataNode));
    if (!new_node) {
        /* Handle memory allocation failure */
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    /* Initialize the new node */
    new_node->data = data;
    new_node->next = NULL;
    if (data_tail) {
        /* Add the new node to the end of the list */
        data_tail->next = new_node;
        data_tail = new_node;
    } else {
        /* If the list is empty, set the head and tail to the new node */
        data_head = data_tail = new_node;
    }
}

void free_instructions() {
    InstructionNode *current = instruction_head;
    while (current) {
        InstructionNode *next = current->next;
        /* Free the current instruction node */
        free(current);
        current = next;
    }
    /* Reset the head and tail pointers */
    instruction_head = instruction_tail = NULL;
}

void free_data() {
    DataNode *current = data_head;
    while (current) {
        DataNode *next = current->next;
        /* Free the current data node */
        free(current);
        current = next;
    }
    /* Reset the head and tail pointers */
    data_head = data_tail = NULL;
}

void save_symbol_table(Symbol **symbol_table, Symbol **symbol_memory_table) {
    Symbol *current = *symbol_table;
    Symbol *last = NULL;
    while (current != NULL) {
        /* Allocate memory for a new symbol */
        Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
        if (!new_symbol) {
            /* Handle memory allocation failure */
            fprintf(stderr, "Error: Memory allocation failed\n");
            free_symbol_table(*symbol_memory_table);
            *symbol_memory_table = NULL;
            exit(1);
        }
        /* Copy symbol properties */
        strncpy(new_symbol->name, current->name, MAX_LABEL_LEN);
        new_symbol->name[MAX_LABEL_LEN] = '\0';
    
        new_symbol->address = current->address;
        new_symbol->is_extern = current->is_extern;
        new_symbol->is_entry = current->is_entry;
        new_symbol->type = current->type;
        new_symbol->line_number = current->line_number;
        new_symbol->entry_addresses = NULL;
        new_symbol->entry_address_count = 0;
        new_symbol->external_addresses = NULL;
        new_symbol->external_address_count = 0;
        new_symbol->next = NULL;


        /* Copy entry addresses if they exist */
        if (current->entry_addresses) {
            new_symbol->entry_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
            if (!new_symbol->entry_addresses) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                exit(1);
            }
            memcpy(new_symbol->entry_addresses, current->entry_addresses, current->entry_address_count * sizeof(int));
            new_symbol->entry_address_count = current->entry_address_count;
        }
        /* Copy external addresses if they exist */
        if (current->external_addresses) {
            new_symbol->external_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
            if (!new_symbol->external_addresses) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                exit(1);
            }
            memcpy(new_symbol->external_addresses, current->external_addresses, current->external_address_count * sizeof(int));
            new_symbol->external_address_count = current->external_address_count;
        }

        /* Add the new symbol to the memory table */

        if (*symbol_memory_table == NULL) {
            *symbol_memory_table = new_symbol;
        } else {
            last->next = new_symbol;
        }
        last = new_symbol;
        current = current->next;
    }
}

int symbol_already_defined_in_pass(const char *symbol_name, Symbol *symbol_table) {
    Symbol *current_symbol;
    current_symbol = symbol_table; 
    while (current_symbol != NULL) {
        /* Compare the symbol names to check for duplicates */
        if (strcmp(current_symbol->name, symbol_name) == 0) {
            return 1; 
        }
        current_symbol = current_symbol->next;
    }
    return 0; 
}

void check_symbol_definitions(Symbol *symbol_table, Line *line) {
    Symbol *current = symbol_table;
    while (current != NULL) {
        /* Check if a symbol is incorrectly defined as both entry and extern */
        if (current->is_entry && current->is_extern) {
            add_error(line, 0, "Symbol defined as both entry and extern");
        }
        /* Check if an entry symbol is missing a definition */
        if (current->is_entry && current->address == 0 && !current->is_extern) {
            if (!symbol_already_defined_in_pass(current->name, symbol_table)) {
                add_error(line, 0, "Entry symbol without definition");
            }
        }
        /* Check if an extern symbol has an incorrect address */
        if (current->is_extern && current->address != -1) {
            add_error(line, 0, "Extern symbol with non-negative-one address");
        }
        current = current->next;
    }
}

Directive *read_entry_or_extern(char *str, Symbol **symbol_table, Line *line) {
    Directive *directive;
    char *token;
    size_t label_len;
    Symbol *existing_symbol;  
    char str_copy[MAX_LINE_LENGTH];
    /* Copy the line to preserve the original string */
    strncpy(str_copy, str, MAX_LINE_LENGTH);
    str_copy[MAX_LINE_LENGTH - 1] = '\0';

    /* Allocate memory for the directive */
    directive = (Directive *)malloc(sizeof(Directive));
    if (directive == NULL) {
        add_error(line, line->line_number, "Memory allocation failed");
        return NULL;
    }

    directive->label = NULL;
    /* Tokenize the line to extract the directive type */
    token = strtok(str_copy, " \t\n");
    if (token == NULL) {
        add_error(line, line->line_number, "Missing directive type");
        free(directive);
        return NULL;
    }

    /* Determine the directive type based on the token */
    if (strcmp(token, ".extern") == 0) {
        directive->directive_type = extern_dir;
    } else if (strcmp(token, ".entry") == 0) {
        directive->directive_type = entry_dir;
    } else {
        add_error(line, line->line_number, "Invalid directive type");
        free(directive);
        return NULL;
    }

    /* Get the label associated with the directive */
    token = strtok(NULL, " \t\n");

    while (token && *token == '\0') {
        token = strtok(NULL, " \t\n");
    }
    /* Validate the label */
    if (token == NULL || !is_valid_label_name(token)) {
        add_error(line, line->line_number, "Invalid label in .extern or .entry directive");
        free(directive);
        return NULL;
    }

    /* Check if the symbol already exists in the table */
    existing_symbol = get_symbol(*symbol_table, token);
    if (existing_symbol) {
        /* Handle conflicts between entry and extern definitions */
        if(directive->directive_type == extern_dir && existing_symbol->type != extern_symbol)
        {
            add_error(line, line->line_number, "Cannot define .extern as it already defined");
            free(directive);
            return NULL; 
        }
        if (directive->directive_type == extern_dir && existing_symbol->is_entry) {
            add_error(line, line->line_number, "Cannot define the same label as both .entry and .extern");
            free(directive);
            return NULL;
        } else if (directive->directive_type == entry_dir && existing_symbol->is_extern) {
            add_error(line, line->line_number, "Cannot define the same label as both .extern and .entry");
            free(directive);
            return NULL;
        }
    }

    /* Allocate memory for the label and copy it */
    label_len = strlen(token);
    directive->label = (char *)malloc(label_len + 1);
    if (directive->label == NULL) {
        add_error(line, line->line_number, "Memory allocation failed");
        free(directive);
        return NULL;
    }
    strncpy(directive->label, token, label_len);
    directive->label[label_len] = '\0';

    /* Check for extra tokens after the label */
    token = strtok(NULL, " \t\n");
    if (token != NULL) {
        add_error(line, line->line_number, "Extra text after .extern or .entry directive");
        free(directive->label);
        free(directive);
        return NULL;
    }

    return directive;
} 

int first_pass(FILE *input_file, Symbol **symbol_table, Error **error_list, Warning **warning_list, int* finalIC, int* finalDC) {
    char line[MAX_LINE_LENGTH]; /* Buffer to store each line read from the file */
    char line_copy[MAX_LINE_LENGTH]; /* Copy of the current line for safe manipulation */

    int IC = 100;  /* Instruction Counter initialized to 100 (or starting address) */
    int DC = 0; /* Data Counter initialized to 0 */
    int line_number = 1; /* Tracks the current line number */
    int error_flag = 0; /* Flag indicating if an error has occurred */
    Symbol *current;
    char *comment, *token, *next_token;
    size_t label_len;
    int opcode;
    Macro *macros = NULL;
    Line current_line; /* Structure to store current line details */
    int prev_was_register;
    int current_is_register;

    initialize_line(&current_line); /* Initialize line structure for error handling */

    /* Read each line from the input file until EOF */
    while (fgets(line, sizeof(line), input_file)) {
        current_line.line_number = line_number;

        /* Skip blank lines and comment-only lines */
        if (is_blank_line(line) || line[0] == ';') {
            line_number++;
            continue;
        }

        /* Remove comments from the line */
        comment = strchr(line, ';');
        if (comment) {
            *comment = '\0';
        }

        /* Copy the line to avoid modifying the original */
        strncpy(line_copy, line, MAX_LINE_LENGTH);
        line_copy[MAX_LINE_LENGTH - 1] = '\0';
        token = strtok(line_copy, " \t\n"); /* Tokenize the line */

        /* Continue to the next line if no token is found */
        if (!token) {
            line_number++;
            continue;
        }

        /* Check if the token is a label definition */
        if (is_label(token)) {
            label_len = strlen(token);
            token[label_len - 1] = '\0'; /* Remove the colon from the label */

            /* Validate the label name */
            if (!is_valid_label_name(token)) {
                add_error(&current_line, current_line.line_number, "Invalid label name");
                error_flag = 1;
                line_number++;
                continue;
            }

            /* Get the next token after the label */
            next_token = strtok(NULL, " \t\n");
            if (next_token == NULL) {
                add_error(&current_line, current_line.line_number, "Label defined without instruction or directive");
                error_flag = 1;
                line_number++;
                continue;
            }

            /* Check for spacing issues between the label and the directive/instruction */
            if (next_token == token + label_len) {
                add_error(&current_line, current_line.line_number, "No space after label");
                error_flag = 1;
                line_number++;
                continue;
            }
            /* Handle special cases with .entry, .extern, macro definitions, or macro invocations */
            if (strcmp(next_token, ".entry") == 0 || strcmp(next_token, ".extern") == 0) {
                add_warning(&current_line, line_number, "Label before .entry or .extern ignored");
                token = next_token;
            } else if (strcmp(next_token, "macr") == 0) {
                add_error(&current_line, current_line.line_number, "Label before macro definition");
                error_flag = 1;
                line_number++;
                continue;
            } else if (get_macro_body(macros, next_token)) {
                add_error(&current_line, current_line.line_number, "Label before macro invocation");
                error_flag = 1;
                line_number++;
                continue;
            } else {
                /* Check if the label is already defined in the symbol table */
                Symbol *existing_symbol = get_symbol(*symbol_table, token);
                if (existing_symbol && !existing_symbol->is_entry) {
                    add_error(&current_line, current_line.line_number, "Label already defined (as extern or multiple definitions)"); 
                    error_flag = 1;
                    line_number++;
                    continue;
                }

                /* Add the symbol depending on the directive type */
                if (strcmp(next_token, ".data") == 0)
                {

                    add_symbol(symbol_table, token, DC, 0, 0, data_symbol, line_number);
                    if (next_token != NULL) {
                        next_token = strtok(NULL, "\n");

                        if (validate_data_directive(next_token, &current_line, line_number, &DC)) {
                            error_flag = 1;
                        }
                    }
                    continue;
                }
                else if(strcmp(next_token, ".string") == 0) {
                    add_symbol(symbol_table, token, DC, 0, 0, data_symbol, line_number);
                    next_token = strtok(NULL, "\n");
                    if (validate_string_directive(next_token, &current_line, line_number, &DC)) {
                        error_flag = 1;
                    } 

                    continue;
                } else {
                    /* Add the label as a code symbol */
                    add_symbol(symbol_table, token, IC, 0, 0, code_symbol, line_number);
                    token = next_token;
                }
            }
        }
        /* Handle different types of directives */
        if (token[0] == '.') {
            if (strcmp(token, ".data") == 0) {
                next_token = strtok(NULL, "\n");
                if (next_token != NULL) {
                    while (*next_token == ' ' || *next_token == '\t') {
                        next_token++;
                    }
                    if (validate_data_directive(next_token, &current_line, line_number, &DC)) {
                        error_flag = 1;
                    }
                }
            } else if (strcmp(token, ".string") == 0) {
                next_token = strtok(NULL, "\n");
                if (validate_string_directive(next_token, &current_line, line_number, &DC)) {
                    error_flag = 1;
                }

            } else if (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0) {
                Directive *directive = read_entry_or_extern(line, symbol_table, &current_line);
                if (directive) {
                    add_symbol(symbol_table, directive->label, -1,
                               directive->directive_type == extern_dir,
                               directive->directive_type == entry_dir,
                               directive->directive_type == extern_dir ? extern_symbol : entry_symbol,
                               line_number);
                    free(directive->label);
                    free(directive);
                } else {
                    error_flag = 1;
                } 
            } else {
                add_error(&current_line, current_line.line_number, "Unknown directive");
                error_flag = 1;
            }
        } else {
            /* Process the instruction using the opcode */
            opcode = get_opcode(token);
            if (opcode != -1) {
                IC++;
                prev_was_register = 0;

                /* Handle operands for the instruction */
                while ((token = strtok(NULL, " \t\n")) != NULL) {

                    Symbol *sym = get_symbol(*symbol_table, token);
                    if (sym != NULL) {
                        if (sym->is_extern) {
                            if (sym->external_addresses == NULL) {
                                sym->external_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
                                sym->external_address_count = 0;
                            }
                        } else if (sym->is_entry) {
                            if (sym->entry_addresses == NULL) {
                                sym->entry_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
                                sym->entry_address_count = 0;
                            }
                            sym->entry_addresses[sym->entry_address_count++] = IC;
                        }
                    }

                    current_is_register = (token[0] == 'r') || (token[0] == '*' && token[1] == 'r');
                    if (prev_was_register && current_is_register) {
                        /* Handle consecutive register addresses */
                    } else {
                        IC++;
                    }
                    prev_was_register = current_is_register;

}
            } else {
                add_error(&current_line, current_line.line_number, "Unknown instruction");
                error_flag = 1;
            }

        }
        line_number++;

        /* Check if memory limits are exceeded */
        if (IC + DC > MAX_MEM_SIZE) {
        	add_error(&current_line, current_line.line_number, "Memory overflow: Address exceeds RAM size");
    		error_flag = 1;
    		break;
	}
    }

    /* Update symbol addresses for data symbols */
    current = *symbol_table;
    while (current != NULL) {
        if (current->type == data_symbol) {
            current->address += IC ;
        }
        current = current->next;
    }

    /* Set final IC and DC values */
    *finalDC = DC;
    *finalIC = IC-100;

    /* Perform final checks on symbol definitions and entry labels */
    check_symbol_definitions(*symbol_table, &current_line);
    check_entry_labels(symbol_table, &current_line);
    /* Free resources */
    free_instructions();
    free_data();
    free_errors(&current_line); 
    /* Return error flag indicating if any errors were found */ 
    return error_flag;
}

void check_entry_labels(Symbol **symbol_table, Line *current_line) {
    Symbol *current_symbol = *symbol_table;
    Symbol *symbol_definition;

    /* Iterate through each symbol in the table */
    while (current_symbol != NULL) {
        /* Check if the current symbol is marked as an entry */
        if (current_symbol->is_entry) {
            /* Retrieve the actual definition of the symbol */
            symbol_definition = get_symbol(*symbol_table, current_symbol->name);
            if (symbol_definition == NULL) {
                /* Report an error if the entry label is not defined */
                add_error(current_line, current_symbol->line_number, "Entry label not defined");
            } else if (!symbol_definition->is_extern && symbol_definition->address == -1) {
                /* Report an error if the entry symbol lacks a valid address */
                add_error(current_line, current_symbol->line_number, "Entry symbol without definition or external reference");
            } else {
                /* Mark the symbol as an entry if not already set */
                if (!symbol_definition->is_entry) {
                    symbol_definition->is_entry = 1;
                }
            }
        }
        /* Move to the next symbol */
        current_symbol = current_symbol->next;
    }
}

