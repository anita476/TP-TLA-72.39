#include "Generator.h"
#include <sys/stat.h>

/* MODULE INTERNAL STATE */
FILE *_outputFile = NULL;
const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger *_logger = NULL;

void initializeGeneratorModule() { _logger = createLogger("Generator"); }

void shutdownGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/** PRIVATE FUNCTIONS */

static void printEachSlide(CompilerState *compilerState) {
    logDebugging(_logger, "Printing each slide in the presentation...");
    if (_outputFile == NULL) {
        logError(_logger, "Cannot open output file for writing");
        return;
    }
    fprintf(_outputFile, "<h1>Slides Overview</h1>\n");
    // Here we would iterate through the slides and print them (to file)
    for (Slide *slide = compilerState->slides->head; slide != NULL; slide = slide->next) {
        fprintf(_outputFile, "<h2>Slide: %s</h2>\n", slide->identifier);
        for (int i = slide->maxRow; i >= slide->minRow; i--) {
            Row *row = g_hash_table_lookup(slide->rows, int_key(i));
            if (row) {
                fprintf(_outputFile, "<div class='row'>\n");
                logInformation(_logger, "Min column: %d, Max column: %d for row %d in slide %s",
                               row->minCol, row->maxCol, i, slide->identifier);
                for (int j = row->minCol; j <= row->maxCol; j++) {
                    logInformation(_logger, "Printing column %d for row %d in slide %s", j, i,
                                   slide->identifier);
                    PositionedObject *obj = g_hash_table_lookup(row->columns, int_key(j));
                    if (obj) {
                        fprintf(_outputFile, "<span class='object'>%s</span>\n", obj->identifier);
                    } else {
                        logWarning(_logger, "Column %d not found in row %d of slide %s", j, i,
                                   slide->identifier);
                    }
                }
            }
            fprintf(_outputFile, "</div>\n");
        }
    }
    fprintf(_outputFile, "<p>End of slides overview.</p>\n");
    logDebugging(_logger, "Finished printing each slide.");
}

/** PUBLIC FUNCTIONS */
void callToPrint() {
    logDebugging(_logger, "Calling to print the presentation...");
    fprintf(_outputFile, "<h1>Welcome to the generated presentation!</h1>\n");
    fprintf(_outputFile, "<p>This is a placeholder for the content of the presentation.</p>\n");
    fprintf(_outputFile, "<p>More content will be added here...</p>\n");
    fprintf(_outputFile, "</body>\n</html>\n");
}

void generate(CompilerState *compilerState) {
    logDebugging(_logger, "Generating final output...");
    int mkdirStatus = mkdir("output", 0755);
    // In append mode to write to the end of the file, create otherwise
    _outputFile = fopen("output/presentation.html", "a");
    if (_outputFile == NULL) {
        logError(_logger, "Cannot open output file for writing");
        return;
    }
    printEachSlide(compilerState);
    //_generatePrologue();
    //_generateProgram(compilerState->abstractSyntaxtTree);
    //_generateEpilogue(compilerState->value);
    fclose(_outputFile);
    logDebugging(_logger, "Generation is done.");
}
