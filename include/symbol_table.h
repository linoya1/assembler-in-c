#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "constants.h"
#include "syntax_analyzer.h"
#include "opcode_table.h"

/**
 * NAME: LINOY BITON
 * ID:211589775 */

typedef enum {
    extern_symbol,
    entry_symbol,
    code_symbol,
    data_symbol,
    entry_code_symbol,
    entry_data_symbol
} SymbolType;

typedef struct Symbol {
    char name[MAX_LABEL_LEN + 1];
    int address;
    int is_extern;
    int is_entry;
    SymbolType type;
    int line_number;
    int *entry_addresses;  
    int entry_address_count;
    int *external_addresses;  
    int external_address_count;
    struct Symbol *next;
} Symbol;

/**
 * @brief Initializes the symbol table.
 * 
 * @param symbol_table Pointer to the head of the symbol table.
 */
void init_symbol_table(Symbol **symbol_table);

/**
 * @brief Frees the memory used by the symbol table.
 * 
 * @param head Pointer to the head of the symbol table.
 */
void free_symbol_table(Symbol *head);

/**
 * @brief Retrieves a symbol from the symbol table.
 * 
 * @param head Pointer to the head of the symbol table.
 * @param name The name of the symbol to retrieve.
 * @return Pointer to the symbol if found, NULL otherwise.
 */
Symbol *get_symbol(Symbol *head, const char *name);

/**
 * @brief Adds a new symbol to the symbol table.
 * 
 * @param head Pointer to the head of the symbol table.
 * @param name The name of the symbol to add.
 * @param address The address of the symbol.
 * @param is_extern Flag indicating if the symbol is external.
 * @param is_entry Flag indicating if the symbol is an entry.
 * @param type The type of the symbol.
 * @param line_number The line number where the symbol is defined.
 */
void add_symbol(Symbol **head, const char *name, int address, int is_extern, int is_entry, SymbolType type, int line_number);

/**
 * @brief Loads the symbol table from a file.
 * 
 * @param filename The name of the file to load the symbol table from.
 * @return Pointer to the loaded symbol table.
 */
Symbol* load_symbol_table(const char *filename);

/**
 * @brief Retrieves the address of a symbol from the symbol table.
 * 
 * @param symbol_table Pointer to the symbol table.
 * @param symbol The name of the symbol.
 * @return The address of the symbol, or -1 if not found.
 */
int get_symbol_address(Symbol *symbol_table, const char *symbol);

/**
 * @brief Checks if a label is defined in the symbol table.
 * 
 * @param symbol_table Pointer to the symbol table.
 * @param name The name of the label.
 * @return 1 if the label is defined, 0 otherwise.
 */
int is_label_defined(Symbol *symbol_table, const char *name);

/**
 * @brief Checks if a label is external in the symbol table.
 * 
 * @param symbol_table Pointer to the symbol table.
 * @param name The name of the label.
 * @return 1 if the label is external, 0 otherwise.
 */
int is_label_extern(Symbol *symbol_table, const char *name);


/**
 * @brief Checks if a direct label is valid in the symbol table.
 * 
 * @param symbol_table Pointer to the symbol table.
 * @param label_name The name of the label.
 * @return 1 if the label is valid, 0 otherwise.
 */
int is_valid_direct_label(Symbol *symbol_table, const char *label_name);
/**
 * @brief Checks if a label is valid in the given symbol table.
 * 
 * @param symbol_table Pointer to the symbol table.
 * @param label_name The name of the label.
 * @return 1 if the label is valid, 0 otherwise.
 */
int is_label_valid_in_given_table(Symbol *symbol_table, const char *label_name);

/**
 * @brief Checks if a symbol type is an entry.
 * 
 * @param type The type of the symbol.
 * @param line_number The line number of the symbol.
 * @return 1 if the symbol is an entry, 0 otherwise.
 */
int is_entry(SymbolType type, int line_number);

#endif

