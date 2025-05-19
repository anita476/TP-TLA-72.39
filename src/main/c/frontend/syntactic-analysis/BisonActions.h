#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */
/*
Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression);
*/

Program * ProgramSemanticAction(CompilerState * compilerState, ObjectDefinition * objectList, StructureSentence * structureList, AnimationDefinition * animationList);

ObjectDefinition * ObjectListSemanticAction(); 
ObjectDefinition * SlideSemanticAction(); 
ObjectDefinition * TextBlockSemanticAction(); 
ObjectDefinition * ImageSemanticAction(); 

CssProperty * PropertyListSemanticAction(); 	
CssProperty * PropertySemanticAction(); 

StructureSentence * StructureListSemanticAction();										
StructureSentence * StructureSentenceSemanticAction();

SlideContent * AdditionSlideContent(); 
SlideContent * AdditionSlideContent(); 
SlideContent * AnchorPositionSlideContent();  
SlideContent * RelativeSimplePositionSlideContent(); 
SlideContent * RelativeDoublePositionSlideContent(); 

AnimationDefinition * AnimationListSemanticAction(); 
AnimationDefinition * AnimationDefinitionSemanticAction(AnimationStep);
AnimationStep * AnimationSequenceSemanticAction(AnimationStep * new, AnimationStep * steps); 
AnimationStep * AnimationStepSemanticAction(char* identifier, AnimationType type); 		
#endif
