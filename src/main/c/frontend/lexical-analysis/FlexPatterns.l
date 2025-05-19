%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x OBJECTS_SECTION 
%x OBJECTS_DEFINITION 
%x PROPERTIES


/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								    { yy_push_state(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				    { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); yy_pop_state(); }
<MULTILINE_COMMENT>[[:space:]]+		    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


[[:digit:]]+						    { return IntegerLexemeAction(createLexicalAnalyzerContext()); }

"Presentation"                          { return KeywordLexemeAction(createLexicalAnalyzerContext(),PRESENTATION); }
"Objects"                               { return KeywordLexemeAction(createLexicalAnalyzerContext(),OBJECT); yy_push_state(OBJECTS_SECTION); }
<OBJECTS_SECTION>"{"                    { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); yy_push_state(OBJECTS_DEFINITION); }
<OBJECTS_SECTION>"}"                    { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); yy_pop_state(); }
<OBJECTS_DEFINITION>"{"                 { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); yy_push_state(PROPERTIES); }
<OBJECTS_DEFINITION>"}"                 { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); yy_pop_state(); }

<PROPERTIES>[a-z]+(-[a-z]+)*            {return PropertyLexemeAction(createLexicalAnalyzerContext());}
<PROPERTIES>"}"                         { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); yy_pop_state(); }

"Structure"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), STRUCTURE); }
"Animations"                            { return KeywordLexemeAction(createLexicalAnalyzerContext(), ANIMATION); }
"Slide"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), SLIDE); }
"Image"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), IMAGE); }
"Textblock"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), TEXTBLOCK); }

"add"                                   { return KeywordLexemeAction(createLexicalAnalyzerContext(), ADD); }
"with"                                  { return KeywordLexemeAction(createLexicalAnalyzerContext(), WITH); }
"start"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), START); }
"then"                                  { return KeywordLexemeAction(createLexicalAnalyzerContext(), THEN); }
"repeat"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), REPEAT); }
"end"                                   { return KeywordLexemeAction(createLexicalAnalyzerContext(), END); }
"times"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), TIMES); }

"appear"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), APPEAR); }
"dissapear"                             { return KeywordLexemeAction(createLexicalAnalyzerContext(), DISSAPEAR); }
"rotate"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), ROTATE); }



"above"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), ABOVE); }
"below"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), BELOW); }

"top"                                   { return KeywordLexemeAction(createLexicalAnalyzerContext(), TOP); }
"bottom"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), BOTTOM); }

"left"                                  { return KeywordLexemeAction(createLexicalAnalyzerContext(), LEFT); }
"right"                                 { return KeywordLexemeAction(createLexicalAnalyzerContext(), RIGHT); }
"center"                                {return KeywordLexemeAction(createLexicalAnalyzerContext(), CENTER); }

"anchor"                                { return KeywordLexemeAction(createLexicalAnalyzerContext(), ANCHOR); }

\"[^\"]*\"                              { return StringLexemeAction(createLexicalAnalyzerContext()); }
[a-zA-Z_][a-zA-Z0-9_]*                  { return IdentifierLexemeAction(createLexicalAnalyzerContext());}
":"                                     { return ColonLexemeAction(createLexicalAnalyzerContext(),COLON); }
";"                                     { return SemiColonLexemeAction(createLexicalAnalyzerContext(),SEMICOLON); }

[[:space:]]+						    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"}"                                     { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); }   
"{"                                     { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); }   

.									    { return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"