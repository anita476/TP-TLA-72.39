%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	int integer;
	Token token;
	char * string;
	float decimal;
	Position position;
	AnimationType animation_type;

	/** Non-terminals. */
	AnimationDefinition * animation_definition;
	AnimationStep * animation_step;
	ObjectDefinition * object_definition;
	CssProperty * css_property;
	StructureDefinition * structure_definition;
	SlideContent * slide_content;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseObjectsSection($$); } objects
%destructor { releaseStructureSection($$); } structure
%destructor { releaseAnimationsSection($$); } animation

/** Terminals. */

/* Sections */
%token <token> OBJECT
%token <token> ANIMATION
%token <token> STRUCTURE	

/* Built in data types */
%token <string> STRING
%token <integer> INTEGER
%token <decimal> DECIMAL

/* Naming variables and CSS properties */
%token <string> IDENTIFIER
%token <string> PROPERTY

/* Non built in data types */
%token <token> PRESENTATION
%token <token> SLIDE
%token <token> TEXTBLOCK
%token <token> IMAGE

/* Actions */
%token <token> ADD
%token <token> ANCHOR
%token <token> WITH
%token <token> TOP
%token <token> BOTTOM
%token <token> BELOW
%token <token> ABOVE
%token <token> LEFT
%token <token> RIGHT
%token <token> CENTER
%token <token> START
%token <token> THEN
%token <token> END
%token <token> REPEAT

/* Animations we accept */
%token <token> ROTATE
%token <token> APPEAR
%token <token> DISAPPEAR
%token <token> JUMP
%token <token> FADE
%token <token> INTO

/* Separation */
%token <token> COLON 
%token <token> SEMICOLON
%token <token> OPEN_CURLY_BRACE
%token <token> CLOSE_CURLY_BRACE

/* To log errors*/
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program

%type <position> compound_position
%type <position> simple_position
%type <position> anchor_position
%type <slide_content> slide_content
%type <slide_content> slide_contents

%type <structure_definition> structure_definition
%type <structure_definition> structure_definitions
%type <structure_definition> structure

%type <css_property> css_property
%type <css_property> css_properties

%type <object_definition> object_definition
%type <object_definition> object_definitions
%type <object_definition> objects

%type <animation_type> animation_type
%type <animation_type> animation_type_slides

%type <animation_step> animation_step
%type <animation_step> animation_sequence
%type <animation_definition> animation_definition
%type <animation_definition> animation_definitions
%type <animation_definition> animation

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol. -> should use it for empty structure or objects, or animations! 
/* Only animations section is optional */
program: 
	PRESENTATION IDENTIFIER OPEN_CURLY_BRACE objects structure animation CLOSE_CURLY_BRACE      { $$ = ProgramSemanticAction(currentCompilerState(), $2, $4, $5, $6); }
	|PRESENTATION IDENTIFIER OPEN_CURLY_BRACE objects structure CLOSE_CURLY_BRACE 			   	{ $$ = ProgramSemanticAction(currentCompilerState(), $2, $4, $5, NULL); }
	;
objects:
	OBJECT OPEN_CURLY_BRACE object_definitions CLOSE_CURLY_BRACE  								{ $$ = $3; }						
	;
object_definitions:
	%empty 																						{ $$ = NULL; }											
	|object_definitions object_definition									    				{ $$ = ObjectListSemanticAction($1,$2); }	
	;  

object_definition:
	SLIDE IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 							{ $$ = ObjectDefinitionSemanticAction(OBJ_SLIDE, $2, $4); }
	|TEXTBLOCK IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 					{ $$ = ObjectDefinitionSemanticAction(OBJ_TEXTBLOCK, $2, $4); }
	|IMAGE IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 						{ $$ = ObjectDefinitionSemanticAction(OBJ_IMAGE, $2, $4); }
	;
css_properties:
	%empty 																						{ $$ = NULL; }											
	| css_properties css_property																{ $$ = PropertyListSemanticAction($1, $2); }	
	;
css_property:
	PROPERTY COLON IDENTIFIER SEMICOLON 														{ $$ = PropertySemanticAction($1, $3); }
	| PROPERTY COLON DECIMAL SEMICOLON 															{ $$ = PropertyDecimalSemanticAction($1, $3); }
	|PROPERTY COLON PROPERTY SEMICOLON 															{ $$ = PropertySemanticAction($1, $3); }
	| PROPERTY COLON INTEGER SEMICOLON 															{ $$ = PropertyNumberSemanticAction($1, $3); }
	; 
structure:
	STRUCTURE OPEN_CURLY_BRACE structure_definitions CLOSE_CURLY_BRACE 							{ $$ = $3; }
	;
structure_definitions:
	%empty 																						{ $$ = NULL; }										
	| structure_definitions structure_definition												{ $$ = StructureListSemanticAction($1, $2); }											
	;
structure_definition:
	IDENTIFIER OPEN_CURLY_BRACE slide_contents CLOSE_CURLY_BRACE 								{ $$ = StructureDefinitionSemanticAction($1, $3); }
	;

slide_contents:
	%empty 																						{ $$ = NULL; }											
	| slide_contents slide_content																{ $$ = SlideContentListSemanticAction($1, $2); }	
	;

slide_content:
	ADD IDENTIFIER SEMICOLON 																	{ $$ = AdditionSlideContent($2, NULL); }
	| ADD IDENTIFIER WITH STRING SEMICOLON 														{ $$ = AdditionSlideContent($2, $4); }
	| ANCHOR anchor_position SEMICOLON															{ $$ = AnchorPositionSlideContent($2); }
	| IDENTIFIER simple_position SEMICOLON  													{ $$ = RelativeSimplePositionSlideContent($1, $2); }
	| IDENTIFIER compound_position SEMICOLON  													{ $$ = RelativeSimplePositionSlideContent($1, $2); }
	| IDENTIFIER simple_position IDENTIFIER SEMICOLON  											{ $$ = RelativeDoublePositionSlideContent($1, $3, $2); }
	| IDENTIFIER compound_position IDENTIFIER SEMICOLON  										{ $$ = RelativeDoublePositionSlideContent($1, $3, $2); }
	;
anchor_position:
	TOP LEFT                       																{ $$ = POS_TOP_LEFT; }
	| TOP RIGHT																					{ $$ = POS_TOP_RIGHT; }		
	| BOTTOM LEFT																				{ $$ = POS_BOTTOM_LEFT; }
	| BOTTOM RIGHT														    					{ $$ = POS_BOTTOM_RIGHT; }
	| CENTER LEFT												            					{ $$ = POS_CENTER_LEFT; }
	| CENTER RIGHT											                					{ $$ = POS_CENTER_RIGHT; }	
	| CENTER                                                                					{ $$ = POS_CENTER; }
	| CENTER TOP 														    					{ $$ = POS_CENTER_TOP; }	
	| CENTER BOTTOM                                                         					{ $$ = POS_CENTER_BOTTOM; }
	| TOP																						{ $$ = POS_CENTER_TOP; }
	| BOTTOM																					{ $$ = POS_CENTER_BOTTOM; }
	| LEFT																						{ $$ = POS_CENTER_LEFT; }
	| RIGHT																						{ $$ = POS_CENTER_RIGHT; }
	;
simple_position:
	ABOVE																						{ $$ = POS_TOP; }
	| BELOW														            					{ $$ = POS_BOTTOM; }		
	| LEFT              									                					{ $$ = POS_LEFT; }
	| RIGHT                                                                 					{ $$ = POS_RIGHT; }
	;
compound_position:
	  ABOVE LEFT									                        					{ $$ = POS_TOP_LEFT; }	
	| ABOVE RIGHT                                                           					{ $$ = POS_TOP_RIGHT; }
	| BELOW LEFT                                                            					{ $$ = POS_BOTTOM_LEFT; }
	| BELOW RIGHT                                                           					{ $$ = POS_BOTTOM_RIGHT; }
	;
animation:
	ANIMATION OPEN_CURLY_BRACE animation_definitions CLOSE_CURLY_BRACE 							{ $$ = $3; }
	;

animation_definitions:
	%empty 																						{ $$ = NULL; }											
	| animation_definitions animation_definition												{ $$ = AnimationListSemanticAction($1,$2); }
	;
animation_definition:
	IDENTIFIER animation_type SEMICOLON								        					{ $$ = AnimationDefinitionSemanticAction($1, $2); }
	| IDENTIFIER animation_type_slides IDENTIFIER SEMICOLON										{ $$ = AnimationDefinitionPairSemanticAction($1, $3, $2); }
	| IDENTIFIER START animation_sequence END SEMICOLON											{ $$ = AnimationDefinitionSequenceSemanticAction($1, $3, 1); }
	| IDENTIFIER START animation_sequence END REPEAT INTEGER SEMICOLON							{ $$ = AnimationDefinitionSequenceSemanticAction($1, $3, $6); }
	;
animation_sequence:
	%empty 																						{ $$ = NULL; }											
	| animation_sequence animation_step  														{ $$ = AnimationSequenceSemanticAction($1, $2); }	
	;
animation_step:
	THEN IDENTIFIER animation_type							               						{ $$ = AnimationStepSemanticAction($2, $3); }		
	;
animation_type:
	APPEAR																						{ $$ = ANIM_APPEAR; }
	| DISAPPEAR																					{ $$ = ANIM_DISAPPEAR; }	
	| ROTATE																					{ $$ = ANIM_ROTATE; }
	;
animation_type_slides:
	FADE INTO																					{ $$ = ANIM_FADE_INTO; }
	| JUMP INTO																					{ $$ = ANIM_JUMP_INTO; }
	;
%%