#include "syntax_analyzer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Variables - Store tokens and tracking variables
Token tokens[1000]; // Array of tokens
int currentTokenIndex = 0; // Tracks the current token index
int totalTokens = 0; // Total number of tokens

// Utility Functions - Token management
Token* getNextToken() {
    if (currentTokenIndex < totalTokens) {
        return &tokens[currentTokenIndex++];
    }
    return NULL; // End of tokens
}

Token* peekToken() {
    if (currentTokenIndex < totalTokens) {
        return &tokens[currentTokenIndex];
    }
    return NULL; // End of tokens
}

void syntaxError(const char* message) {
    Token* token = peekToken();
    if (token) {
        printf("Syntax Error at line %d: %s\n", token->lineNumber, message);
    } else {
        printf("Syntax Error: %s\n", message);
    }
    exit(EXIT_FAILURE);
}

// Recursive Descent Parsing Functions
void parseProgram() {
    printf("Parsing Program...\n");
    while (peekToken() && strcmp(peekToken()->type, "Keyword") == 0) {
        parseDeclarationStatement();
    }
    parseMainFunction();
}

void parseMainFunction() {
    printf("Parsing Main Function...\n");
    Token* token = getNextToken();

    if (!token || strcmp(token->value, "void") != 0) {
        syntaxError("Expected 'void' for main function");
    }

    token = getNextToken();
    if (!token || strcmp(token->value, "main") != 0) {
        syntaxError("Expected 'main' for main function");
    }

    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'main'");
    }

    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after '(' in main function");
    }

    parseBlock();
}

void parseBlock() {
    printf("Parsing Block...\n");
    Token* token = getNextToken();

    if (!token || strcmp(token->type, "L_CURLY") != 0) {
        syntaxError("Expected '{' to start a block");
    }

    while (peekToken() && strcmp(peekToken()->type, "R_CURLY") != 0) {
        parseStatement();
    }

    token = getNextToken();
    if (!token || strcmp(token->type, "R_CURLY") != 0) {
        syntaxError("Expected '}' to end a block");
    }
}

void parseStatementList() {
    printf("Parsing Statement List...\n");
    while (peekToken() && strcmp(peekToken()->type, "SEMICOLON") != 0) {
        parseStatement();
    }
}

void parseDeclarationStatement() {
    printf("Parsing Declaration Statement...\n");
    while (peekToken() && strcmp(peekToken()->type, "Keyword") == 0) {
        parseVariableDeclaration();
    }
}

void parseVariableDeclaration() {
    printf("Parsing Variable Declaration...\n");
    parseTypeSpecifier();

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in variable declaration.");
    }

    token = peekToken();
    if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        getNextToken(); // Consume '='
        parseExpression(); // Parse initializer
    }

    while (peekToken() && strcmp(peekToken()->type, "COMMA") == 0) {
        getNextToken(); // Consume ','
        token = getNextToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            syntaxError("Expected an identifier after ',' in variable declaration.");
        }

        token = peekToken();
        if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
            getNextToken(); // Consume '='
            parseExpression(); // Parse initializer
        }
    }

    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of variable declaration.");
    }
}

void parseTypeSpecifier() {
    printf("Parsing Type Specifier...\n");

    Token* token = getNextToken();
    if (!token || !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
                    strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
                    strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a type specifier (int, float, etc.).");
    }
}

void parseStatement() {
    printf("Parsing Statement...\n");
    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing statement");
    }

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        parseAssignmentStatement();
    } else if (strcmp(token->value, "input") == 0) {
        parseInputStatement();
    } else if (strcmp(token->value, "printf") == 0) {
        parseOutputStatement();
    } else if (strcmp(token->value, "if") == 0) {
        parseConditionalStatement();
    } else if (strcmp(token->value, "for") == 0 || strcmp(token->value, "while") == 0 || strcmp(token->value, "do") == 0) {
        parseIterativeStatement();
    } else if (strcmp(token->value, "return") == 0 || strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
        parseJumpStatement();
    } else {
        parseExpressionStatement();
    }
}

void parseExpression() {
    printf("Parsing Expression...\n");
    // TODO: Implement full expression parsing based on grammar rules
}

// TODO: Implement remaining functions
void parseLogicalOrExpr() {
    printf("Parsing Logical OR Expression...\n");
    // TODO: Implement
}

void parseLogicalAndExpr() {
    printf("Parsing Logical AND Expression...\n");
    // TODO: Implement
}

void parseEqualityExpr() {
    printf("Parsing Equality Expression...\n");
    // TODO: Implement
}

void parseRelationalExpr() {
    printf("Parsing Relational Expression...\n");
    // TODO: Implement
}

void parseAdditiveExpr() {
    printf("Parsing Additive Expression...\n");
    // TODO: Implement
}

void parseMultiplicativeExpr() {
    printf("Parsing Multiplicative Expression...\n");
    // TODO: Implement
}

void parseExponentialExpr() {
    printf("Parsing Exponential Expression...\n");
    // TODO: Implement
}

void parseUnaryExpr() {
    printf("Parsing Unary Expression...\n");
    // TODO: Implement
}

void parsePostfixExpr() {
    printf("Parsing Postfix Expression...\n");
    // TODO: Implement
}

void parseLiteral() {
    printf("Parsing Literal...\n");
    // TODO: Implement
}

void parseBoolLiteral() {
    printf("Parsing Boolean Literal...\n");
    // TODO: Implement
}

void parsePrimaryExpr() {
    printf("Parsing Primary Expression...\n");
    // TODO: Implement
}

void parseIfStatement() {
    printf("Parsing If Statement...\n");
    // TODO: Implement
}

void parseSwitchStatement() {
    printf("Parsing Switch Statement...\n");
    // TODO: Implement
}

void parseForLoop() {
    printf("Parsing For Loop...\n");
    // TODO: Implement
}

void parseWhileLoop() {
    printf("Parsing While Loop...\n");
    // TODO: Implement
}

void parseDoWhileLoop() {
    printf("Parsing Do-While Loop...\n");
    // TODO: Implement
}

void parseArrayDeclaration() {
    printf("Parsing Array Declaration...\n");
    // TODO: Implement
}

void parseArrayOperations() {
    printf("Parsing Array Operations...\n");
    // TODO: Implement
}

void parseArrayAccess() {
    printf("Parsing Array Access...\n");
    // TODO: Implement
}

void parseArrayAssignment() {
    printf("Parsing Array Assignment...\n");
    // TODO: Implement
}

void parseFunctionDeclaration() {
    printf("Parsing Function Declaration...\n");
    // TODO: Implement
}

void parseFunctionCall() {
    printf("Parsing Function Call...\n");
    // TODO: Implement
}

void parseParameterList() {
    printf("Parsing Parameter List...\n");
    // TODO: Implement
}

void parseArgumentList() {
    printf("Parsing Argument List...\n");
    // TODO: Implement
}

void parseAssignmentStatement() {
    printf("Parsing Assignment Statement...\n");
    // TODO: Implement
}

void parseInputStatement() {
    printf("Parsing Input Statement...\n");
    // TODO: Implement
}

void parseOutputStatement() {
    printf("Parsing Output Statement...\n");
    // TODO: Implement
}

void parseConditionalStatement() {
    printf("Parsing Conditional Statement...\n");
    // TODO: Implement
}

void parseIterativeStatement() {
    printf("Parsing Iterative Statement...\n");
    // TODO: Implement
}

void parseExpressionStatement() {
    printf("Parsing Expression Statement...\n");
    // TODO: Implement
}

void parseJumpStatement() {
    printf("Parsing Jump Statement...\n");
    // TODO: Implement
}
