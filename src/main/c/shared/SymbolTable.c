#include "SymbolTable.h"


static Logger * _logger = NULL;



void initializeSemanticsModule(){
    _logger = createLogger("Semantics");
}

void shutdownSemanticsModule(){
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

SymbolTable *initializeSymbolTable(){
    GHashTable * table = g_hash_table_new(g_str_hash, g_str_equal);
    if(table != NULL){
        SymbolTable * symbolTable = malloc(sizeof(SymbolTable));
        if(symbolTable != NULL){
            symbolTable->table = table;
            logDebugging(_logger, "Symbol table initialized successfully");
            return symbolTable;
        } else {
            logError(_logger, "Failed to allocate memory for SymbolTable");
            g_hash_table_destroy(table);
            return NULL;
        }
    }
    else {
        logError(_logger, "Failed to create symbol table");
        return NULL;
    }
}


void addSymbol(SymbolTable *symbolTable, const char *identifier, SymbolType type){
    logDebugging(_logger, "Adding symbol: %s of type %d", identifier, type);
    if (symbolTable == NULL || identifier == NULL) {
        logError(_logger, "Cannot add symbol: symbolTable or identifier is NULL");
        return;
    }
    
    // Check if the symbol already exists
    if (symbolExists(symbolTable, identifier)) {
        logDebugging(_logger, "Symbol %s already exists in the symbol table", identifier);
        return;
    }
    g_hash_table_insert(symbolTable->table, g_strdup(identifier), GINT_TO_POINTER(type));
}


SymbolType getSymbolType(SymbolTable *symbolTable, const char *identifier) {
    logDebugging(_logger, "Retrieving symbol type for identifier: %s", identifier);
    if (symbolTable == NULL || identifier == NULL) {
        logError(_logger, "Cannot retrieve symbol type: symbolTable or identifier is NULL");
        return SYMBOL_TYPE_UNKNOWN;
    }

    gpointer value = g_hash_table_lookup(symbolTable->table, identifier);
    int intValue = GPOINTER_TO_INT(value);
    if (value != NULL) {
        logDebugging(_logger, "Symbol %s found in the symbol table with type %d", identifier, intValue);
        return (SymbolType)intValue;
    } else {
        logError(_logger, "Symbol %s not found in the symbol table", identifier);
        return SYMBOL_TYPE_UNKNOWN;
    }
}

boolean symbolExists(SymbolTable *symbolTable, const char *identifier) {
    logDebugging(_logger, "Checking if symbol exists: %s", identifier);
    if (symbolTable == NULL || identifier == NULL) {
        logError(_logger, "Cannot check symbol existence: symbolTable or identifier is NULL");
        return false;
    }

    if(g_hash_table_contains(symbolTable->table, identifier)){
        logDebugging(_logger, "Symbol %s exists in the symbol table", identifier);
        return true;
    }
    return false;
}
boolean symbolExists(SymbolTable *symbolTable, const char *identifier);


void destroySymbolTable(SymbolTable *symbolTable){
    logDebugging(_logger, "Destroying symbol table");
    if (symbolTable != NULL) {
        g_hash_table_destroy(symbolTable->table);
        free(symbolTable);
        logDebugging(_logger, "Symbol table destroyed successfully");
    } else {
        logError(_logger, "Cannot destroy symbol table: symbolTable is NULL");
    }
}
