#ifndef SYMBOL_TABLE_HEADER

#define SYMBOL_TABLE_HEADER

#include <glib.h>
#include <stdlib.h>
#include "Type.h"
#include "String.h"
#include "Logger.h"


/**
 * For simplicity of implementation, we use glib implementation of hashtables to store the key-values of the symbol table 
 * We hide the implementation details of the symbol table using a struct
 **/

/**
 * Variable are stored in a hashtable, where the key is the identifier of the variable and the value is a type.
 * TODO maybe later on we will need to store more information about the variables ... 
 */

typedef struct {
    ObjectType type;        // Type of the symbol
    int currentSlide;       // Name of the symbol
} SymbolTableItem;


typedef struct {
    GHashTable *table;       // The hashtable that contains the symbols
} SymbolTable;


/* To log semantic errors as suck  */
/** Initialize module's internal state. */
void initializeSemanticsModule();

/** Shutdown module's internal state. */
void shutdownSemanticsModule();

/**
 * Initializes the symbol table.
 * @return A pointer to the initialized symbol table.
 */
SymbolTable *initializeSymbolTable();

/**
 * Adds a symbol to the symbol table.
 * @param symbolTable The symbol table to add the symbol to.
 * @param identifier The identifier of the symbol.
 * @param item The SymbolTableItem to add.
 */
void addSymbol(SymbolTable *symbolTable, const char *identifier, ObjectType type);


/**
 * Retrieves a symbol from the symbol table.
 * @param symbolTable The symbol table to retrieve the symbol from.
 * @param identifier The identifier of the symbol to retrieve.
 * @return The SymbolTableItem if found, NULL otherwise.
 */
SymbolTableItem *getSymbol(SymbolTable *symbolTable, const char *identifier);

/**
 * Retrieves a symbol's type from the symbol table.
 * @param symbolTable The symbol table to retrieve the symbol from.
 * @param identifier The identifier of the symbol to retrieve.
 * @return The object type if found, OBJ_UNKNOWN otherwise.
 */
ObjectType getSymbolType(SymbolTable *symbolTable, const char *identifier);

/**
 * Retrieves a symbol from the symbol table, checking if it is a slide.
 * @param symbolTable The symbol table to retrieve the symbol from.
 * @param identifier The identifier of the symbol to retrieve.
 * @return The current slide  (0 if none)
 */
int getSymbolCurrentSlide(SymbolTable *symbolTable, const char *identifier);

/**
 * Checks if a symbol exists in the symbol table.
 * @param symbolTable The symbol table to check.
 * @param identifier The identifier of the symbol to check.
 * @return True if the symbol exists, false otherwise.
 */
boolean symbolExists(SymbolTable *symbolTable, const char *identifier);

/**
 * Destroys the symbol table and frees its resources.
 * @param symbolTable The symbol table to destroy.
 */
void destroySymbolTable(SymbolTable *symbolTable);


#endif





