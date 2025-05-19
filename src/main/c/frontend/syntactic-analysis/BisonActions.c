#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}


/* PUBLIC FUNCTIONS */
Program * ProgramSemanticAction(CompilerState * compilerState, char * presName, ObjectDefinition * objectList, StructureDefinition * structureList, AnimationDefinition * animationList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->object_definitions = objectList;
	program->structure_definitions = structureList;
	program->animation_definitions = animationList;
	program->presentation_identifier = presName;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}

ObjectDefinition * ObjectListSemanticAction(ObjectDefinition * objectList, ObjectDefinition * newObject) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (objectList != NULL) {
		newObject->next = objectList;
	}
	return newObject;
} 

ObjectDefinition * ObjectDefinitionSemanticAction(ObjectType type, char * identifier, CssProperty * cssProperties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ObjectDefinition * object = calloc(1, sizeof(ObjectDefinition));
	object->type = type;
	object->identifier = identifier;
	object->css_properties = cssProperties;
	object->next = NULL; 
	return object;
} 

CssProperty * PropertyListSemanticAction(CssProperty * propertyList, CssProperty * newProperty) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (propertyList != NULL) {
		newProperty->next = propertyList;
	}
	return newProperty;
} 	

CssProperty * PropertySemanticAction(char * propertyName, char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CssProperty * property = calloc(1, sizeof(CssProperty));
	property->property_name = propertyName;
	property->value.identifier = value;
	property->next = NULL; 
	return property;
} 

CssProperty * PropertyNumberSemanticAction(char * propertyName, int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CssProperty * property = calloc(1, sizeof(CssProperty));
	property->value_type = PROP_VAL_INTEGER;
	property->property_name = propertyName;
	property->value.integer = value;
	property->next = NULL; 
	return property;
} 
CssProperty * PropertyDecimalSemanticAction(char * propertyName, float value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CssProperty * property = calloc(1, sizeof(CssProperty));
	property->value_type = PROP_VAL_DECIMAL;
	property->property_name = propertyName;
	property->value.decimal = value;
	property->next = NULL; 
	return property;
}
StructureDefinition * StructureListSemanticAction(StructureDefinition * structureList, StructureDefinition * newStructure) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (structureList != NULL) {
		newStructure->next = structureList;
	}
	return newStructure;
}						

StructureDefinition * StructureDefinitionSemanticAction(char * identifier, SlideContent * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StructureDefinition * structure = calloc(1, sizeof(StructureDefinition));
	structure->identifier = identifier;
	structure->content = content;
	structure->next = NULL; 
	return structure;
}

SlideContent * SlideContentListSemanticAction(SlideContent * slideContentList, SlideContent * newSlideContent) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (slideContentList != NULL) {
		newSlideContent->next = slideContentList;
	}
	return newSlideContent;
}

SlideContent * AdditionSlideContent(char * identifier, char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideContent * slideContent = calloc(1, sizeof(SlideContent));
	slideContent->type = SLIDE_CONTENT_ADD;
	slideContent->add.identifier = identifier;
	slideContent->add.with_string = content;
	slideContent->next = NULL; 
	return slideContent;
}

SlideContent * AnchorPositionSlideContent(Position position) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideContent * slideContent = calloc(1, sizeof(SlideContent));
	slideContent->type = SLIDE_CONTENT_ANCHOR;
	slideContent->anchor = position;
	slideContent->next = NULL; 
	return slideContent;
}

SlideContent * RelativeSimplePositionSlideContent(char * relative , Position position) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideContent * slideContent = calloc(1, sizeof(SlideContent));
	slideContent->type = SLIDE_CONTENT_SIMPLE_POS;
	slideContent->position_item.identifier = relative;
	slideContent->position_item.pos = position;
	slideContent->next = NULL; 
	return slideContent;
}

SlideContent * RelativeDoublePositionSlideContent(char * relative, char * fixed, Position position) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SlideContent * slideContent = calloc(1, sizeof(SlideContent));
	slideContent->type = SLIDE_CONTENT_DOUBLE_POS;
	slideContent->position_items.child = relative;
	slideContent->position_items.pos = position;
	slideContent->position_items.parent = fixed;
	slideContent->next = NULL; 
	return slideContent;
} 

AnimationDefinition * AnimationListSemanticAction(AnimationDefinition * animationList, AnimationDefinition * newAnimation) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (animationList != NULL) {
		newAnimation->next = animationList;
	}
	return newAnimation;
} 

AnimationDefinition * AnimationDefinitionSemanticAction(char * identifier, AnimationType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AnimationDefinition * animation = calloc(1, sizeof(AnimationDefinition));
	animation->kind = ANIM_DEF_SINGLE;
	animation->single.identifier = identifier;
	animation->single.type = type;
	return animation;
}

AnimationDefinition * AnimationDefinitionSequenceSemanticAction(char * identifier ,AnimationStep * steps, int repeat) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AnimationDefinition * animation = calloc(1, sizeof(AnimationDefinition));
	animation->kind = ANIM_DEF_SEQUENCE;
	animation->sequence.identifier = identifier;
	animation->sequence.steps = steps;
	animation->sequence.repeat_count = repeat;
	return animation;
}

AnimationDefinition * AnimationDefinitionPairSemanticAction(char * identifier1, char * identifier2, AnimationType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AnimationDefinition * animation = calloc(1, sizeof(AnimationDefinition));
	animation->kind = ANIM_DEF_PAIR;
	animation->pair.identifier1 = identifier1;
	animation->pair.identifier2 = identifier2;
	animation->pair.type = type;
	return animation;
}

AnimationStep * AnimationSequenceSemanticAction(AnimationStep * steps, AnimationStep * new) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (steps != NULL) {
		new->next = steps;
	}
	return new;
}

AnimationStep * AnimationStepSemanticAction(char* identifier, AnimationType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AnimationStep * step = calloc(1, sizeof(AnimationStep));
	step->identifier = identifier;
	step->type = type;
	step->next = NULL; 
	return step;
} 	