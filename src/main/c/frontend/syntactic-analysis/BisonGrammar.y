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

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol. -> should use it for empty structure or objects, or animations! 

program: 
	%empty 																														{ fprintf(stderr, "Empty program\n"); }
	| PRESENTATION IDENTIFIER OPEN_CURLY_BRACE objects structure animation CLOSE_CURLY_BRACE      								{ fprintf(stdout, "Inside propgram"); }
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
	;
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
	| ADD IDENTIFIER WITH STRING SEMICOLON 																							{ fprintf(stdout, "Inside slide content"); }
	| ANCHOR anchor_position SEMICOLON																						{ fprintf(stdout, "Inside slide content"); }
	| IDENTIFIER simple_position SEMICOLON  																							{ fprintf(stdout, "Inside slide content"); }
	| IDENTIFIER compound_position SEMICOLON  																							{ fprintf(stdout, "Inside slide content"); }
	;
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
	;

animation_definitions:
	%empty 																													
	| animation_definitions animation_definition
	;
animation_definition:
	IDENTIFIER animation_type SEMICOLON
	| START animation_sequence END SEMICOLON
	| START animation_sequence END REPEAT INTEGER TIMES SEMICOLON
	;
animation_sequence:
	%empty 																													
	| animation_step animation_sequence 
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