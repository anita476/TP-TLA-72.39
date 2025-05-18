#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token IntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token SemiColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token ColonLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token OpenBraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token CloseBraceLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token StringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token PropertyLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);


/* error */
Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif
