#ifndef TYPE_HEADER
#define TYPE_HEADER

typedef enum {
	false = 0,
	true = 1
} boolean;

typedef enum {
    OBJ_SLIDE,
    OBJ_TEXTBLOCK,
    OBJ_IMAGE,
	OBJ_UNKNOWN
} ObjectType;


// Property value types 
typedef enum {
    PROP_VAL_IDENTIFIER,
    PROP_VAL_INTEGER,
    PROP_VAL_DECIMAL
} PropertyValueType;

// Struct for a CSS property: property name and value 
typedef struct CssProperty {
    char *property_name;                // PROPERTY token text
    PropertyValueType value_type;
    union value {
        char *identifier;
        int integer;
        float decimal;
    } value;
    struct CssProperty *next;  
} CssProperty;

typedef int Token;

#endif
