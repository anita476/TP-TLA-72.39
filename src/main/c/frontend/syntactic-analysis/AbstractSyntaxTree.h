#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Program Program;
/**
 * Node types for the Abstract Syntax Tree (AST).
 */

// Enum for object types (slide, textblock, image)
typedef enum {
    OBJ_SLIDE,
    OBJ_TEXTBLOCK,
    OBJ_IMAGE,
} ObjectType;

// Enum for property value types
typedef enum {
    PROP_VAL_IDENTIFIER,
    PROP_VAL_INTEGER,
} PropertyValueType;

// Struct for a CSS property: property name and value (identifier or integer)
typedef struct CssProperty {
    char *property_name;     // PROPERTY token text
    PropertyValueType value_type;
    union {
        char *identifier;
        int integer;
    } value;
    struct CssProperty *next;  // linked list
} CssProperty;

// Struct for object definition (slide, textblock, image)
typedef struct ObjectDefinition {
    ObjectType type;
    char *identifier;             // name of the object
    CssProperty *css_properties; // linked list of properties
    struct ObjectDefinition *next;
} ObjectDefinition;

// Enum for all positions (centralize them all for simplicity, but in each case we accept only a subset)
typedef enum {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CENTER_LEFT,
    CENTER_RIGHT,
    CENTER,
    CENTER_TOP,
    CENTER_BOTTOM,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
} Position;

// Enum to discriminate slide_content variants
typedef enum {
    SLIDE_CONTENT_EMPTY,
    SLIDE_CONTENT_ADD,
    SLIDE_CONTENT_ADD_WITH_STRING,
    SLIDE_CONTENT_ANCHOR,
    SLIDE_CONTENT_SIMPLE_POS,
    SLIDE_CONTENT_COMPOUND_POS,
} SlideContentType;

// Struct for slide content
typedef struct SlideContent {
    SlideContentType type;
    union {
        struct {
            char *identifier;
            char *with_string;   // NULL if none
        } add;

        Position anchor;

        struct {
            char *identifier;
            Position pos;
        } simple_pos;

        struct {
            char *identifier;
            Position pos;
        } compound_pos;
    };
    struct SlideContent *next;  // linked list of slide content items
} SlideContent;

// Struct for structure sentence
typedef struct StructureSentence {
    char *identifier;      
    SlideContent *content; // linked list of slide contents
    struct StructureSentence *next;
} StructureSentence;

typedef enum {
    ANIM_APPEAR,
    ANIM_DISSAPPEAR,
    ANIM_ROTATE,
} AnimationType;

typedef enum {
    ANIM_DEF_SINGLE,       // to animate a single object
    ANIM_DEF_SEQUENCE,     // START ... END control structure or with repeat
} AnimationDefinitionKind;


typedef struct AnimationStep AnimationStep;

// Struct for animation step: THEN IDENTIFIER animation_type
struct AnimationStep {
    char *identifier;
    AnimationType type;
    AnimationStep *next;
};

typedef struct AnimationDefinition {
    AnimationDefinitionKind kind;
    union {
        struct {
            char *identifier;
            AnimationType type;
        } single;
        struct {
            AnimationStep *steps; // linked list of steps
            int repeat_count;     // 1 if no repeat, otherwise repeat times
        } sequence;
    };
    struct AnimationDefinition *next;
} AnimationDefinition;



/************************************* MY PROGRAM *********************************************** */
typedef struct {
    char *presentation_identifier; // Presentation name

    ObjectDefinition *object_definitions;   
    StructureSentence *structure_sentences; 
    AnimationDefinition *animation_definitions; 
} Program;

/**
 * Node recursive destructors.
 */
void releaseConstant(Constant * constant);
void releaseExpression(Expression * expression);
void releaseFactor(Factor * factor);
void releaseProgram(Program * program);

#endif
