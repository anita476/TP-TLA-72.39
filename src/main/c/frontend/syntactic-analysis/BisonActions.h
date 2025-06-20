#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/* Initialize module's internal state. */
void initializeBisonActionsModule();

/* Shutdown module's internal state. */
void shutdownBisonActionsModule();

/*
 * Bison semantic actions.
*/

Program * ProgramSemanticAction(CompilerState * compilerState, char * presName, ObjectDefinition * objectList, StructureDefinition * structureList, AnimationDefinition * animationList);

ObjectDefinition * ObjectListSemanticAction(ObjectDefinition * objectList, ObjectDefinition * newObject); 
ObjectDefinition * ObjectDefinitionSemanticAction(CompilerState * compilerState, ObjectType type, char * identifier, CssProperty * cssProperties); 

CssProperty * PropertyListSemanticAction(CssProperty * propertyList, CssProperty * newProperty); 	

CssProperty * PropertySemanticAction(char * propertyName, char * value); 
CssProperty * PropertyNumberSemanticAction(char * propertyName, int value); 
CssProperty * PropertyDecimalSemanticAction(char * propertyName, float value);

StructureDefinition * StructureListSemanticAction(StructureDefinition * structureList, StructureDefinition * newStructure);										
StructureDefinition * StructureDefinitionSemanticAction(CompilerState * CompilerState, char * identifier, SlideContent * content);

SlideContent * SlideContentListSemanticAction(SlideContent * slideContentList, SlideContent * newSlideContent);
SlideContent * AdditionSlideContent(CompilerState * compilerState, char * identifier, char * content); 
//SlideContent * AnchorPositionSlideContent(Position position);  
//SlideContent * RelativeSimplePositionSlideContent(char * relative , Position position); 
SlideContent * RelativeDoublePositionSlideContent(CompilerState * compilerState, char * relative, char * fixed, Position position); 

AnimationDefinition * AnimationListSemanticAction(AnimationDefinition * animationList, AnimationDefinition * newAnimation); 
AnimationDefinition * AnimationDefinitionSemanticAction(char * identifier, AnimationType type);
AnimationDefinition * AnimationDefinitionSequenceSemanticAction(CompilerState * compilerState, char * identifier, AnimationStep * steps, int repeat);
AnimationDefinition * AnimationDefinitionPairSemanticAction(CompilerState * CompilerState, char * identifier1, char * identifier2, AnimationType type);

AnimationStep * AnimationSequenceSemanticAction(AnimationStep * new, AnimationStep * steps); 
AnimationStep * AnimationStepSemanticAction(CompilerState * compilerState, char* identifier, AnimationType type); 		

#endif
