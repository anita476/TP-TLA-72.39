#ifndef TYPE_HEADER
#define TYPE_HEADER
#include <glib.h>

typedef enum { false = 0, true = 1 } boolean;

typedef enum { OBJ_SLIDE, OBJ_TEXTBLOCK, OBJ_IMAGE, OBJ_UNKNOWN } ObjectType;

// Property value types
typedef enum { PROP_VAL_IDENTIFIER, PROP_VAL_INTEGER, PROP_VAL_DECIMAL } PropertyValueType;

// Struct for a CSS property: property name and value
typedef struct CssProperty {
    char *property_name; // PROPERTY token text
    PropertyValueType value_type;
    union value {
        char *identifier;
        int integer;
        float decimal;
    } value;
    struct CssProperty *next;
} CssProperty;

typedef struct PositionedObject {
    char *identifier;
    int row;
    int col;
} PositionedObject;

typedef struct Row {
    GHashTable *columns; // key: int* -> column index, value: PositionedObject*
    int minCol;
    int maxCol;
} Row;

typedef struct Slide {
    char *identifier; // To then gather the properties
    GHashTable *rows; // key: int* -> row index, value: Row*
    int minRow;
    int maxRow;
    int minCol;
    int maxCol;
    GHashTable *symbolToObject; // key: char* (identifier), value: PositionedObject*
    struct Slide *next;
} Slide;

typedef struct {
    Slide *head;
    Slide *tail;
} SlideList;

typedef int Token;

#endif
