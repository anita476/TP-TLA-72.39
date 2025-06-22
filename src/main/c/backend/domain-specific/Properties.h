/* Take a list of cssProperties and
** turn them into a string of formatted css properties
** based on the cases we accept for each object */

#ifndef PROPERTIES_HEADER
#define PROPERTIES_HEADER
#include "../../shared/Type.h"
#include <stdio.h>
/* Parse a series of css properties and  */

char *parseImageProperties(CssProperty *properties);
char *parseTextblockProperties(CssProperty *properties);
char *parseSlideProperties(CssProperty *properties);

#endif