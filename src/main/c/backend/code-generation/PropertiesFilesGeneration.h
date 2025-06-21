#ifndef PROPERTIES_FILES_GENERATION_HEADER
#define PROPERTIES_FILES_GENERATION_HEADER
#include "../../shared/CompilerState.h"
#include "../../shared/String.h"
#include <stdarg.h>
#include <stdio.h>
/** Initialize module's internal state. */
void initializePropertiesFilesGenerationModule();
/** Shutdown module's internal state. */
void shutdownPropertiesFilesGenerationModule();

/**
 * Style files will be generated in directoy "assets/styles"
 * There should be one file per presentation (because styles are global..)
 */

/**
 * Generates the properties files using the  compiler state.
 */
void generatePropertiesFiles(CompilerState *compilerState);

#endif