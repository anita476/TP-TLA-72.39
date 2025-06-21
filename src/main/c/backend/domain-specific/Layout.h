#ifndef OBJECTS_HEADER
#define OBJECTS_HEADER

/**
 * We reuse the types from the AST for convenience, but you should separate
 * the layers of the backend and frontend using another group of
 * domain-specific models or DTOs (Data Transfer Objects).
 */
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include <limits.h>

void initializeObjectsModule();
void shutdownObjectsModule();

SlideList *generateObjects(CompilerState *compilerState);

void destroyObjects(SlideList *slides);

#endif
