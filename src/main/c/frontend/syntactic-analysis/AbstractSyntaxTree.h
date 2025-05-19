#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();



/**
 * Node types for the Abstract Syntax Tree (AST).
 */

// Enum for object types (slide, textblock, image)
typedef enum {
    OBJ_SLIDE,
    OBJ_TEXTBLOCK,
    OBJ_IMAGE,
} ObjectType;

// Property value types 
typedef enum {
    PROP_VAL_IDENTIFIER,
    PROP_VAL_INTEGER,
    PROP_VAL_DECIMAL
} PropertyValueType;

// Struct for a CSS property: property name and value 
typedef struct CssProperty {
    char *property_name;     // PROPERTY token text
    PropertyValueType value_type;
    union value{
        char *identifier;
        int integer;
        float decimal;
    } value;
    struct CssProperty *next;  
} CssProperty;

// Struct for object definition (slide, textblock, image)
// could make a container for it to put the size i guess
typedef struct ObjectDefinition {
    ObjectType type;
    char *identifier;             // name of the object
    CssProperty *css_properties; // linked list of properties
    struct ObjectDefinition *next;
} ObjectDefinition;

// Enum for all positions (centralize them all for simplicity, but in each case we accept only a subset)
typedef enum {
    POS_TOP_LEFT,
    POS_TOP_RIGHT,
    POS_BOTTOM_LEFT,
    POS_BOTTOM_RIGHT,
    POS_CENTER_LEFT,
    POS_CENTER_RIGHT,
    POS_CENTER,
    POS_CENTER_TOP,
    POS_CENTER_BOTTOM,
	POS_TOP,
	POS_BOTTOM,
	POS_LEFT,
	POS_RIGHT
} Position;

// Slide content types 
typedef enum {
    SLIDE_CONTENT_ADD,
    SLIDE_CONTENT_ADD_WITH_STRING,
    SLIDE_CONTENT_ANCHOR,
    SLIDE_CONTENT_SIMPLE_POS,
    SLIDE_CONTENT_DOUBLE_POS,
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
			char * child;
			Position pos;
			char * parent;
		} position_items;
        struct {
            char *identifier;
            Position pos;
        } position_item;
    };
    struct SlideContent *next;  // linked list of slide content items
} SlideContent;

// Struct for structure sentence
typedef struct StructureDefinition {
    char *identifier;      
    SlideContent *content; // linked list of slide contents
    struct StructureDefinition *next;
} StructureDefinition;

typedef enum {
    ANIM_APPEAR,
    ANIM_DISSAPPEAR,
    ANIM_ROTATE,
    ANIM_FADE_INTO,
    ANIM_JUMP_INTO
} AnimationType;

typedef enum {
    ANIM_DEF_SINGLE,       // to animate a single object
    ANIM_DEF_SEQUENCE,     // START ... END control structure or with repeat
    ANIM_DEF_PAIR,         // to animate slide transitions
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
            char * identifier; // identifier of SLIDE (validate identifier types later in backend)
            AnimationStep *steps; // linked list of steps
            int repeat_count;     // 1 if no repeat, otherwise repeat times
        } sequence;
        struct {
            char * identifier1;
            char * identifier2;
            AnimationType type; 
        } pair;
    };
    struct AnimationDefinition *next;
} AnimationDefinition;



/************************************* OUR PROGRAM *********************************************** */
typedef struct {
    char *presentation_identifier; 

    ObjectDefinition *object_definitions;   
    StructureDefinition *structure_definitions; 
    AnimationDefinition *animation_definitions; 

} Program;


/**
 * Node recursive destructors.
 */
void destroyProgram(Program * program);

void releaseAnimationsSection(AnimationDefinition * animationList);
void releaseObjectsSection(ObjectDefinition * objectList);
void releaseStructureSection(StructureDefinition * structureList);

#endif
