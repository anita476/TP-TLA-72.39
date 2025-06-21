#include "backend/code-generation/Generator.h"
#include "backend/domain-specific/Layout.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/Environment.h"
#include "shared/Logger.h"
#include "shared/String.h"
#include "shared/SymbolTable.h"

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int count, const char **arguments) {
    LoggingLevel logTo = INFORMATION;
    Logger *logger = createLogger("EntryPoint");
    initializeFlexActionsModule();
    initializeBisonActionsModule();
    initializeSyntacticAnalyzerModule();
    initializeAbstractSyntaxTreeModule();
    initializeSemanticsModule();
    initializeObjectsModule();
    initializeGeneratorModule();

    // Logs the arguments of the application.
    for (int k = 0; k < count; ++k) {
        logInformation(logger, arguments[k]);
        if (!strcasecmp(arguments[k], "-v")) {
            logTo = DEBUGGING;
        }
    }
    setGlobalLoggingLevel(logTo);

    // Begin compilation process.
    CompilerState compilerState = {.abstractSyntaxtTree = NULL,
                                   .symbolTable = initializeSymbolTable(),
                                   .succeed = false,
                                   .value = 0,
                                   .slideCounter = 0,
                                   .slides = NULL};
    const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
    CompilationStatus compilationStatus = SUCCEED;
    Program *program = compilerState.abstractSyntaxtTree;
    if (syntacticAnalysisStatus == ACCEPT) {
        // ----------------------------------------------------------------------------------------
        // Beginning of the Backend... ------------------------------------------------------------
        logDebugging(logger, "Generating layout maps...");

        compilerState.slides = generateObjects(program);
        if (compilerState.slides == NULL) {
            logWarning(logger, "No slides found");
            compilationStatus = ACCEPT;
        } else {
            generate(&compilerState);
        }

        // ...end of the Backend. -----------------------------------------------------------------
        // ----------------------------------------------------------------------------------------
    } else {
        logCritical(logger, "The syntactic-analysis phase rejects the input program.");
        compilationStatus = FAILED;
    }
    logDebugging(logger, "Releasing AST resources...");
    destroyProgram(program);
    logDebugging(logger, "Releasing symbol table resources...");
    destroySymbolTable(compilerState.symbolTable);
    logDebugging(logger, "Releasing compiler state resources...");
    destroyObjects(compilerState.slides);
    logDebugging(logger, "Releasing modules resources...");
    shutdownGeneratorModule();
    shutdownObjectsModule();
    shutdownSemanticsModule();
    shutdownAbstractSyntaxTreeModule();
    shutdownSyntacticAnalyzerModule();
    shutdownBisonActionsModule();
    shutdownFlexActionsModule();
    logDebugging(logger, "Compilation is done.");
    destroyLogger(logger);
    return compilationStatus;
}
