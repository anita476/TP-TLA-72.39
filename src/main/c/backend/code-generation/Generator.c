#include "Generator.h"
#include <stdlib.h>
#include <sys/stat.h>

FILE *_outputFile = NULL;
boolean success = true; // in case of error
const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger *_logger = NULL;

/* MODULE INTERNAL STATE */

void initializeGeneratorModule() { _logger = createLogger("Generator"); }

void shutdownGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}
/* helpers */
static void generatePrologue(CompilerState *compilerState);
static void generateEpilogue();
static void outputProperties(CompilerState *compilerState);

static void generateSlide(Slide *slide, AnimationDefinition *sequence, SymbolTable *symbolTable);
static void generateSlides(CompilerState *compilerState);
static void generateItem(SymbolTableItem *object, char *identifier, char *animations, char *orders);
static AnimationType findAnimationTransition(Slide *slide, Program *program);
static AnimationDefinition *findSlideAnimationSequence(Slide *slide, Program *program);
static char *findObjAnimSteps(char *objIdentifier, AnimationDefinition *animationSeq);
static char *findObjAnimOrders(char *objIdentifier, AnimationDefinition *animationSeq);

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
    generatePrologue(compilerState);
    // TODO generate css classes for objects
    generateSlides(compilerState);
    generateEpilogue();
    fclose(_outputFile);
    _outputFile = NULL; //  step on pointer
    logDebugging(_logger, "Generation is done.");
}

/* Private func definitions */
void generateSlide(Slide *slide, AnimationDefinition *sequence, SymbolTable *symbolTable) {
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
                    // TODO do all in same function and return a struct
                    char *animations = findObjAnimSteps(obj->identifier, sequence);
                    char *orders = findObjAnimOrders(obj->identifier, sequence);

                    generateItem(getSymbol(symbolTable, obj->identifier), obj->identifier,
                                 animations, orders);
                    if (animations) {
                        free(animations);
                    }
                    if (orders) {
                        free(orders);
                    }
                } else {
                    logWarning(_logger, "Column %d not found in row %d of slide %s", j, i,
                               slide->identifier);
                }
            }
            fprintf(_outputFile, "</div>\n");
        }
    }
}

static void generateSlides(CompilerState *compilerState) {
    Program *program = compilerState->abstractSyntaxtTree;
    logDebugging(_logger, "Printing each slide in the presentation...");
    if (_outputFile == NULL) {
        logError(_logger, "Cannot open output file for writing");
        return;
    }
    // iterate through the slides and print them (to file)
    int i = 0;
    for (Slide *slide = compilerState->slides->head; slide != NULL; slide = slide->next, i++) {
        AnimationType anim = findAnimationTransition(slide, program);
        AnimationDefinition *animSeq = findSlideAnimationSequence(slide, program);
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
        logDebugging(_logger, "Slide %s with animation type %s %d", slide->identifier, animS, anim);

        if (i != 0) {
            fprintf(_outputFile, "<div class='slide %s' %s data-repeats='1'>\n", slide->identifier,
                    animS);

        } else {
            fprintf(_outputFile, "<div class='slide %s active' %s data-repeats='1'>\n",
                    slide->identifier, animS);
        }
        generateSlide(slide, animSeq, compilerState->symbolTable);
        fprintf(_outputFile, "</div>\n");
    }
    if (!success) {
        logCritical(_logger, "There were errors during the generation process.");
    }
    logDebugging(_logger, "Finished printing each slide.");
}

static void generatePrologue(CompilerState *CompilerState) {
    Program *program = CompilerState->abstractSyntaxtTree;
    fprintf(_outputFile, "<!DOCTYPE html>\n");
    fprintf(_outputFile, "<html lang='en'>\n");
    fprintf(_outputFile, "<head>\n");
    fprintf(_outputFile, "<meta charset='UTF-8'>\n");
    fprintf(_outputFile,
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    fprintf(_outputFile, "<title>%s</title>\n", program->presentation_identifier);
    fprintf(_outputFile, "<link rel='stylesheet' href='../src/main/web/css/styles.css'>\n");
    fprintf(_outputFile, "<style>\n");
    outputProperties(CompilerState);
    fprintf(_outputFile, "</style>\n");
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
            (strcasecmp(animations->pair.identifier1, slide->identifier) == 0)) {
            return animations->pair.type;
        }
    }
    return ANIM_NO_ANIM;
}

/* This gets the FIRST animation sequence found for that slide.
** But because the list is constructed backwards, that is the LAST sequence declared for that slide
** therefore only the last sequence is taken into account (the rest are ignored)
*/
static AnimationDefinition *findSlideAnimationSequence(Slide *slide, Program *program) {
    for (AnimationDefinition *animations = program->animation_definitions; animations != NULL;
         animations = animations->next) {
        if (animations->kind == ANIM_DEF_SEQUENCE &&
            (strcasecmp(animations->sequence.identifier, slide->identifier) == 0)) {
            logDebugging(_logger, "Animation found for slide %s, it repeats %d times",
                         slide->identifier, animations->sequence.repeat_count);
            return animations;
        }
    }
    return NULL;
}

// Given a sequence, for each step where the object appears as identifier, concat to a string the
// animation type (case appear, case dissapear and case rotate)
// builds dinamically -> must free after

static char *findObjAnimSteps(char *objIdentifier, AnimationDefinition *animationSeq) {
    if (!animationSeq || animationSeq->kind != ANIM_DEF_SEQUENCE || !objIdentifier) {
        return NULL;
    }
    char *result = malloc(1);
    if (!result) {
        logError(_logger, "Memory allocation failed");
        success = FALSE;
        return NULL;
    }
    result[0] = '\0';
    int currentSize = 1;
    AnimationStep *step = animationSeq->sequence.steps;
    int first = 1;

    while (step != NULL) {
        if (strcasecmp(step->identifier, objIdentifier) == 0) {
            const char *animType = NULL;
            int animTypeLen = 0;
            switch (step->type) {
            case ANIM_APPEAR:
                animType = "appear";
                animTypeLen = 6;
                break;
            case ANIM_DISAPPEAR:
                animType = "disappear";
                animTypeLen = 9;
                break;
            case ANIM_ROTATE:
                animType = "rotate";
                animTypeLen = 6;
                break;
            default:
                break;
            }

            if (animType) {
                int spaceNeeded = animTypeLen + (first ? 0 : 1);
                int newSize = currentSize + spaceNeeded;

                char *newResult = realloc(result, newSize);
                if (!newResult) {
                    logError(_logger, "Memory reallocation failed ");
                    free(result);
                    return NULL;
                }
                result = newResult;
                if (!first) {
                    strcat(result, " ");
                }
                strcat(result, animType);
                currentSize = newSize;
                first = 0;
            }
        }
        step = step->next;
    }
    if (currentSize == 1) {
        free(result);
        return NULL;
    }
    return result;
}

static char *findObjAnimOrders(char *objIdentifier, AnimationDefinition *animationSeq) {
    if (!animationSeq || animationSeq->kind != ANIM_DEF_SEQUENCE || !objIdentifier) {
        return NULL;
    }
    char *result = malloc(1);
    if (!result) {
        logError(_logger, "Memory allocation failed");
        success = FAILED;
        return NULL;
    }
    result[0] = '\0';
    int currentSize = 1;
    AnimationStep *step = animationSeq->sequence.steps;
    int first = 1;
    int stepNumber = 1;

    while (step != NULL) {
        if (strcasecmp(step->identifier, objIdentifier) == 0) {
            char stepStr[16]; // up to 999999 is enough...
            snprintf(stepStr, sizeof(stepStr), "%d", stepNumber);
            int stepStrLen = strlen(stepStr);

            int spaceNeeded = stepStrLen + (first ? 0 : 2); // +2 for comma and space if not first
            int newSize = currentSize + spaceNeeded;

            char *newResult = realloc(result, newSize);
            if (!newResult) {
                logError(_logger, "Memory reallocation failed");
                free(result);
                success = FALSE;
                return NULL;
            }
            result = newResult;
            if (!first) {
                strcat(result, ", ");
            }
            strcat(result, stepStr);
            currentSize = newSize;
            first = 0;
        }
        step = step->next;
        stepNumber++;
    }
    if (currentSize == 1) {
        free(result);
        return NULL;
    }
    return result;
}

/* In many cases we use switches because if/else does not scale and its quite possible other object
 * types will be included*/
static void generateItem(SymbolTableItem *object, char *identifier, char *animations,
                         char *orders) {
    switch (object->type) {
    case OBJ_IMAGE:
        if (animations && orders) {
            fprintf(
                _outputFile,
                "<img src='%s' class='slide-image %s' data-animation='%s' data-anim-order='%s'>",
                object->string, identifier, animations, orders);
        } else {
            fprintf(_outputFile, "<img src='%s' class='slide-image %s'>", object->string,
                    identifier);
        }
        break;

    case OBJ_TEXTBLOCK:
        if (animations && orders) {
            fprintf(_outputFile,
                    "<div class='%s' data-animation='%s' data-anim-order='%s'>%s</div>", identifier,
                    animations, orders, (object->string == NULL) ? "" : object->string);
        } else {
            fprintf(_outputFile, "<div class='%s'>%s</div>", identifier,
                    (object->string == NULL) ? "" : object->string);
        }
        break;
    default:
        break;
    }
}

static void outputProperties(CompilerState *compilerState) {
    SymbolTable *SymbolTable = compilerState->symbolTable;

    if (SymbolTable == NULL || SymbolTable->table == NULL) {
        return;
    }
    // iterate through all symbols
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, SymbolTable->table);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *identifier = (char *)key;
        SymbolTableItem *item = (SymbolTableItem *)value;

        // only output for those with properties defined
        if (item == NULL || item->properties == NULL) {
            logDebugging(_logger, "Skipping item %s - item is NULL or has no properties",
                         identifier);
            continue;
        }

        char *cssProperties = NULL;
        logDebugging(
            _logger,
            "Processing item %s with type %d (OBJ_SLIDE=%d, OBJ_TEXTBLOCK=%d, OBJ_IMAGE=%d)",
            identifier, item->type, OBJ_SLIDE, OBJ_TEXTBLOCK, OBJ_IMAGE);

        switch (item->type) {
        case OBJ_IMAGE:
            logDebugging(_logger, "Calling parseImageProperties for %s", identifier);
            cssProperties = parseImageProperties(item->properties);
            break;
        case OBJ_TEXTBLOCK:
            logDebugging(_logger, "Calling parseTextblockProperties for %s", identifier);
            cssProperties = parseTextblockProperties(item->properties);
            break;
        case OBJ_SLIDE:
            logDebugging(_logger, "Calling parseSlideProperties for %s", identifier);
            cssProperties = parseSlideProperties(item->properties);
            break;
        default:
            logDebugging(_logger, "Unknown object type %d for %s", item->type, identifier);
            continue;
        }

        // dont output if there were no valid props
        if (cssProperties != NULL && strlen(cssProperties) > 0) {
            fprintf(_outputFile, ".%s {\n%s }\n", identifier, cssProperties);
            free(cssProperties);
        }
    }
}