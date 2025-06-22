#include "FlexActions.h"

/* MODULE INTERNAL STATE */
static Logger *_logger = NULL;
static boolean _logIgnoredLexemes = true;

void initializeFlexActionsModule() {
    _logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
    if (_logger == NULL) {
        _logger = createLogger("FlexActions");
    }
}

void shutdownFlexActionsModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/* PRIVATE FUNCTIONS */

static void _logLexicalAnalyzerContext(const char *functionName,
                                       LexicalAnalyzerContext *lexicalAnalyzerContext);

/* Logs a lexical-analyzer context in DEBUGGING level */
static void _logLexicalAnalyzerContext(const char *functionName,
                                       LexicalAnalyzerContext *lexicalAnalyzerContext) {
    char *escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
    logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)", functionName,
                 escapedLexeme, lexicalAnalyzerContext->currentContext,
                 lexicalAnalyzerContext->length, lexicalAnalyzerContext->line);
    free(escapedLexeme);
}

/* PUBLIC FUNCTIONS */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    if (_logIgnoredLexemes) {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void EndMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    if (_logIgnoredLexemes) {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

void IgnoredLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    if (_logIgnoredLexemes) {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
}

Token OpenBraceLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return OPEN_CURLY_BRACE;
}

Token CloseBraceLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return CLOSE_CURLY_BRACE;
}

Token IntegerLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->integer = atoi(lexicalAnalyzerContext->lexeme);
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return INTEGER;
}

Token DecimalLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->decimal = atof(lexicalAnalyzerContext->lexeme);
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return DECIMAL;
}

Token ColonLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return COLON;
}

Token SemiColonLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return SEMICOLON;
}

Token StringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);

    // Strip quotes from the beginning and end of the string
    char *lexeme = lexicalAnalyzerContext->lexeme;
    int length = lexicalAnalyzerContext->length;

    if (length >= 2 && lexeme[0] == '"' && lexeme[length - 1] == '"') {
        char *strippedString =
            malloc(length - 1); // -1 for null terminator, +1 for content, -2 for quotes
        if (strippedString) {
            strncpy(strippedString, lexeme + 1, length - 2);
            strippedString[length - 2] = '\0';
            lexicalAnalyzerContext->semanticValue->string = strippedString;
        } else {
            lexicalAnalyzerContext->semanticValue->string = strdup(lexicalAnalyzerContext->lexeme);
        }
    } else {
        lexicalAnalyzerContext->semanticValue->string = strdup(lexicalAnalyzerContext->lexeme);
    }

    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return STRING;
}

/* Identifies the names of variables and so on */
Token IdentifierLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->string = strdup(lexicalAnalyzerContext->lexeme);
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return IDENTIFIER;
}

Token PropertyLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->string = strdup(lexicalAnalyzerContext->lexeme);
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return PROPERTY;
}

Token KeywordLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return token;
}

/* Rejects invalid syntax */
Token UnknownLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    destroyLexicalAnalyzerContext(lexicalAnalyzerContext);
    return UNKNOWN;
}
