#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include "token.h" // Assuming token.h provides the Token structure and necessary utilities

// Enum to represent parsing states or grammar-related errors
typedef enum {
    PARSE_SUCCESS,
    PARSE_FAILURE
} ParseStatus;

// Function declarations for the top-level grammar rules
void parseProgram();
void parseMainFunction();
void parseBlock();
void parseStatementList();

// Function declarations for specific statement types
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

// Function declarations for expressions and operators
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

// Function declarations for literals and identifiers
void parseLiteral();
void parseBoolLiteral();
void parsePrimaryExpr();

// Function declarations for control flow
void parseIfStatement();
void parseSwitchStatement();
void parseForLoop();
void parseWhileLoop();
void parseDoWhileLoop();

// Function declarations for array handling
void parseArrayDeclaration();
void parseArrayOperations();
void parseArrayAccess();
void parseArrayAssignment();

// Function declarations for functions
void parseFunctionDeclaration();
void parseFunctionCall();
void parseParameterList();
void parseArgumentList();

// Utility functions
void matchToken(TokenType expectedType); // Match and consume a token, throw an error if mismatched
Token peekToken(); // Peek at the next token without consuming it
Token getNextToken(); // Consume and return the next token

// Error handling
void reportSyntaxError(const char *message);
void recoverFromError(); // Error recovery mechanism

#endif // SYNTAX_ANALYZER_H
