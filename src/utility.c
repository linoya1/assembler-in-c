#include "constants.h"
#include "utility.h"
#include "syntax_analyzer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int is_label(const char *token) {
    size_t len = strlen(token);
    if (len > 0 && token[len - 1] == ':') {
        char label[MAX_LABEL_LEN + 1];
        strncpy(label, token, len - 1);
        label[len - 1] = '\0';
        return is_valid_label_name(label);
    }
    return 0;
}

int is_blank_line(const char *line) {
    while (*line) {
        if (!isspace((unsigned char)*line)) {
            return 0;
        }
        line++;
    }
    return 1;
}

void add_file_extension(char *filename, const char *extension) {
    char *dot = strrchr(filename, '.');
    if (dot) {
        *dot = '\0';
    }
    strcat(filename, extension);
}


char *trim_characters(char *str, const char *chars_to_trim) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

char *trim_whitespace_and_special(char *str) {
    char *end;

    while (*str && (isspace((unsigned char)*str) || *str == ',' || *str == '\n')) str++;

    if (*str == 0)  
        return str;

    end = str + strlen(str) - 1;
    while (end > str && (isspace((unsigned char)*end) || *end == ',' || *end == '\n')) 
        end--;

    end[1] = '\0';

    return str;
}

