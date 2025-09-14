#include "constants.h"
#include "symbol_table.h"
#include "syntax_analyzer.h"
#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void init_symbol_table(Symbol **symbol_table) {
    *symbol_table = NULL; /* Initialize the symbol table as empty */
}

void free_symbol_table(Symbol *head) {
    Symbol *current = head;
    Symbol *next;

    /* Traverse the symbol table and free each symbol */

    while (current != NULL) {
        next = current->next;
        if (current->entry_addresses) {
            free(current->entry_addresses);
            current->entry_addresses = NULL;
        }
        if (current->external_addresses) {
            free(current->external_addresses);
            current->external_addresses = NULL;
        }
        free(current);
        current = next;
    }
}

Symbol* get_symbol(Symbol *head, const char *name) {
    Symbol *current;

    /* Check if the name is valid */
    if (name == NULL || *name == '\0') {
        return NULL;
    }

    /* Traverse the symbol table to find the symbol by name */
    current = head;
    while (current != NULL) {
        if (strstr(name, current->name) !=  NULL) {
/* Use strstr to find partial matches */
            return current;
        }
        current = current->next;
    }

    return NULL; /* Return NULL if the symbol is not found */
}

void add_symbol(Symbol **head, const char *name, int address, int is_extern, int is_entry, SymbolType type, int line_number) {
    Symbol *existing_symbol = get_symbol(*head, name);
    Symbol *new_symbol;
    /* If the symbol already exists, update its properties */
    if (existing_symbol != NULL) {
        if (existing_symbol->address == -1 && address != -1) {  
            existing_symbol->address = address; /* Update address if previously undefined */
        }
        if (is_entry) {
            existing_symbol->is_entry = 1;
            if (existing_symbol->entry_addresses == NULL) {
                existing_symbol->entry_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
                if (!existing_symbol->entry_addresses) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    exit(1);
                }
                existing_symbol->entry_address_count = 0;
            }
            existing_symbol->entry_addresses[existing_symbol->entry_address_count++] = address;
        }
        if (is_extern) {
            existing_symbol->is_extern = 1;
            if (existing_symbol->external_addresses == NULL) {
                existing_symbol->external_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
                if (!existing_symbol->external_addresses) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    exit(1);
                }
                existing_symbol->external_address_count = 0;
            }
            if (address != -1) { 
                existing_symbol->external_addresses[existing_symbol->external_address_count++] = address;
            }
        }
        existing_symbol->type = type;
        return;
    }

    /* Create a new symbol if it doesn't exist */
    new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (!new_symbol) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    strcpy(new_symbol->name, name);
    new_symbol->address = address; 
    new_symbol->is_extern = is_extern;
    new_symbol->is_entry = is_entry;
    new_symbol->type = type;
    new_symbol->line_number = line_number;
    new_symbol->next = NULL;
    /* Initialize address arrays */
    new_symbol->entry_addresses = NULL;
    new_symbol->entry_address_count = 0;
    new_symbol->external_addresses = NULL;
    new_symbol->external_address_count = 0;

    if (is_entry) {
        new_symbol->entry_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
        if (!new_symbol->entry_addresses) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free(new_symbol);
            exit(1);
        }
        new_symbol->entry_addresses[new_symbol->entry_address_count++] = address;
    }

    if (is_extern) {
        new_symbol->external_addresses = (int *)malloc(MAX_ADDRESSES * sizeof(int));
        if (!new_symbol->external_addresses) {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free(new_symbol->entry_addresses);
            free(new_symbol);
            exit(1);
        }
        if (address != -1) {  
            new_symbol->external_addresses[new_symbol->external_address_count++] = address;
        }
    }

    /* Add the new symbol to the end of the symbol table */
    if (*head == NULL) {
        *head = new_symbol;
    } else {
        Symbol *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_symbol;
    }
}


int get_symbol_address(Symbol *symbol_table, const char *symbol) {
    Symbol *current = symbol_table;
    /* Traverse the table to find the symbol by name */
    while (current != NULL) {
        if (strcmp(current->name, symbol) == 0) {
            return current->address;
        }
        current = current->next;
    }
    return -1;
}

int is_label_defined(Symbol *symbol_table, const char *name) {
    Symbol *symbol = get_symbol(symbol_table, name);
    return (symbol != NULL);
}

int is_label_extern(Symbol *symbol_table, const char *name) {
    Symbol *symbol = get_symbol(symbol_table, name);
    return (symbol != NULL && symbol->is_extern);
}

int is_valid_direct_label(Symbol *symbol_table, const char *label_name) {
    /* Check if the label is defined or external */
    if (is_label_defined(symbol_table, label_name)) {
        return 1;
    }

    if (is_label_extern(symbol_table, label_name)) {
        return 1;
    }
    return 0; /* Return 0 if the label is neither defined nor external */
}

