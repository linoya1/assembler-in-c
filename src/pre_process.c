#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "constants.h"
#include "pre_process.h"
#include "utility.h"


void add_macro(Macro **head, const char *name, const char *body) {
    Macro *new_macro = (Macro *)malloc(sizeof(Macro));
    if (!new_macro) {
        /* Handle memory allocation failure */
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    /* Copy the macro name with a length limit */
    strncpy(new_macro->name, name, MAX_MACRO_NAME);
    new_macro->name[MAX_MACRO_NAME] = '\0';  
    /* Allocate memory and copy the macro body */
    new_macro->body = (char *)malloc(strlen(body) + 1);
    if (!new_macro->body) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    strcpy(new_macro->body, body);
    /* Insert the new macro at the beginning of the list */
    new_macro->next = *head;
    *head = new_macro;
}

char* get_macro_body(Macro *head, const char *name) {
    Macro *current = head;
    /* Iterate through the list to find the macro */
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->body;
        }
        current = current->next;
    }
    return NULL; /* Macro not found */
}

void free_macros(Macro *head) {
    Macro *current = head;
    Macro *next;
    /* Iterate and free each macro */
    while (current != NULL) {
        next = current->next;
        free(current->body);  
        free(current);
        current = next;
    }
}

int is_whitespace_only_line(const char *line) {
    const char *p;
    /* Check each character in the line */
    for (p = line; *p; p++) {
        if (!strchr(" \t\n", *p)) {
            return 0; /* Found a non-whitespace character */
        }
    }
    return 1; /* Line contains only whitespace */
}

int is_valid_macro_name(const char *name) {
    const char *p;
    int valid_length;
    const char *reserved_words[] = {
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", 
        "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "mcr", "endmcr", ".data", ".string", "entry", "extern"
    };
    int i;

    printf("Checking macro name: %s\n", name);

    /* Check that the first character is a letter */
    if (!isalpha(name[0])) {
        printf("Invalid first character: %c (ASCII: %d)\n", name[0], name[0]);
        return 0;  
    }
    /* Check remaining characters for validity */
    for (p = name + 1; *p; p++) {
        if (!isalnum(*p) && *p != '_') {
            printf("Invalid character: %c (ASCII: %d)\n", *p, *p);
            return 0;  
        }
    }
    /* Check the length of the name */
    valid_length = strlen(name) <= MAX_MACRO_NAME;
    if (!valid_length) {
        printf("Name too long: %s\n", name);
        return 0;
    }

    /* Check against reserved words */
    for (i = 0; i < sizeof(reserved_words) / sizeof(reserved_words[0]); i++) {
        if (strcmp(name, reserved_words[i]) == 0) {
            printf("Invalid macro name: %s is a reserved word\n", name);
            return 0;
        }
    }

    return 1; 
}

int is_instruction_name(const char *name) {
    /* Array of instruction names */
    const char *instructions[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    int i;
    /* Loop through the array and compare each instruction name with the input */
    for (i = 0; i < 16; i++) {
        if (strcmp(name, instructions[i]) == 0) {
            /* Return 1 if a match is found */
            return 1;
        }
    }
    /* Return 0 if no match is found */
    return 0;
}

int pre_process(FILE *input_file, FILE *output_file, const char *filename) {
    char line[MAX_LINE_LENGTH];          /* Buffer to store each line from the file */
    char macro_body[MAX_LINE_LENGTH * 10]; /* Buffer to store the body of a macro */
    int in_macro = 0;                    /* Flag to indicate if currently inside a macro definition */
    char macro_name[MAX_MACRO_NAME] = {0}; /* Name of the current macro being defined */
    Macro *macros = NULL;                /* Linked list of macros */
    int line_number = 1;                 /* Counter for the current line number */
    char *macro_body_str;                /* Pointer to the body of a macro when expanding */
    char *line_copy;                     /* Copy of the current line for processing */
    char *token;                         /* Pointer to the current token in the line */
    int error_count = 0;                 /* Counter for the number of errors encountered */

    /* Read each line from the input file */
    while (fgets(line, sizeof(line), input_file)) {
        /* Check if the line is too long */
        if (strlen(line) > MAX_LINE_LENGTH) {
            fprintf(stderr, "Error: Line too long in file %s at line %d\n", filename, line_number);
            error_count++;
            if (error_count >= 20) break;  /* Stop processing after too many errors */
            continue;
        }

        /* Skip lines that are comments */
        if (line[0] == ';') {
            line_number++;
            continue; 
        } else {
            /* Remove comments from the line */
            char *comment = strchr(line, ';');
            if (comment) {
                *comment = '\0'; 
            }
        }

        /* Skip lines that contain only whitespace */
        if (is_whitespace_only_line(line)) {
            line_number++;
            continue;
        }

        /* Create a copy of the line for processing */
        line_copy = (char *)malloc(strlen(line) + 1);
        if (!line_copy) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            exit(1);
        }
        strcpy(line_copy, line);

        /* Tokenize the line to get the first word */
        token = strtok(line_copy, " \t\n");

        /* If the line is empty after tokenizing, continue to the next line */
        if (!token) {
            line_number++;
            free(line_copy);
            continue;
        }

        /* Check if the line starts a macro definition */
        if (strcmp(token, "macr") == 0) {
            /* Error if already inside a macro definition */
            if (in_macro) {
                fprintf(stderr, "Error: Nested macro definitions are not allowed (%s:%d)\n", filename, line_number);
                error_count++;
                free(line_copy);
                continue;
            }
            /* Start macro definition */
            in_macro = 1;
            macro_body[0] = '\0';  /* Initialize the macro body */
            token = strtok(NULL, " \t\n"); /* Get the macro name */

            if (token) {
                /* Check if the macro name is already defined */
                if (get_macro_body(macros, token)) {
                    fprintf(stderr, "Error: Macro name already defined (%s:%d)\n", filename, line_number);  
                    error_count++;
                    free(line_copy);
                    if (error_count >= 20) break;  
                    continue;
                }
                /* Check if the macro name is a valid instruction */
                if (is_instruction_name(token)) {
                    fprintf(stderr, "Error: Macro name cannot be an instruction name (%s:%d)\n", filename, line_number);
                    error_count++;
                    free(line_copy);
                    continue;
                }
                /* Check if the macro name is valid */
                if (!is_valid_macro_name(token)) {
                    fprintf(stderr, "Error: Invalid macro name (%s:%d)\n", filename, line_number);
                    error_count++;
                    free(line_copy);
                    continue;
                }
                /* Save the macro name */
                strcpy(macro_name, token);
            } else {
                /* Error if macro name is missing */
                fprintf(stderr, "Error: Macro name missing (%s:%d)\n", filename, line_number);
                error_count++;
                free(line_copy);
                continue;
            }
            /* Check for extra tokens after the macro name */
            token = strtok(NULL, " \t\n");
            if (token) {
                fprintf(stderr, "Error: Extra characters after macro name (%s:%d)\n", filename, line_number);
                error_count++;
                free(line_copy);
                continue;
            }
        } 
        /* Check if the line ends a macro definition */
        else if (strcmp(token, "endmacr") == 0) {
            /* Error if not inside a macro definition */
            if (!in_macro) {
                fprintf(stderr, "Error: endmacr without macr (%s:%d)\n", filename, line_number);
                error_count++;
                free(line_copy);
                continue;
            }
            /* Check for extra tokens after endmacr */
            token = strtok(NULL, " \t\n");
            if (token) {
                fprintf(stderr, "Error: Extra characters after endmacr (%s:%d)\n", filename, line_number);
                error_count++;
                free(line_copy);
                continue;
            }
            /* Add the completed macro to the list */
            add_macro(&macros, macro_name, macro_body);
            in_macro = 0; /* Exit macro definition mode */
        } 
        /* If currently inside a macro, append the line to the macro body */
        else if (in_macro) {
            strcat(macro_body, line);
        } 
        /* If not inside a macro, check for macro invocation and expand if found */
        else {
            macro_body_str = get_macro_body(macros, token);
            if (macro_body_str) {
                /* Write the expanded macro body to the output file */
                fputs(macro_body_str, output_file);
            } else {
                /* Write the original line to the output file */
                fputs(line, output_file);
            }
        }
        line_number++;
        free(line_copy); /* Free the line copy after processing */
    }

    /* Check if a macro definition was not properly closed */
    if (in_macro) {
        fprintf(stderr, "Error: Missing endmacr for macro %s (%s:%d)\n", macro_name, filename, line_number);
        error_count++;
    }

    /* Free all macros after processing */
    free_macros(macros);
    return (error_count > 0) ? 1 : 0;  /* Return 1 if errors were encountered, otherwise 0 */
}

int pre_process_main(int argc, char *argv[]) {
    int i;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input files...>\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        char input_filename[MAX_FILENAME_LENGTH];
        char output_filename[MAX_FILENAME_LENGTH];

        FILE *input_file, *output_file;

        strncpy(input_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        input_filename[MAX_FILENAME_LENGTH - 4] = '\0';   
        add_file_extension(input_filename, ".as");

        strncpy(output_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        output_filename[MAX_FILENAME_LENGTH - 4] = '\0'; 
        add_file_extension(output_filename, ".am");


        input_file = fopen(input_filename, "r");
        if (!input_file) {
            fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
            continue;
        }

        output_file = fopen(output_filename, "w");
        if (!output_file) {
            fprintf(stderr, "Error: Could not open output file %s\n", output_filename);
            fclose(input_file);
            continue;
        }

        if (pre_process(input_file, output_file, input_filename) != 0) {
            fprintf(stderr, "Error: Preprocessing failed for file %s\n", input_filename);
            fclose(input_file);
            fclose(output_file);
            remove(output_filename);
            return 1;
        }

        fclose(input_file);
        fclose(output_file);
    }

    return 0;
}
