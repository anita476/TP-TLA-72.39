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
%x OBJECTS
%x STRUCTURE
%x ANIMATIONS
%x PRESENTATION


/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

<*>[[:space:]]+		                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<*>"/*"								{ yy_push_state(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); yy_pop_state(); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"-"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), ADD); }

"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
"}"                                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); yy_pop_state(); }   

[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }

"Presentation"                    { yy_push_state(PRESENTATION); IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Objects {"                         { yy_push_state(OBJECTS); IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Structure {"                       { yy_push_state(STRUCTURE); IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Animations {"                      { yy_push_state(ANIMATIONS); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<OBJECTS>"Slide"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<OBJECTS>"Image"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<OBJECTS>"Textblock"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<OBJECTS>"Animation"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<OBJECTS>"define"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<STRUCTURE>"add"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<STRUCTURE>"with"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<ANIMATIONS>"start"               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<ANIMATIONS>"then"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<ANIMATIONS>"repeat"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<ANIMATIONS>"end"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<ANIMATIONS>"appear"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<ANIMATIONS>"dissapear"           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

<STRUCTURE>"above"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<STRUCTURE>"below"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<STRUCTURE>"left"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<STRUCTURE>"right"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<STRUCTURE>"anchor"               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
[[:alpha:]][[:alnum:]]*             { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"