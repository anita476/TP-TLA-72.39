#include "Properties.h"

typedef enum {
    IMG_BORDER_SIZE,
    IMG_BORDER_COLOR,
    IMG_MAX_HEIGHT,
    IMG_MAX_WIDTH,
    IMG_BORDER_RADIUS,
    IMG_BORDER_STYLE,
    IMG_UNKNOWN
} ImageProperty;
ImageProperty getImagePropertyKey(const char *name);

typedef enum {
    TXT_BACKGROUND_COLOR,
    TXT_FONT_SIZE,
    TXT_FONT_COLOR,
    TXT_FONT_FAMILY,
    TXT_UNKNOWN
} TextblockProperty;
TextblockProperty getTextblockPropertyKey(const char *name);

typedef enum { SLD_BACKGROUND_COLOR, SLD_FONT_FAMILY, SLD_UNKNOWN } SlideProperty;
SlideProperty getSlidePropertyKey(const char *name);

char *parseImageProperties(CssProperty *properties) {
    if (properties == NULL) {
        return "";
    }

    char *result = malloc(1);
    if (!result) {
        return "";
    }
    result[0] = '\0';
    int currentSize = 1;

    for (CssProperty *props = properties; props != NULL;) {
        if (props == NULL) {
            break;
        }

        if (props->property_name == NULL) {
            CssProperty *next = props->next;
            props = next;
            continue;
        }
        char *value = NULL;

        // value based on the property type
        switch (props->value_type) {
        case PROP_VAL_IDENTIFIER:
            if (props->value.identifier != NULL) {
                value = props->value.identifier;
            } else {
                value = "";
            }
            break;
        case PROP_VAL_INTEGER:
            value = malloc(32);
            if (value) {
                snprintf(value, 32, "%d", props->value.integer);
            }
            break;
        case PROP_VAL_DECIMAL:
            value = malloc(32);
            if (value) {
                snprintf(value, 32, "%.2f", props->value.decimal);
            }
            break;
        default:
            value = "";
            break;
        }

        if (!value) {
            value = "";
        }
        if (props->property_name == NULL) {
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        ImageProperty type = getImagePropertyKey(props->property_name);
        if (type == IMG_UNKNOWN) {
            if (props->value_type == PROP_VAL_INTEGER ||
                props->value_type == PROP_VAL_DECIMAL) {
                free(value);
            }
            CssProperty *next = props->next;
            props = next;
            continue;
        }
        int propNameLen = strlen(props->property_name);
        int valueLen = strlen(value);
        int requiredSize = propNameLen + valueLen + 10;

        char *cssProperty = malloc(requiredSize);
        if (cssProperty) {
            snprintf(cssProperty, requiredSize, "%s: %s;\n", props->property_name, value);

            int newSize = currentSize + strlen(cssProperty);
            char *newResult = realloc(result, newSize);
            if (newResult) {
                result = newResult;
                strcat(result, cssProperty);
                currentSize = newSize;
            }
            free(cssProperty);
        }
        if (props->value_type == PROP_VAL_INTEGER || props->value_type == PROP_VAL_DECIMAL) {
            free(value);
        }
        CssProperty *next = props->next;
        props = next;
    }

    return result;
}
char *parseTextblockProperties(CssProperty *properties) {
    if (properties == NULL) {
        return "";
    }

    char *result = malloc(1);
    if (!result) {
        return "";
    }
    result[0] = '\0';
    int currentSize = 1;

    for (CssProperty *props = properties; props != NULL;) {
        if (props == NULL) {
            break;
        }

        if (props->property_name == NULL) {
            // Safely advance to next
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        char *value = NULL;

        switch (props->value_type) {
        case PROP_VAL_IDENTIFIER:
            if (props->value.identifier != NULL) {
                value = props->value.identifier;
            } else {
                value = "";
            }
            break;
        case PROP_VAL_INTEGER:
            value = malloc(32);
            if (value) {
                snprintf(value, 32, "%d", props->value.integer);
            }
            break;
        case PROP_VAL_DECIMAL:
            value = malloc(32);
            if (value) {
                snprintf(value, 32, "%.2f", props->value.decimal);
            }
            break;
        default:
            value = "";
            break;
        }

        if (!value) {
            value = "";
        }
        if (props->property_name == NULL) {
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        TextblockProperty type = getTextblockPropertyKey(props->property_name);
        if (type == TXT_UNKNOWN) {
            if (props->value_type == PROP_VAL_INTEGER ||
                props->value_type == PROP_VAL_DECIMAL) {
                free(value);
            }
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        int propNameLen = strlen(props->property_name);
        int valueLen = strlen(value);
        int requiredSize = propNameLen + valueLen + 10; // ": ;\n" + safety margin

        char *cssProperty = malloc(requiredSize);
        if (cssProperty) {
            snprintf(cssProperty, requiredSize, "%s: %s;\n", props->property_name, value);
            int newSize = currentSize + strlen(cssProperty);
            char *newResult = realloc(result, newSize);
            if (newResult) {
                result = newResult;
                strcat(result, cssProperty);
                currentSize = newSize;
            }
            free(cssProperty);
        }

        if (props->value_type == PROP_VAL_INTEGER || props->value_type == PROP_VAL_DECIMAL) {
            free(value);
        }
        CssProperty *next = props->next;
        props = next;
    }

    return result;
}
char *parseSlideProperties(CssProperty *properties) {

    if (properties == NULL) {
        return "";
    }

    char *result = malloc(1);
    if (!result) {
        return "";
    }
    result[0] = '\0';
    int currentSize = 1;

    for (CssProperty *props = properties; props != NULL;) {
        // validate current property node
        if (props == NULL) {
            break;
        }

        if (props->property_name == NULL) {
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        char *value = NULL;

        switch (props->value_type) {
        case PROP_VAL_IDENTIFIER:
            if (props->value.identifier != NULL) {
                value = props->value.identifier;
            } else {
                value = "";
            }
            break;
        case PROP_VAL_INTEGER:
            value = malloc(32);
            if (value) {
                snprintf(value, 32, "%d", props->value.integer);
            }
            break;
        case PROP_VAL_DECIMAL:
            value = malloc(32);
            if (value) {
                snprintf(value, 32, "%.2f", props->value.decimal);
            }
            break;
        default:
            value = "";
            break;
        }

        if (!value) {
            value = "";
        }

        if (props->property_name == NULL) {
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        SlideProperty type = getSlidePropertyKey(props->property_name);
        if (type == SLD_UNKNOWN) {
            if (props->value_type == PROP_VAL_INTEGER ||
                props->value_type == PROP_VAL_DECIMAL) {
                free(value);
            }
            CssProperty *next = props->next;
            props = next;
            continue;
        }

        int propNameLen = strlen(props->property_name);
        int valueLen = strlen(value);
        int requiredSize = propNameLen + valueLen + 10;

        char *cssProperty = malloc(requiredSize);
        if (cssProperty) {
            snprintf(cssProperty, requiredSize, "%s: %s;\n", props->property_name, value);

            int newSize = currentSize + strlen(cssProperty);
            char *newResult = realloc(result, newSize);
            if (newResult) {
                result = newResult;
                strcat(result, cssProperty);
                currentSize = newSize;
            }
            free(cssProperty);
        }

        if (props->value_type == PROP_VAL_INTEGER || props->value_type == PROP_VAL_DECIMAL) {
            free(value);
        }

        CssProperty *next = props->next;
        props = next;
    }

    return result;
}

/*******************************Helpers****************************************/

ImageProperty getImagePropertyKey(const char *name) {
    if (name == NULL) {
        return IMG_UNKNOWN;
    }

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
    if (strcmp(name, "border-style") == 0) {
        return IMG_BORDER_STYLE;
    }
    return IMG_UNKNOWN;
}

TextblockProperty getTextblockPropertyKey(const char *name) {
    if (name == NULL) {
        return TXT_UNKNOWN;
    }

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
    return TXT_UNKNOWN;
}
SlideProperty getSlidePropertyKey(const char *name) {
    if (name == NULL) {
        return SLD_UNKNOWN;
    }

    if (strcmp(name, "background-color") == 0) {
        return SLD_BACKGROUND_COLOR;
    }
    if (strcmp(name, "font-family") == 0) {
        return SLD_FONT_FAMILY;
    }
    return SLD_UNKNOWN;
}