#include "Properties.h"

typedef enum {
    IMG_BORDER_SIZE,
    IMG_BORDER_COLOR,
    IMG_MAX_HEIGHT,
    IMG_MAX_WIDTH,
    IMG_BORDER_RADIUS,
    IMG_UNKNOWN
} ImageProperty;
ImageProperty getImagePropertyKey(const char *name);

typedef enum {
    TXT_BACKGROUND_COLOR,
    TXT_FONT_SIZE,
    TXT_FONT_COLOR,
    TXT_FONT_FAMILY,

} TextblockProperty;
TextblockProperty getTextblockPropertyKey(const char *name);

typedef enum { SLD_BACKGROUND_COLOR, SLD_FONT_FAMILY } SlideProperty;
SlideProperty getSlidePropertyKey(const char *name);

char *parseImageProperties(CssProperty *properties) {
    for (CssProperty *props = properties; props != NULL; props = props->next) {
    }

    return "";
}
char *parseTextblockProperties(CssProperty *properties) { return ""; }
char *parseSlideProperties(CssProperty *properties) { return ""; }

/*******************************Helpers****************************************/

ImageProperty getImagePropertyKey(const char *name) {
    if (strcmp(name, "border-radius") == 0) {
        return IMG_BORDER_RADIUS;
    }
    if (strcmp(name, "border-width") == 0) {
        return IMG_BORDER_SIZE;
    }
    if (strcmp(name, "border-color") == 0) {
        return IMG_BORDER_COLOR;
    }
    if (strcmp(name, "max-width") == 0) {
        return IMG_MAX_WIDTH;
    }
    if (strcmp(name, "max-height") == 0) {
        return IMG_MAX_HEIGHT;
    }
}

TextblockProperty getTextblockPropertyKey(const char *name) {
    if (strcmp(name, "background-color") == 0) {
        return TXT_BACKGROUND_COLOR;
    }
    if (strcmp(name, "font-family") == 0) {
        return TXT_FONT_FAMILY;
    }
    if (strcmp(name, "color") == 0) {
        return TXT_FONT_COLOR;
    }
    if (strcmp(name, "font-size") == 0) {
        return TXT_FONT_SIZE;
    }
}
SlideProperty getSlidePropertyKey(const char *name) {
    if (strcmp(name, "background-color") == 0) {
        return SLD_BACKGROUND_COLOR;
    }
    if (strcmp(name, "font-family") == 0) {
        return SLD_FONT_FAMILY;
    }
}