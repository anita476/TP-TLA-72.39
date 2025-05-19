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

/**  PRIVATE FUNCTIONS **/
void releaseAnimationSteps(AnimationStep * animationSteps);
void releaseCssProperties(CssProperty * propertyList);
void releaseSlideContentList(SlideContent * slideContentList);
void releaseSlideContent(SlideContent * slideContent);


/** PUBLIC FUNCTIONS */
void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		releaseObjectsSection(program->object_definitions);
		releaseStructureSection(program->structure_definitions);
		releaseAnimationsSection(program->animation_definitions);
		free(program->presentation_identifier);
		free(program);
	}
}

void releaseAnimationsSection(AnimationDefinition * animationList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (animationList != NULL) {
		switch (animationList->kind) {
			case ANIM_DEF_SINGLE:
				free(animationList->single.identifier);
				break;
			case ANIM_DEF_SEQUENCE:
				free(animationList->sequence.identifier);
				releaseAnimationSteps(animationList->sequence.steps);
				break;
			case ANIM_DEF_PAIR:
				free(animationList->pair.identifier1);
				free(animationList->pair.identifier2);
				break;
		}
		releaseAnimationsSection(animationList->next);
		free(animationList);
	}
}

void releaseAnimationSteps(AnimationStep * animationSteps) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (animationSteps != NULL) {
		free(animationSteps->identifier);
		releaseAnimationSteps(animationSteps->next);
		free(animationSteps);
	}
}

void releaseObjectsSection(ObjectDefinition * objectList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (objectList != NULL) {
		free(objectList->identifier);
		releaseCssProperties(objectList->css_properties);
		releaseObjectsSection(objectList->next);
		free(objectList);
	}
}

void releaseCssProperties(CssProperty * propertyList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (propertyList != NULL) {
		switch (propertyList->value_type) {
			case PROP_VAL_IDENTIFIER:
				logDebugging(_logger, "Freeing identifier property");
				free(propertyList->value.identifier);
				break;
			case PROP_VAL_INTEGER:
				logDebugging(_logger, "Freeing integer property");
				break;
		}
		free(propertyList->property_name);
		releaseCssProperties(propertyList->next);
		free(propertyList);
	}
}

void releaseStructureSection(StructureDefinition * structureList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (structureList != NULL) {
		releaseSlideContent(structureList->content);
		free(structureList->identifier);
		releaseStructureSection(structureList->next);
		free(structureList);
	}
}

void releaseSlideContentList(SlideContent * slideContentList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (slideContentList != NULL) {
		releaseSlideContent(slideContentList->next);
		free(slideContentList);
	}
}

void releaseSlideContent(SlideContent * slideContent) {
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
