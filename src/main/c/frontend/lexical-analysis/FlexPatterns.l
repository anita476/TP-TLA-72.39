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

"/*"								{ yy_push_state(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); yy_pop_state(); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }


[[:digit:]]+						    { return IntegerLexemeAction(createLexicalAnalyzerContext()); }

"Presentation"                          { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Objects"                               { IgnoredLexemeAction(createLexicalAnalyzerContext()); yy_push_state(OBJECTS_SECTION); }
<OBJECTS_SECTION>"{"                    { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); yy_push_state(OBJECTS_DEFINITION); }
<OBJECTS_SECTION>"}"                    { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); yy_pop_state(); }
<OBJECTS_DEFINITION>"{"                 { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); yy_push_state(PROPERTIES); }
<OBJECTS_DEFINITION>"}"                 { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); yy_pop_state(); }

<PROPERTIES>[a-z]+(-[a-z]+)*            {return PropertyLexemeAction(createLexicalAnalyzerContext());}
<PROPERTIES>"}"                        { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); yy_pop_state(); }

"Structure"                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Animations"                           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Slide"                                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Image"                                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Textblock"                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Animation"                            { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"define"                               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"add"                                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"with"                                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"start"                                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"then"                                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"repeat"                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"end"                           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"appear"                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"dissapear"                     { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"dissolve_into"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"above"                         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"below"                         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"top"                         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"bottom"                         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"left"                          { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"right"                         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"center"                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"anchor"                        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

\"[^\"]*\"                      { return StringLexemeAction(createLexicalAnalyzerContext()); }
[a-zA-Z_][a-zA-Z0-9_]*          { return IdentifierLexemeAction(createLexicalAnalyzerContext());}
":"                             { return ColonLexemeAction(createLexicalAnalyzerContext(),COLON); }
";"                             { return SemiColonLexemeAction(createLexicalAnalyzerContext(),SEMICOLON); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"}"                                 { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); }   
"{"                                 { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); }   

.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"