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

typedef int Token;

#endif
