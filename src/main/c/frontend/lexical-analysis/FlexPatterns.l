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


/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

[[:space:]]+		                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"/*"								{ yy_push_state(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); yy_pop_state(); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"}"                                 { return OpenBraceLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); }   
"{"                                 { return CloseBraceLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); }   


[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }

"Presentation"                    { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Objects"                         { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Structure"                       { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Animations"                      { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"Slide"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Image"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Textblock"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"Animation"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"define"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"add"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"with"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"start"               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"then"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"repeat"              { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"end"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"appear"                  { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"dissapear"               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"dissolve_into"           { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"above"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"below"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"left"                 { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"right"                { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"anchor"               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


\"[^\"]*\"                      { return StringLexemeAction(createLexicalAnalyzerContext()); }
[a-zA-Z_][a-zA-Z0-9_]*          { return IdentifierLexemeAction(createLexicalAnalyzerContext());}
":"                             { return ColonLexemeAction(createLexicalAnalyzerContext(),COLON); }
";"                             { return SemiColonLexemeAction(createLexicalAnalyzerContext(),SEMICOLON); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"