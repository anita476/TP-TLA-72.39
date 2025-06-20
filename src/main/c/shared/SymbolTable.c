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

static void freeSymbolTableItem(gpointer item) {
    if (item != NULL) {
        SymbolTableItem *symbolItem = (SymbolTableItem *)item;
        g_array_free(symbolItem->appearsIn, true); 
        free(symbolItem);
    }
}


SymbolTable *initializeSymbolTable(){
    GHashTable * table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeSymbolTableItem);
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


void addSymbol(SymbolTable *symbolTable, const char *identifier, ObjectType type){
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

    SymbolTableItem *item = malloc(sizeof(SymbolTableItem));
    if (item != NULL) {
        item->type = type;
        item->currentSlide = -1; // Initialize currentSlide to -1 (not assigned)
        item->appearsIn = g_array_new(false, true, sizeof(int)); 
        g_hash_table_insert(symbolTable->table, g_strdup(identifier), item);
    } else {
        logError(_logger, "Failed to allocate memory for SymbolTableItem");
    }
}

SymbolTableItem *getSymbol(SymbolTable *symbolTable, const char *identifier) {
    logDebugging(_logger, "Retrieving symbol for identifier: %s", identifier);
    if (symbolTable == NULL || identifier == NULL) {
        logError(_logger, "Cannot retrieve symbol: symbolTable or identifier is NULL");
        return NULL;
    }

    SymbolTableItem *item = g_hash_table_lookup(symbolTable->table, identifier);
    if (item != NULL) {
        logDebugging(_logger, "Symbol %s found in the symbol table with type %d", identifier, item->type);
        return item;
    } else {
        logDebugging(_logger, "Symbol %s not found in the symbol table", identifier);
        return NULL;
    }
}


ObjectType getSymbolType(SymbolTable *symbolTable, const char *identifier) {

    SymbolTableItem *item = getSymbol(symbolTable, identifier);
    if (item != NULL) {
        return item->type;
    }
    return OBJ_UNKNOWN;
}

int getSymbolCurrentSlide(SymbolTable *symbolTable, const char *identifier) {
    SymbolTableItem *item = getSymbol(symbolTable, identifier);
    return item->currentSlide;
}

boolean symbolExists(SymbolTable *symbolTable, const char *identifier) {
    logDebugging(_logger, "Checking if symbol exists: %s", identifier);
    if (symbolTable == NULL || identifier == NULL) {
        logError(_logger, "Cannot check symbol existence: symbolTable or identifier is NULL");
        return false;
    }
    return g_hash_table_contains(symbolTable->table, identifier);
}

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
