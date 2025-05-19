%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	//todo add decimal numbers for css properties
	int integer;
	Token token;
	char * string;
	Position position;
	AnimationType animation_type;

	/** Non-terminals. */
	AnimationDefinition * animation_definition;
	AnimationStep * animation_step;
	ObjectDefinition * object_definition;
	PropertyList * css_property_list;
	CSSProperty * css_property;
	StructureSentence * structure_sentence;
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


/** Terminals. */

/* sections */
%token <token> OBJECT
%token <token> ANIMATION
%token <token> STRUCTURE	

/* built in data types*/
%token <string> STRING
%token <integer> INTEGER

/* naming variables and definin css properties */
%token <string> IDENTIFIER
%token <string> PROPERTY

/* Non built in data types*/
%token <token> PRESENTATION
%token <token> SLIDE
%token <token> TEXTBLOCK
%token <token> IMAGE

/* actions */
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
%token <token> TIMES

/* animations we accept*/
%token <token> ROTATE
%token <token> APPEAR
%token <token> DISSAPPEAR


/* separation */
%token <token> COLON 
%token <token> SEMICOLON
%token <token> OPEN_CURLY_BRACE
%token <token> CLOSE_CURLY_BRACE

/* to log errors*/
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program

%type <position> compound_position
%type <position> simple_position
%type <position> anchor_position
%type <slide_content> slide_content

%type <structure_sentence> structure_sentence
%type <structure_sentence> structure_sentences
%type <structure_sentence> structure

%type <css_property> css_property
%type <css_property_list> css_properties

%type <object_definition> object_definition
%type <object_definition> object_definitions
%type <object_definition> objects

%type <animation_type> animation_type
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

program: 
	PRESENTATION IDENTIFIER OPEN_CURLY_BRACE objects structure animation CLOSE_CURLY_BRACE     { ProgramSemanticAction(currentCompilerState(),$1,$2,$3); }
	;
objects:
	OBJECT OPEN_CURLY_BRACE object_definitions CLOSE_CURLY_BRACE  				{$$ = $3;}						
	;
object_definitions:
	%empty 																													
	|object_definitions object_definition									    { $$ = ObjectListSemanticAction(); }	
	;  

object_definition:
	SLIDE IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 			{ $$ = SlideSemanticAction(); }
	|TEXTBLOCK IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 	{ $$ = TextBlockSemanticAction(); }
	|IMAGE IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 		{ $$ = ImageSemanticAction(); }
	;
css_properties:
	%empty 																													
	| css_properties css_property												{ $$ = PropertyListSemanticAction(); }	
	;
css_property:
	PROPERTY COLON IDENTIFIER SEMICOLON 										{ $$ = PropertySemanticAction(); }
	| PROPERTY COLON INTEGER SEMICOLON 											{ $$ = PropertySemanticAction(); }
	; 
structure:
	STRUCTURE OPEN_CURLY_BRACE structure_sentences CLOSE_CURLY_BRACE 			{ $$ = $3; }
	;
structure_sentences:
	%empty 																													
	| structure_sentences structure_sentence									{$$ = StructureListSemanticAction(); }											
	;
structure_sentence:
	IDENTIFIER OPEN_CURLY_BRACE slide_content CLOSE_CURLY_BRACE 				{ $$ = StructureSentenceSemanticAction(); }
	;

slide_content:
	%empty 																													
	| ADD IDENTIFIER SEMICOLON 													{ $$ = AdditionSlideContent(); }
	| ADD IDENTIFIER WITH STRING SEMICOLON 										{ $$ = AdditionSlideContent(); }
	| ANCHOR anchor_position SEMICOLON											{ $$ = AnchorPositionSlideContent();  }
	| IDENTIFIER simple_position SEMICOLON  									{ $$ = RelativeSimplePositionSlideContent(); }
	| IDENTIFIER compound_position SEMICOLON  									{ $$ = RelativeSimplePositionSlideContent(); }
	| IDENTIFIER simple_position IDENTIFIER SEMICOLON  							{ $$ = RelativeDoublePositionSlideContent(); }
	| IDENTIFIER compound_position IDENTIFIER SEMICOLON  						{ $$ = RelativeDoublePositionSlideContent(); }
	;
anchor_position:
	TOP LEFT                       												{ $$ = POS_TOP_LEFT;}
	| TOP RIGHT																	{ $$ = POS_TOP_RIGHT;}		
	| BOTTOM LEFT																{ $$ = POS_BOTTOM_LEFT;}
	| BOTTOM RIGHT														    	{ $$ = POS_BOTTOM_RIGHT;}
	| CENTER LEFT												            	{ $$ = POS_CENTER_LEFT;}
	| CENTER RIGHT											                	{ $$ = POS_CENTER_RIGHT;}	
	| CENTER                                                                	{ $$ = POS_CENTER;}
	| CENTER TOP 														    	{ $$ = POS_CENTER_TOP;}	
	| CENTER BOTTOM                                                         	{ $$ = POS_CENTER_BOTTOM;}
	;
simple_position:
	ABOVE																		{ $$ = POS_TOP;}
	| BELOW														            	{ $$ = POS_BOTTOM;}		
	| LEFT              									                	{ $$ = POS_LEFT;}
	| RIGHT                                                                 	{ $$ = POS_RIGHT;}
	;
compound_position:
	  ABOVE LEFT									                        	{ $$ = POS_TOP_LEFT;}	
	| ABOVE RIGHT                                                           	{ $$ = POS_TOP_RIGHT;}
	| BELOW LEFT                                                            	{ $$ = POS_BOTTOM_LEFT;}
	| BELOW RIGHT                                                           	{ $$ = POS_BOTTOM_RIGHT;}
	;
animation:
	ANIMATION OPEN_CURLY_BRACE animation_definitions CLOSE_CURLY_BRACE 			{ $$ = $3; }
	;

animation_definitions:
	%empty 																													
	| animation_definitions animation_definition								{ $$ = AnimationListSemanticAction(); }
	;
animation_definition:
	IDENTIFIER animation_type SEMICOLON								        	{ $$ = AnimationDefinitionSemanticAction(); }
	| START animation_sequence END SEMICOLON									{ $$ = AnimationDefinitionSemanticAction(); }
	| START animation_sequence END REPEAT INTEGER TIMES SEMICOLON				{ $$ = AnimationDefinitionSemanticAction(); }
	;
animation_sequence:
	%empty 																													
	| animation_step animation_sequence 										{ $$ = AnimationSequenceSemanticAction($1,$2); }	
	;
animation_step:
	THEN IDENTIFIER animation_type							               		{ $$ = AnimationStepSemanticAction($2, $3); }		
	;
animation_type:
	APPEAR																		{$$ = ANIM_APPEAR;}
	| DISSAPPEAR																{$$ = ANIM_DISSAPPEAR;}	
	| ROTATE																	{$$ = ANIM_ROTATE;}
	;
%%