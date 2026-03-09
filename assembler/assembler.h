#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>



/**
 * NAME: LINOY BITON
 * @brief The main function of the assembler program.
 *
 * This function serves as the entry point for the assembler program, 
 * which processes assembly files provided as command-line arguments. 
 * It initializes the assembler, processes input files, and generates 
 * the appropriate output files.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments. 
 *             The arguments typically include the program name and the 
 *             filenames of the assembly files to be processed.
 * 
 * @return Returns 0 on successful execution, otherwise returns a non-zero 
 *         error code indicating the type of failure.
 */
int main(int argc, char *argv[]);


#define MAX_FILENAME_LENGTH 300

#endif 

