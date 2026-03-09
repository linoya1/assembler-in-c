#ifndef UTILITY_H
#define UTILITY_H
#include "constants.h"
#include "syntax_analyzer.h"



/**
 * @brief Checks if the given token is a valid label.
 * 
 * @param token The string to be checked as a label.
 * @return int Returns 1 if the token is a valid label, 0 otherwise.
 */
int is_label(const char *token);

/**
 * @brief Checks if the given line is blank (contains only whitespace).
 * 
 * @param line The line of text to be checked.
 * @return int Returns 1 if the line is blank, 0 otherwise.
 */
int is_blank_line(const char *line);

/**
 * @brief Adds the specified file extension to the given filename.
 * 
 * @param filename The original filename without an extension.
 * @param extension The file extension to be added (e.g., ".txt").
 */
void add_file_extension(char *filename, const char *extension);

/**
 * @brief Trims specified characters from both ends of the string.
 * 
 * @param str The string to be trimmed.
 * @param chars_to_trim A string containing characters to be trimmed from the ends of `str`.
 * @return char* A pointer to the trimmed string.
 */
char *trim_characters(char *str, const char *chars_to_trim);

/**
 * @brief Trims whitespace and special characters from both ends of the string.
 * 
 * @param str The string to be trimmed of whitespace and special characters.
 * @return char* A pointer to the trimmed string.
 */
char *trim_whitespace_and_special(char *str);

#endif
