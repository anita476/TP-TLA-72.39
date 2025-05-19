#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */
void destroyProgram(Program * program){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseObjectsSection(program->object_definitions);
		releaseStructuresSection(program->structure_definitions);
		releaseAnimationsSection(program->animation_definitions);
		free(program->presentation_identifier);
		free(program);
	}
}

void releaseAnimationsSection(AnimationDefinition * animationList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (animationList != NULL) {
		switch (animationList->kind) {
			case ANIM_DEF_SINGLE:
				free(animationList->single.identifier);
				break;
			case ANIM_DEF_SEQUENCE:
				releaseAnimationSteps(animationList->sequence.steps);
				break;
		}
		releaseAnimationsSection(animationList->next);
		free(animationList);
	}
}

void releaseAnimationSteps(AnimationStep * animationSteps){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (animationSteps != NULL) {
		free(animationSteps->identifier);
		releaseAnimationSteps(animationSteps->next);
		free(animationSteps);
	}
}

void releaseObjectsSection(ObjectDefinition * objectList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (objectList != NULL) {
		free(objectList->identifier);
		releaseCssProperties(objectList->css_properties);
		releaseObjectsSection(objectList->next);
		free(objectList);
	}
}

void releaseCssProperties(CssProperty * propertyList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (propertyList != NULL) {
		switch (propertyList->value_type) {
			case PROP_VAL_IDENTIFIER:
				free(propertyList->value.identifier);
				break;
			default:
				break;
		}
		free(propertyList->property_name);
		releaseCssProperties(propertyList->next);
		free(propertyList);
	}
}

void releaseStructuresSection(StructureDefinition * structureList){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (structureList != NULL) {
		releaseSlideContent(structureList->content);
		free(structureList->identifier);
		releaseStructuresSection(structureList->next);
		free(structureList);
	}
}

void releaseSlideContent(SlideContent * slideContent){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (slideContent != NULL) {
		switch (slideContent->type) {
			case SLIDE_CONTENT_ADD:
				free(slideContent->add.identifier);
				free(slideContent->add.with_string);
				break;
			case SLIDE_CONTENT_ANCHOR:
				break;
			case SLIDE_CONTENT_SIMPLE_POS:
				free(slideContent->position_item.identifier);
				break;
			case SLIDE_CONTENT_DOUBLE_POS:
				free(slideContent->position_items.child);
				free(slideContent->position_items.parent);
				break;
		}
		releaseSlideContent(slideContent->next);
		free(slideContent);
	}
}





/*
void releaseConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void releaseExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				releaseExpression(expression->leftExpression);
				releaseExpression(expression->rightExpression);
				break;
			case FACTOR:
				releaseFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void releaseFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				releaseConstant(factor->constant);
				break;
			case EXPRESSION:
				releaseExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseExpression(program->expression);
		free(program);
	}
}
*/
