#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include "parse_tree.h"
#include "token.h"

// Enum for parsing status (optional, if used)
typedef enum {
    PARSE_SUCCESS,
    PARSE_FAILURE
} ParseStatus;

// Top-level grammar rules
void parseProgram();
void parseMainFunction();
void parseBlock();
void parseStatementList();

// Statement parsing
void parseDeclarationStatement();
void parseVariableDeclaration();
void parseTypeSpecifier();
void parseStatement();
void parseAssignmentStatement();
void parseInputStatement();
void parseOutputStatement();
void parseConditionalStatement();
void parseIterativeStatement();
void parseArrayStatement();
void parseFunctionStatement();
void parseExpressionStatement();
void parseJumpStatement();

// Expressions and operators
void parseExpression();
void parseLogicalOrExpr();
void parseLogicalAndExpr();
void parseEqualityExpr();
void parseRelationalExpr();
void parseAdditiveExpr();
void parseMultiplicativeExpr();
void parseExponentialExpr();
void parseUnaryExpr();
void parsePostfixExpr();

// Literals and identifiers
void parseLiteral();
void parseBoolLiteral();
void parsePrimaryExpr();

// Control flow
void parseIfStatement();
void parseSwitchStatement();
void parseForLoop();
void parseWhileLoop();
void parseDoWhileLoop();

// Arrays
void parseArrayDeclaration();
void parseArrayOperations();
void parseArrayAccess();
void parseArrayInitializer(); // Missing definition added
void parseArrayDimensions();  // Missing definition added

// Functions
void parseFunctionDeclaration();
void parseFunctionCall();
void parseParameterList();
void parseArgumentList();

// Utility functions
void matchToken(const char* expectedType);
Token* peekToken();
Token* getNextToken();
void syntaxError(const char* message);

// Error handling
void reportSyntaxError(const char* message);
void recoverFromError();

#endif // SYNTAX_ANALYZER_H
