#include "Generator.h"
#include <sys/stat.h>

/* MODULE INTERNAL STATE */
FILE *_outputFile = NULL;
boolean success = true; // in case of error
const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger *_logger = NULL;

void initializeGeneratorModule() { _logger = createLogger("Generator"); }

void shutdownGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}
/* helpers */
static void generatePrologue(char *presName);
static void generateEpilogue();

static void generateSlide(Slide *slide);
static void generateSlides(CompilerState *compilerState);
static AnimationType findAnimationTransition(Slide *slide, Program *program);

/** PUBLIC FUNCTIONS */
void generate(CompilerState *compilerState) {
    logDebugging(_logger, "Generating final output...");

    int mkdirStatus = mkdir("output", 0755);
    // write mode to overwrite several calls to the same presentation file
    Program *ast = (Program *)compilerState->abstractSyntaxtTree;
    char *title = ast->presentation_identifier;
    char filepath[256]; // If its too long it will truncate (invalid file but it wont leak memory)
    snprintf(filepath, sizeof(filepath), "output/%s.html", title);
    _outputFile = fopen(filepath, "w");
    if (_outputFile == NULL) {
        logError(_logger, "Cannot open output file for writing");
        return;
    }
    generatePrologue(title);
    // TODO generate css classes for objects
    generateSlides(compilerState);
    generateEpilogue();
    fclose(_outputFile);
    _outputFile = NULL; //  step on pointer
    logDebugging(_logger, "Generation is done.");
}

/* Private func definitions */
void generateSlide(Slide *slide) {
    fprintf(_outputFile, "<h2>Slide: %s</h2>\n", slide->identifier);
    for (int i = slide->maxRow; i >= slide->minRow; i--) {
        Row *row = g_hash_table_lookup(slide->rows, int_key(i));
        if (row) {
            fprintf(_outputFile, "<div class='row'>\n");
            logDebugging(_logger, "Min column: %d, Max column: %d for row %d in slide %s",
                         row->minCol, row->maxCol, i, slide->identifier);
            for (int j = row->minCol; j <= row->maxCol; j++) {
                logDebugging(_logger, "Printing column %d for row %d in slide %s", j, i,
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

static void generateSlides(CompilerState *compilerState) {
    Program *program = compilerState->abstractSyntaxtTree;
    logDebugging(_logger, "Printing each slide in the presentation...");
    if (_outputFile == NULL) {
        logError(_logger, "Cannot open output file for writing");
        return;
    }
    fprintf(_outputFile, "<h1>Slides Overview</h1>\n");
    // iterate through the slides and print them (to file)
    int i = 0;
    for (Slide *slide = compilerState->slides->head; slide != NULL; slide = slide->next, i++) {
        AnimationType anim = findAnimationTransition(slide, program);
        char *animS;
        switch (anim) {
        case ANIM_FADE_INTO:
            animS = "data-transition='fade'";
            break;
        case ANIM_JUMP_INTO:
            animS = "data-transition='jump'";
            break;
        default:
            animS = "";
            break;
        }
        logInformation(_logger, "Slide %s with animation type %s %d", slide->identifier, animS,
                       anim);

        if (i != 0) {
            fprintf(_outputFile, "<div class='slide %s' %s data-repeats='1'>\n", slide->identifier,
                    animS);

        } else {
            fprintf(_outputFile, "<div class='slide %s active' %s data-repeats='1'>\n",
                    slide->identifier, animS);
        }
        generateSlide(slide);
        fprintf(_outputFile, "</div>\n");
    }
    fprintf(_outputFile, "<p>End of slides overview.</p>\n");
    if (!success) {
        logCritical(_logger, "There were errors during the generation process.");
    }
    logDebugging(_logger, "Finished printing each slide.");
}

static void generatePrologue(char *presName) {
    fprintf(_outputFile, "<!DOCTYPE html>\n");
    fprintf(_outputFile, "<html lang='en'>\n");
    fprintf(_outputFile, "<head>\n");
    fprintf(_outputFile, "<meta charset='UTF-8'>\n");
    fprintf(_outputFile,
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    fprintf(_outputFile, "<title>%s</title>\n", presName);
    fprintf(_outputFile, "<link rel='stylesheet' href='../src/main/web/css/styles.css'>\n");
    fprintf(_outputFile, "</head>\n");
    fprintf(_outputFile, "<body>\n");
    fprintf(_outputFile, "<div class='class-container'>\n");
}

static void generateEpilogue() {
    fprintf(_outputFile, "</div>\n");
    fprintf(_outputFile, "<div class='slide-number'></div>\n");
    fprintf(_outputFile, "<script src='../src/main/web/js/core.js'></script>\n");
    fprintf(_outputFile, "<script src='../src/main/web/js/animation.js'></script>\n");
    fprintf(_outputFile, "<script src='../src/main/web/js/transition.js'></script>\n");
    fprintf(_outputFile, "<script src='../src/main/web/js/animation-sequence.js'></script>");
    fprintf(_outputFile, "<script src='../src/main/web/js/slides.js'></script>");
    fprintf(_outputFile, "<script src='../src/main/web/js/controller.js'></script>");
    fprintf(_outputFile, "<script src='../src/main/web/js/ui.js'></script>");
    fprintf(_outputFile, "<script src='../src/main/web/js/main.js'></script>");
    fprintf(_outputFile, "</body>\n");
    fprintf(_outputFile, "</html>\n");
}

// TODO modify ast so that the slide animations are in a different slide (more efficient for many
// slides)
static AnimationType findAnimationTransition(Slide *slide, Program *program) {
    for (AnimationDefinition *animations = program->animation_definitions; animations != NULL;
         animations = animations->next) {
        if (animations->kind == ANIM_DEF_PAIR &&
            (strcasecmp(animations->pair.identifier1, slide->identifier))) {
            return animations->pair.type;
        }
    }
    return ANIM_NO_ANIM;
}