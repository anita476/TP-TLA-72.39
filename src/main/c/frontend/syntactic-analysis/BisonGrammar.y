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

	/** Non-terminals. */
	//todo turn these into structs
	String objects; /* define objects */
	String structure; /* define structure and relationships of objects */
	String animations; /* define extra behaviour*/

	String object_definition; /* define type of object definition also */
	String css_property /* has a key (kebab case) and a value (a constant)*/
	String slide_content; /* type of addition, text and so on */
	String structure_sentences /* list of slide1 { slide_content } */
	
	String animation_definition; /* define type of animation -> sequence or standalone */
	String animation_sequence; /* should i store as a map ? or something like that*/
	String animation_standalone; /* should probably know the type */


	Program * program;
	Constant * constant; /* string, integer and later on float*/
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <>
%destructor { releaseFactor($$); } <>

/** Terminals. */

/* sections */
%token <token> OBJECT
%token <token> ANIMATION
%token <token> STRUCTURE	

/* built in data types*/
%token <string> STRING
%token <integer> INTEGER

/* Non built in data types*/
%token <token> PRESENTATION
%token <token> SLIDE
%token <token> TEXTBLOCK
%token <token> IMAGE

/* naming variables and definin css properties */
%token <string> IDENTIFIER
%token <string> PROPERTY


/* separation */
%token <token> COLON 
%token <token> SEMICOLON
%token <token> OPEN_CURLY_BRACE
%token <token> CLOSE_CURLY_BRACE

%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <program> program
%type <string> objects 
%type <string> structure 
%type <string> animations 
%type <string> object_definitions
%type <string> object_definition 
%type <string> css_properties 

%type <string> css_property 
%type <string> structure_sentences
%type <string> slide_content 
%type <string> animation_definition 
%type <string> animation_sequence 
%type <string> animation_standalone;

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol. -> should use it for empty structure or objects, or animations! 

program: 
	%empty 																														{ fprintf(stderr, "Empty program\n"); }
	| PRESENTATION IDENTIFIER OPEN_CURLY_BRACE objects structure animations CLOSE_CURLY_BRACE      								{ fprintf(stdout, "Inside propgram"); }
	;
objects:
	OBJECT OPEN_CURLY_BRACE object_definitions CLOSE_CURLY_BRACE  																							{ fprintf(stdout, "Inside objects"); }
	;
object_definitions:
	%empty 																													
	|object_definitions object_definition
	;  

object_definition:
	SLIDE IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 								{ fprintf(stdout, "Inside object definition slide"); }
	|TEXTBLOCK IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 							{ fprintf(stdout, "Inside object definition text"); }
	|IMAGE IDENTIFIER OPEN_CURLY_BRACE css_properties CLOSE_CURLY_BRACE 							{ fprintf(stdout, "Inside object definition image"); }
	;
css_properties:
	%empty 																													
	| css_properties css_property
	;
css_property:
	PROPERTY COLON IDENTIFIER SEMICOLON 																							{ fprintf(stdout, "Inside css property"); }
	| PROPERTY COLON INTEGER SEMICOLON 																							{ fprintf(stdout, "Inside css property"); }
	; 
structure:
	STRUCTURE OPEN_CURLY_BRACE structure_sentences CLOSE_CURLY_BRACE 														{ fprintf(stdout, "Inside structure"); }
structure_sentences:
	%empty 																													
	| structure_sentences structure_sentence
	;
structure_sentence:
	IDENTIFIER OPEN_CURLY_BRACE slide_content CLOSE_CURLY_BRACE 														{ fprintf(stdout, "Inside structure sentence"); }
	;

slide_content:
	%empty 																													
	| ADD IDENTIFIER SEMICOLON 																							{ fprintf(stdout, "Inside slide content"); }
	| ADD WITH STRING SEMICOLON 																							{ fprintf(stdout, "Inside slide content"); }
	| ANCHOR anchor_position SEMICOLON																						{ fprintf(stdout, "Inside slide content"); }
	| IDENTIFIER simple_position SEMICOLON  																							{ fprintf(stdout, "Inside slide content"); }
	| IDENTIFIER compound_position SEMICOLON  																							{ fprintf(stdout, "Inside slide content"); }

anchor_position:
	TOP LEFT
	| TOP RIGHT
	| BOTTOM LEFT
	| BOTTOM RIGHT
	| CENTER LEFT
	| CENTER RIGHT
	| CENTER
	| CENTER TOP 
	| CENTER BOTTOM
	;
simple_position:
	ABOVE
	| BELOW
	| LEFT
	| RIGHT
	;
compound_position:
	ABOVE LEFT
	| ABOVE RIGHT
	| BELOW LEFT
	| BELOW RIGHT
	;
animation:
	ANIMATION OPEN_CURLY_BRACE animation_definitions CLOSE_CURLY_BRACE 														{ fprintf(stdout, "Inside animation"); }

animation_definitions:
	%empty 																													
	| animation_definitions animation_definition
	;
animation_definition:
	IDENTIFIER animation_type SEMICOLON
	| IDENTIFIER animation_type SEMICOLON
	| START animation_sequence END SEMICOLON
	| START animation_sequence END REPEAT INTEGER TIMES SEMICOLON
	;
animation_sequence:
	%empty 																													
	| animation_sequence animation_step
	;
animation_step:
	THEN IDENTIFIER animation_type
	;
animation_type:
	APPEAR
	| DISSAPPEAR
	| ROTATE
	;
%%

/*
program: PRESENTATION IDENTIFIER OPEN_CURLY_BRACE CLOSE_CURLY_BRACE													{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	;

expression: expression[left] ADD expression[right]					{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor														{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ExpressionFactorSemanticAction($2); }
	| constant														{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER													{ $$ = IntegerConstantSemanticAction($1); }
	;
*/