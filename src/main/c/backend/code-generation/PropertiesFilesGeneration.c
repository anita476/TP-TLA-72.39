#include "PropertiesFilesGeneration.h"

void initializePropertiesFilesGenerationModule() {}
/** Shutdown module's internal state. */
void shutdownPropertiesFilesGenerationModule() {}

void generatePropertiesFiles(CompilerState *compilerState) {
    // create the properties dir (styles)

    // for each item in the symbol table, if properties isnt null:
    //
    //      generate the properties prologue,
    //.     generate the properties string,
    //.     close the file
}