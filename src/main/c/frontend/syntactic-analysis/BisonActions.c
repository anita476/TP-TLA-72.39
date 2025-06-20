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
	else if(compilerState-> errorCount > 0) {
		logError(_logger, "There are %d semantic errors in the syntactic analysis phase.", compilerState->errorCount);
		compilerState->succeed = false;

	}
	else {
		logDebugging(_logger, "Syntactic analysis phase is successful.");
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

ObjectDefinition * ObjectDefinitionSemanticAction(CompilerState * CompilerState, ObjectType type, char * identifier, CssProperty * cssProperties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	/* SEMANTICS CHECK */
	if(CompilerState->symbolTable != NULL && symbolExists(CompilerState->symbolTable, identifier)) {
		logError(_logger, "Duplicate object with identifier '%s' already exists.", identifier);
		CompilerState->errorCount++;
	}
	else {
		addSymbol(CompilerState->symbolTable, identifier, type);
	}

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

StructureDefinition * StructureDefinitionSemanticAction(CompilerState * CompilerState, char * identifier, SlideContent * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	/* SEMANTIC CHECK */
	if(!symbolExists(CompilerState->symbolTable, identifier)){
		logError(_logger, "Object with identifier '%s' does not exist.", identifier);
		CompilerState->errorCount++;
	}
	else if(getSymbolType(CompilerState->symbolTable, identifier) != OBJ_SLIDE) {
		logError(_logger, "Object with identifier '%s' is not a slide.", identifier);
		CompilerState->errorCount++;
	}
	/**
	 * Counter incremented AFTER the counter assignment in slide beecause the parser is ascending
	 * */
	SymbolTableItem * item = getSymbol(CompilerState->symbolTable, identifier);
	if(item->currentSlide == (-1)){
		item->currentSlide = CompilerState->slideCounter;
	}
	CompilerState->slideCounter++;
	

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

SlideContent * AdditionSlideContent(CompilerState * CompilerState, char * identifier, char * content) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	/* SEMANTIC CHECK */
	if(!symbolExists(CompilerState->symbolTable, identifier)) {
		logError(_logger, "Object with identifier '%s' does not exist.", identifier);
		CompilerState->errorCount++;
	}
	else{
		SymbolTableItem * item = getSymbol(CompilerState->symbolTable, identifier);
		if(item->currentSlide == (-1)){
			item->currentSlide = CompilerState->slideCounter;
			//item->appearsIn = g_array_append_val(item->appearsIn, CompilerState->slideCounter);
			/* Less efficient in space complexity but much better for time complexity in the long run  */
			g_array_insert_val(item->appearsIn, CompilerState->slideCounter, CompilerState->slideCounter);

		}
		else if(item->currentSlide == CompilerState->slideCounter) {
			logError(_logger, "Repeated object %s in a single slide", identifier);
			CompilerState->errorCount++;
		}
	}
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

AnimationDefinition * AnimationDefinitionSequenceSemanticAction(CompilerState * compilerState, char * identifier ,AnimationStep * steps, int repeat) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	/* SEMANTIC CHECKS */
	SymbolTableItem * slideItem = getSymbol(compilerState->symbolTable, identifier);
	if(slideItem == NULL || slideItem->type != OBJ_SLIDE){
		logError(_logger, "Object with identifier '%s' does not exist or is not a slide.", identifier);
		compilerState->errorCount++;	
	}

	if(steps != NULL && slideItem != NULL) {
		/* If item is not a slide or doesnt exist we cant check this because theres no slide to compare it to */
		for(AnimationStep * step = steps; step != NULL; step = step->next) {
			SymbolTableItem * stepItem = getSymbol(compilerState->symbolTable, step->identifier);

			gboolean error = FALSE;
			if (stepItem == NULL) {
				error = TRUE;
			} else {
				// Safely check if the object appears in the slide using your sparse array logic
				if (!(stepItem->appearsIn->len > slideItem->currentSlide && g_array_index(stepItem->appearsIn, int, slideItem->currentSlide) == slideItem->currentSlide)) {
					error = TRUE;
				}
			}
			if (error) {
				logError(_logger, "Object with identifier '%s' does not exist or is not part of slide %d.", step->identifier, slideItem->currentSlide);
				compilerState->errorCount++;
			}
		}
	}

	AnimationDefinition * animation = calloc(1, sizeof(AnimationDefinition));
	animation->kind = ANIM_DEF_SEQUENCE;
	animation->sequence.identifier = identifier;
	animation->sequence.steps = steps;
	animation->sequence.repeat_count = repeat;
	return animation;
}

AnimationDefinition * AnimationDefinitionPairSemanticAction(CompilerState * CompilerState, char * identifier1, char * identifier2, AnimationType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	/* SEMANTIC CHECKS */
	SymbolTableItem * slideItem1 = getSymbol(CompilerState->symbolTable, identifier1);
	SymbolTableItem * slideItem2 = getSymbol(CompilerState->symbolTable, identifier2);
	if(slideItem1 == NULL || slideItem1->type != OBJ_SLIDE) {
		logError(_logger, "Object with identifier '%s' does not exist or is not a slide.", identifier1);
		CompilerState->errorCount++;
	}
	if(slideItem2 == NULL || slideItem2->type != OBJ_SLIDE) {
		logError(_logger, "Object with identifier '%s' does not exist or is not a slide.", identifier2);
		CompilerState->errorCount++;
	}
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

AnimationStep * AnimationStepSemanticAction(CompilerState *CompilerState, char * identifier, AnimationType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	/* SEMANTIC  CHECKS */
	SymbolTableItem * item = getSymbol(CompilerState->symbolTable, identifier);
	if(item == NULL) {
		logError(_logger, "Object with identifier '%s' does not exist.", identifier);
		CompilerState->errorCount++;
	}
	else if(item->type == OBJ_SLIDE) {
		logError(_logger, "Object with identifier '%s' is a slide!!", identifier);
		CompilerState->errorCount++;
	}

	AnimationStep * step = calloc(1, sizeof(AnimationStep));
	step->identifier = identifier;
	step->type = type;
	step->next = NULL; 
	return step;
} 