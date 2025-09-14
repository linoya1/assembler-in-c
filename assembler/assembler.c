#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pre_process.h"
#include "first_pass.h"
#include "second_pass.h"
#include "symbol_table.h"
#include "syntax_analyzer.h"
#include "utility.h"

#define MAX_FILENAME_LENGTH 300


int main(int argc, char *argv[]) {
    int i;
    Symbol *symbol_table = NULL;
    Symbol *symbol_memory_table = NULL;
    Line current_line;
    FILE *input_file, *am_file, *first_pass_file, *second_pass_file;
    char binary_filename[MAX_FILENAME_LENGTH];
    int errors_in_first_pass = 0;
    int errors_in_second_pass = 0;
    int IC, DC;
    char output_filename[MAX_FILENAME_LENGTH];
    char *dot_as;
    FILE *output_file = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input files...>\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        char input_filename[MAX_FILENAME_LENGTH + 1];
        char am_filename[MAX_FILENAME_LENGTH + 1];
        char ob_filename[MAX_FILENAME_LENGTH + 1];
        char ent_filename[MAX_FILENAME_LENGTH + 1];
        char ext_filename[MAX_FILENAME_LENGTH + 1];

        /* Construct input and output filenames */

        strncpy(input_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        add_file_extension(input_filename, ".as");

        strncpy(am_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        add_file_extension(am_filename, ".am");

        input_file = fopen(input_filename, "r");
        if (!input_file) {
            perror("Error opening input file");
            return 1;
        }

        am_file = fopen(am_filename, "w");
        if (!am_file) {
            perror("Error opening output file");
            fclose(input_file);
            return 1;
        }

        /* Pre-processing stage */

        if (pre_process(input_file, am_file, input_filename) != 0) {
            fprintf(stderr, "Error: Preprocessing failed for file %s\n", input_filename);
            fclose(input_file);
            fclose(am_file);
            remove(am_filename);
            continue;
        }

        fclose(input_file);
        fclose(am_file);

        first_pass_file = fopen(am_filename, "r");
        if (!first_pass_file) {
            perror("Error opening first pass file");
            return 1;
        }

        init_symbol_table(&symbol_table);

        initialize_line(&current_line);

        IC = 0;
        DC = 0;
        /* First pass */

        if (first_pass(first_pass_file, &symbol_table, current_line.errors, current_line.warnings, &IC, &DC)) {
            errors_in_first_pass = 1;  
        }
        fclose(first_pass_file);

        print_errors(&current_line);
        save_symbol_table(&symbol_table, &symbol_memory_table);


        /* Construct output filenames */

        strncpy(ob_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        add_file_extension(ob_filename, ".ob");

        strncpy(ent_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        add_file_extension(ent_filename, ".ent");

        strncpy(ext_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        add_file_extension(ext_filename, ".ext");

        strncpy(binary_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        add_file_extension(binary_filename, ".bin");

        second_pass_file = fopen(am_filename, "r");
        if (!second_pass_file) {
            perror("Error opening second pass file");
            if (symbol_table != NULL) {
                free_symbol_table(symbol_table);
                symbol_table = NULL;
            }
            continue;
        }

        strncpy(output_filename, argv[i], MAX_FILENAME_LENGTH - 4);
        dot_as = strstr(output_filename, ".as");
        if (dot_as != NULL) {
            *dot_as = '\0';
        } else {
            char *last_dot = strrchr(output_filename, '.');
            if (last_dot != NULL) {
                *last_dot = '\0';
            }
        }
        strcat(output_filename, ".ob");

        output_file = fopen(output_filename, "w");
        if (!output_file) {
            fprintf(stderr, "Error: Could not open output file %s\n", output_filename);
            fclose(second_pass_file);
            continue;
        }

        /* Final IC and DC */
        fprintf(output_file, "  %d %d\n", IC, DC);

        /* Second pass */

        if (second_pass(second_pass_file, argv[i], symbol_table, output_file)) {
            errors_in_second_pass = 1;  
        }

        fclose(second_pass_file);
        fclose(output_file);

        /* Remove output files if there are errors */

        if (errors_in_first_pass || errors_in_second_pass) {
            fprintf(stderr, "Errors detected. Removing output files: %s, %s, %s\n", ob_filename, ent_filename, ext_filename);
            remove(ob_filename);
            remove(ent_filename);
            remove(ext_filename);
        }



        free_errors(&current_line);
        free_warnings(&current_line);
    }

    if (symbol_memory_table) {
        free_symbol_table(symbol_memory_table);
        symbol_memory_table = NULL;  
    }
    return 0;
}

