#include <dirent.h> // For directory operations
#include <string.h> // For string operations
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


// Function to load tokens from a file
void loadTokensFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char type[50], value[50];
    int lineNumber;

    printf("Loading tokens from %s...\n", filename);

    while (fscanf(file, "%49s %49s %d", type, value, &lineNumber) == 3) {
        strcpy(tokens[totalTokens].type, type);
        strcpy(tokens[totalTokens].value, value);
        tokens[totalTokens].lineNumber = lineNumber;
        totalTokens++;

        if (totalTokens >= 1000) {
            printf("Error: Token limit exceeded. Increase the token array size.\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    printf("Loaded %d tokens from %s.\n", totalTokens, filename);
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

void parseLogicalOrExpr() {
    printf("Parsing Logical OR Expression...\n");

    parseLogicalAndExpr(); // Parse the left-hand side as a logical AND expression

    while (peekToken() && strcmp(peekToken()->type, "LOGICAL_OR") == 0) {
        printf("Logical OR operator detected: %s\n", peekToken()->value);
        getNextToken(); // Consume the '||' operator
        parseLogicalAndExpr(); // Parse the right-hand side as another logical AND expression
    }
}


// TODO: Implement remaining functions
void parseLogicalAndExpr() {
    printf("Parsing Logical AND Expression...\n");

    parseEqualityExpr(); // Parse the left-hand side as an equality expression

    while (peekToken() && strcmp(peekToken()->type, "LOGICAL_AND") == 0) {
        printf("Logical AND operator detected: %s\n", peekToken()->value);
        getNextToken(); // Consume the '&&' operator
        parseEqualityExpr(); // Parse the right-hand side as another equality expression
    }
}


void parseLogicalAndExpr() {
    printf("Parsing Logical AND Expression...\n");

    // Parse the first operand
    parseEqualityExpr();

    // Check for multiple logical AND operators (LOGICAL_AND token)
    while (peekToken() && strcmp(peekToken()->type, "LOGICAL_AND") == 0) {
        Token* token = getNextToken(); // Consume the "&&" operator
        printf("Matched Logical AND Operator: %s\n", token->value);

        // Parse the next operand
        parseEqualityExpr();
    }
}


void parseEqualityExpr() {
    printf("Parsing Equality Expression...\n");

    // Parse the first operand
    parseRelationalExpr();

    // Check for equality operators (REL_EQ or REL_NEQ tokens)
    while (peekToken() && 
           (strcmp(peekToken()->type, "REL_EQ") == 0 || strcmp(peekToken()->type, "REL_NEQ") == 0)) {
        Token* token = getNextToken(); // Consume the operator
        printf("Matched Equality Operator: %s\n", token->value);

        // Parse the next operand
        parseRelationalExpr();
    }
}


void parseRelationalExpr() {
    printf("Parsing Relational Expression...\n");

    // Parse the first operand
    parseAdditiveExpr();

    // Check for relational operators (REL_LT, REL_GT, REL_LTE, REL_GTE tokens)
    while (peekToken() && 
           (strcmp(peekToken()->type, "REL_LT") == 0 || strcmp(peekToken()->type, "REL_GT") == 0 ||
            strcmp(peekToken()->type, "REL_LTE") == 0 || strcmp(peekToken()->type, "REL_GTE") == 0)) {
        Token* token = getNextToken(); // Consume the operator
        printf("Matched Relational Operator: %s\n", token->value);

        // Parse the next operand
        parseAdditiveExpr();
    }
}


void parseAdditiveExpr() {
    printf("Parsing Additive Expression...\n");

    // Parse the first operand
    parseMultiplicativeExpr();

    // Check for additive operators (ADD_OP, SUB_OP tokens)
    while (peekToken() && 
           (strcmp(peekToken()->type, "ADD_OP") == 0 || strcmp(peekToken()->type, "SUB_OP") == 0)) {
        Token* token = getNextToken(); // Consume the operator
        printf("Matched Additive Operator: %s\n", token->value);

        // Parse the next operand
        parseMultiplicativeExpr();
    }
}


void parseMultiplicativeExpr() {
    printf("Parsing Multiplicative Expression...\n");

    // Parse the first operand
    parseExponentialExpr();

    // Check for multiplicative operators (MULT_OP, DIV_OP, MOD_OP, INTDIV_OP tokens)
    while (peekToken() && 
           (strcmp(peekToken()->type, "MULT_OP") == 0 || 
            strcmp(peekToken()->type, "DIV_OP") == 0 || 
            strcmp(peekToken()->type, "MOD_OP") == 0 || 
            strcmp(peekToken()->type, "INTDIV_OP") == 0)) {
        Token* token = getNextToken(); // Consume the operator
        printf("Matched Multiplicative Operator: %s\n", token->value);

        // Parse the next operand
        parseExponentialExpr();
    }
}


void parseExponentialExpr() {
    printf("Parsing Exponential Expression...\n");

    // Parse the first operand
    parseUnaryExpr();

    // Check for the exponential operator (EXP_OP token)
    while (peekToken() && strcmp(peekToken()->type, "EXP_OP") == 0) {
        Token* token = getNextToken(); // Consume the operator
        printf("Matched Exponential Operator: %s\n", token->value);

        // Parse the next operand
        parseUnaryExpr();
    }
}


void parseUnaryExpr() {
    printf("Parsing Unary Expression...\n");

    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing unary expression");
        return;
    }

    // Check for unary operators (LOGICAL_NOT, SUB_OP, UNARY_INC, UNARY_DEC)
    if (strcmp(token->type, "LOGICAL_NOT") == 0 || strcmp(token->type, "SUB_OP") == 0 ||
        strcmp(token->type, "UNARY_INC") == 0 || strcmp(token->type, "UNARY_DEC") == 0) {
        token = getNextToken(); // Consume the unary operator
        printf("Matched Unary Operator: %s\n", token->value);

        // Parse the operand (recursively handle unary expressions)
        parseUnaryExpr();
    } else {
        // If no unary operator, parse as a postfix expression
        parsePostfixExpr();
    }
}


void parsePostfixExpr() {
    printf("Parsing Postfix Expression...\n");

    // Start by parsing the primary expression
    parsePrimaryExpr();

    // Handle optional postfix operators (e.g., function calls or array accesses)
    Token* token = peekToken();
    while (token && (strcmp(token->type, "L_PAREN") == 0 || strcmp(token->type, "L_BRACKET") == 0)) {
        if (strcmp(token->type, "L_PAREN") == 0) {
            printf("Parsing Function Call...\n");
            getNextToken(); // Consume '('
            parseArgumentList(); // Parse the argument list

            token = getNextToken();
            if (!token || strcmp(token->type, "R_PAREN") != 0) {
                syntaxError("Expected ')' after function arguments");
            }
        } else if (strcmp(token->type, "L_BRACKET") == 0) {
            printf("Parsing Array Access...\n");
            getNextToken(); // Consume '['
            parseExpression(); // Parse the index expression

            token = getNextToken();
            if (!token || strcmp(token->type, "R_BRACKET") != 0) {
                syntaxError("Expected ']' after array index expression");
            }
        }

        token = peekToken(); // Check for more postfix operations
    }
}


void parseLiteral() {
    printf("Parsing Literal...\n");

    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing literal.");
    }

    if (strcmp(token->type, "INT_LITERAL") == 0 ||
        strcmp(token->type, "FLOAT_LITERAL") == 0 ||
        strcmp(token->type, "CHAR_LITERAL") == 0 ||
        strcmp(token->type, "STRING_LITERAL") == 0) {
        printf("Literal detected: %s\n", token->value);
        getNextToken(); // Consume the literal token
    } else if (strcmp(token->type, "Keyword") == 0 &&
               (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        printf("Boolean literal detected: %s\n", token->value);
        getNextToken(); // Consume the boolean literal
    } else {
        syntaxError("Expected a literal (integer, float, char, string, or boolean).");
    }
}


void parseBoolLiteral() {
    printf("Parsing Boolean Literal...\n");

    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing boolean literal.");
    }

    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        printf("Boolean literal detected: %s\n", token->value);
        getNextToken(); // Consume the boolean literal token
    } else {
        syntaxError("Expected a boolean literal ('true' or 'false').");
    }
}


void parsePrimaryExpr() {
    printf("Parsing Primary Expression...\n");

    Token* token = getNextToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing a primary expression");
    }

    if (strcmp(token->type, "INT_LITERAL") == 0 || 
        strcmp(token->type, "FLOAT_LITERAL") == 0 || 
        strcmp(token->type, "CHAR_LITERAL") == 0 || 
        strcmp(token->type, "STRING_LITERAL") == 0 || 
        strcmp(token->type, "Keyword") == 0 && 
        (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        // It's a literal, parsed successfully
        printf("Literal detected: %s\n", token->value);
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // It's an identifier
        printf("Identifier detected: %s\n", token->value);
    } else if (strcmp(token->type, "L_PAREN") == 0) {
        // It's a grouped expression
        parseExpression(); // Parse the inner expression

        token = getNextToken();
        if (!token || strcmp(token->type, "R_PAREN") != 0) {
            syntaxError("Expected ')' to close the grouped expression");
        }
    } else {
        syntaxError("Expected a literal, identifier, or grouped expression");
    }
}


void parseIfStatement() {
    printf("Parsing If Statement...\n");

    // Match the 'if' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "if") != 0) {
        syntaxError("Expected 'if' keyword.");
    }

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'if'.");
    }

    // Parse the condition inside the parentheses
    parseExpression();

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after condition.");
    }

    // Parse the statement block
    parseStatement();

    // Check for optional 'else' or 'else if'
    token = peekToken();
    if (token && strcmp(token->value, "else") == 0) {
        getNextToken(); // Consume 'else'

        token = peekToken();
        if (token && strcmp(token->value, "if") == 0) {
            // Parse an 'else if' statement
            parseIfStatement();
        } else {
            // Parse the 'else' statement block
            parseStatement();
        }
    }
}


void parseCaseStatement() {
    printf("Parsing Case Statement...\n");

    // Match the 'case' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "case") != 0) {
        syntaxError("Expected 'case' keyword.");
    }

    // Match the case value (INT_LITERAL or CHAR_LITERAL)
    token = getNextToken();
    if (!token || !(strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "CHAR_LITERAL") == 0)) {
        syntaxError("Expected a literal value after 'case'.");
    }

    // Match the colon ':'
    token = getNextToken();
    if (!token || strcmp(token->type, "COLON") != 0) {
        syntaxError("Expected ':' after case value.");
    }

    // Parse the statements inside the case
    while (peekToken() && strcmp(peekToken()->value, "break") != 0) {
        parseStatement();
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || strcmp(token->value, "break") != 0) {
        syntaxError("Expected 'break' in case statement.");
    }

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after 'break'.");
    }
}


void parseDefaultCase() {
    printf("Parsing Default Case...\n");

    // Match the 'default' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "default") != 0) {
        syntaxError("Expected 'default' keyword.");
    }

    // Match the colon ':'
    token = getNextToken();
    if (!token || strcmp(token->type, "COLON") != 0) {
        syntaxError("Expected ':' after 'default'.");
    }

    // Parse the statements inside the default case
    while (peekToken() && strcmp(peekToken()->value, "break") != 0) {
        parseStatement();
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || strcmp(token->value, "break") != 0) {
        syntaxError("Expected 'break' in default case.");
    }

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after 'break'.");
    }
}

void parseForInit() {
    printf("Parsing For Loop Initialization...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop initialization.");
    }

    if (strcmp(token->type, "Keyword") == 0) {
        // Variable declaration
        parseVariableDeclaration();
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Assignment statement
        parseAssignmentStatement();
    } else {
        syntaxError("Invalid initialization in 'for' loop.");
    }
}


void parseStatementBlock() {
    printf("Parsing Statement Block...\n");

    Token* token = peekToken();
    if (token && strcmp(token->type, "L_CURLY") == 0) {
        parseBlock(); // Block with multiple statements
    } else {
        parseStatement(); // Single statement
    }
}


void parseForLoop() {
    printf("Parsing For Loop...\n");

    // Match the 'for' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "for") != 0) {
        syntaxError("Expected 'for' keyword.");
    }

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'for'.");
    }

    // Parse the optional initialization statement
    if (peekToken() && strcmp(peekToken()->type, "SEMICOLON") != 0) {
        parseForInit();
    }

    // Match the first semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after initialization in 'for' loop.");
    }

    // Parse the optional condition
    if (peekToken() && strcmp(peekToken()->type, "SEMICOLON") != 0) {
        parseExpression();
    }

    // Match the second semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after condition in 'for' loop.");
    }


void parseIncDec() {
    printf("Parsing Increment/Decrement...\n");

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in increment/decrement statement.");
    }

    token = getNextToken();
    if (!token || !(strcmp(token->type, "UNARY_INC") == 0 || strcmp(token->type, "UNARY_DEC") == 0)) {
        syntaxError("Expected '++' or '--' after identifier.");
    }
}


void parseForUpdate() {
    printf("Parsing For Loop Update...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop update.");
    }

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        Token* nextToken = peekToken();
        if (nextToken && (strcmp(nextToken->type, "ASSIGNMENT_OP") == 0 ||
                          strcmp(nextToken->type, "ADD_ASSIGNOP") == 0 ||
                          strcmp(nextToken->type, "SUB_ASSIGNOP") == 0)) {
            parseAssignmentStatement();
        } else if (nextToken && (strcmp(nextToken->type, "UNARY_INC") == 0 ||
                                 strcmp(nextToken->type, "UNARY_DEC") == 0)) {
            parseIncDec();
        } else {
            syntaxError("Invalid update in 'for' loop.");
        }
    } else {
        syntaxError("Expected an identifier in 'for' loop update.");
    }
}


    // Parse the optional update statement
    if (peekToken() && strcmp(peekToken()->type, "R_PAREN") != 0) {
        parseForUpdate();
    }

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after 'for' loop update.");
    }

    // Parse the loop body
    parseStatementBlock();
}


void parseWhileLoop() {
    printf("Parsing While Loop...\n");

    // Match "while"
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword");
    }

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'while'");
    }

    // Parse the condition expression
    parseExpression();

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after condition expression in 'while' loop");
    }

    // Parse the statement block
    parseStatementBlock();
}


void parseDoWhileLoop() {
    printf("Parsing Do-While Loop...\n");

    // Match "do"
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "do") != 0) {
        syntaxError("Expected 'do' keyword");
    }

    // Parse the statement block
    parseStatementBlock();

    // Match "while"
    token = getNextToken();
    if (!token || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword after 'do' block");
    }

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'while' in do-while loop");
    }

    // Parse the condition expression
    parseExpression();

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after condition expression in 'do-while' loop");
    }

    // Match ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of 'do-while' loop");
    }
}


void parseArrayDeclaration() {
    printf("Parsing Array Declaration...\n");

    // Match the "array" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "array") != 0) {
        syntaxError("Expected 'array' keyword for array declaration");
    }

    // Parse the array type
    parseArrayType();

    // Match the identifier (array name)
    token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for the array name");
    }

    // Parse the array dimensions
    parseArrayDimensions();

    // Check for optional assignment
    token = peekToken();
    if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        getNextToken(); // Consume '='
        parseArrayInitializer(); // Parse the initializer
    }

    // Match semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of the array declaration");
    }
}


void parseArrayOperations() {
    printf("Parsing Array Operations...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing array operations");
    }

    // Determine the type of array operation
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        getNextToken(); // Consume the identifier

        // Check if it's an array access operation
        token = peekToken();
        if (token && strcmp(token->type, "L_BRACKET") == 0) {
            parseArrayAccess();
        } else if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
            // Check if it's an array assignment operation
            getNextToken(); // Consume the '='
            parseExpression(); // Parse the expression to be assigned
            token = getNextToken();

            if (!token || strcmp(token->type, "SEMICOLON") != 0) {
                syntaxError("Expected ';' at the end of array assignment");
            }
        } else {
            syntaxError("Invalid array operation");
        }
    } else {
        syntaxError("Expected an identifier for array operations");
    }
}


void parseArrayAccess() {
    printf("Parsing Array Access...\n");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for array access");
    }

    getNextToken(); // Consume the identifier

    // Parse array indices enclosed in brackets
    while ((token = peekToken()) && strcmp(token->type, "L_BRACKET") == 0) {
        getNextToken(); // Consume '['

        // Parse the index expression
        parseExpression();

        token = getNextToken(); // Consume ']'
        if (!token || strcmp(token->type, "R_BRACKET") != 0) {
            syntaxError("Expected ']' after array index expression");
        }
    }

    // Check for valid termination of array access
    token = peekToken();
    if (!token || (strcmp(token->type, "ASSIGNMENT_OP") != 0 && strcmp(token->type, "SEMICOLON") != 0)) {
        syntaxError("Invalid token following array access");
    }

    if (strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        getNextToken(); // Consume '='
        parseExpression(); // Parse the value to be assigned

        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after array assignment");
        }
    } else if (strcmp(token->type, "SEMICOLON") == 0) {
        getNextToken(); // Consume the semicolon
    }
}


void parseArrayAccess() {
    printf("Parsing Array Access...\n");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for array access");
    }

    getNextToken(); // Consume the identifier

    // Parse array indices enclosed in brackets
    while ((token = peekToken()) && strcmp(token->type, "L_BRACKET") == 0) {
        getNextToken(); // Consume '['

        // Parse the index expression
        parseExpression();

        token = getNextToken(); // Consume ']'
        if (!token || strcmp(token->type, "R_BRACKET") != 0) {
            syntaxError("Expected ']' after array index expression");
        }
    }

    // Check for valid termination of array access
    token = peekToken();
    if (!token || (strcmp(token->type, "ASSIGNMENT_OP") != 0 && strcmp(token->type, "SEMICOLON") != 0)) {
        syntaxError("Invalid token following array access");
    }

    if (strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        getNextToken(); // Consume '='
        parseExpression(); // Parse the value to be assigned

        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after array assignment");
        }
    } else if (strcmp(token->type, "SEMICOLON") == 0) {
        getNextToken(); // Consume the semicolon
    }
}


void parseFunctionDeclaration() {
    printf("Parsing Function Declaration...\n");

    // Parse the return type
    parseTypeSpecifier();

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for function name");
    }

    // Expect '(' to start the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after function name");
    }

    // Parse the parameter list
    token = peekToken();
    if (token && strcmp(token->type, "R_PAREN") != 0) {
        parseParameterList(); // Parse the parameters if any
    }

    // Expect ')' to end the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after parameter list");
    }

    // Check for a semicolon or a compound statement
    token = peekToken();
    if (token && strcmp(token->type, "SEMICOLON") == 0) {
        getNextToken(); // Consume the semicolon (function declaration only)
    } else {
        parseBlock(); // Parse the function body (compound statement)
    }
}


void parseFunctionCall() {
    printf("Parsing Function Call...\n");

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for function name");
    }

    // Expect '(' to start the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after function name");
    }

    // Parse the argument list if there are any
    token = peekToken();
    if (token && strcmp(token->type, "R_PAREN") != 0) {
        parseArgumentList();
    }

    // Expect ')' to end the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after argument list");
    }

    // Expect a semicolon to end the function call
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after function call");
    }
}


void parseParameterList() {
    printf("Parsing Parameter List...\n");

    // Parse the first parameter
    parseParameter();

    // Check for additional parameters separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "COMMA") == 0) {
        getNextToken(); // Consume the comma
        parseParameter(); // Parse the next parameter
        token = peekToken(); // Update the token to check for more commas
    }
}


void parseArgumentList() {
    printf("Parsing Argument List...\n");

    // Parse the first argument
    parseExpression(); // Arguments are typically expressions

    // Check for additional arguments separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "COMMA") == 0) {
        getNextToken(); // Consume the comma
        parseExpression(); // Parse the next argument
        token = peekToken(); // Update the token to check for more commas
    }
}


void parseAssignmentStatement() {
    printf("Parsing Assignment Statement...\n");

    // Expect an identifier at the start of the assignment
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in assignment statement.");
    }

    // Expect an assignment operator
    token = getNextToken();
    if (!token || !(strcmp(token->type, "ASSIGNMENT_OP") == 0 || 
                    strcmp(token->type, "ADD_ASSIGNOP") == 0 ||
                    strcmp(token->type, "SUB_ASSIGNOP") == 0 ||
                    strcmp(token->type, "MULTI_ASSIGNOP") == 0 ||
                    strcmp(token->type, "DIV_ASSIGNOP") == 0 ||
                    strcmp(token->type, "MOD_ASSIGNOP") == 0 ||
                    strcmp(token->type, "INTDIV_ASSIGNOP") == 0)) {
        syntaxError("Expected an assignment operator.");
    }

    // Expect either an expression or a variable list
    token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in assignment statement.");
    }

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Handle variable list
        parseVariableList();
    } else {
        // Handle expression or literal
        parseExpression();
    }

    // Expect a semicolon to terminate the assignment statement
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of assignment statement.");
    }
}


void parseInputStatement() {
    printf("Parsing Input Statement...\n");

    // Expect the "input" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "input") != 0) {
        syntaxError("Expected 'input' keyword at the start of an input statement.");
    }

    // Expect an opening parenthesis
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'input'.");
    }

    // Parse the variable list
    parseVariableList();

    // Expect a closing parenthesis
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' to close the input statement.");
    }

    // Expect a semicolon to terminate the input statement
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of input statement.");
    }
}


void parseOutputStatement() {
    printf("Parsing Output Statement...\n");

    // Expect the "printf" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "printf") != 0) {
        syntaxError("Expected 'printf' keyword at the start of an output statement.");
    }

    // Expect an opening parenthesis
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'printf'.");
    }

    // Parse the output list
    parseOutputList();

    // Expect a closing parenthesis
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' to close the output statement.");
    }

    // Expect a semicolon to terminate the output statement
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of output statement.");
    }
}


void parseConditionalStatement() {
    printf("Parsing Conditional Statement...\n");

    // Peek at the token to decide which conditional structure to parse
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing a conditional statement.");
    }

    if (strcmp(token->value, "if") == 0) {
        parseIfStatement();
    } else if (strcmp(token->value, "switch") == 0) {
        parseSwitchStatement();
    } else {
        syntaxError("Expected 'if' or 'switch' at the start of a conditional statement.");
    }
}


void parseIterativeStatement() {
    printf("Parsing Iterative Statement...\n");

    // Peek at the token to decide which iterative structure to parse
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing an iterative statement.");
    }

    if (strcmp(token->value, "for") == 0) {
        parseForLoop();
    } else if (strcmp(token->value, "while") == 0) {
        parseWhileLoop();
    } else if (strcmp(token->value, "do") == 0) {
        parseDoWhileLoop();
    } else {
        syntaxError("Expected 'for', 'while', or 'do' at the start of an iterative statement.");
    }
}


void parseExpressionStatement() {
    printf("Parsing Expression Statement...\n");

    // Parse the expression
    parseExpression();

    // Expect a semicolon to terminate the expression statement
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of an expression statement.");
    }
}


void parseJumpStatement() {
    printf("Parsing Jump Statement...\n");

    Token* token = getNextToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing jump statement.");
    }

    if (strcmp(token->value, "return") == 0) {
        // Parse `return` statement
        if (peekToken() && (strcmp(peekToken()->type, "IDENTIFIER") == 0 || 
                            strcmp(peekToken()->type, "INT_LITERAL") == 0 || 
                            strcmp(peekToken()->type, "FLOAT_LITERAL") == 0 || 
                            strcmp(peekToken()->type, "CHAR_LITERAL") == 0 || 
                            strcmp(peekToken()->type, "STRING_LITERAL") == 0)) {
            parseExpression();
        }

        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after return statement.");
        }

    } else if (strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
        // Parse `break` or `continue` statement
        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after break/continue statement.");
        }

    } else {
        syntaxError("Unrecognized jump statement. Expected 'return', 'break', or 'continue'.");
    }
}


// Main function
int main() {
    printf("Starting Syntax Analysis...\n");

    // Open the current directory to look for .prsm files
    struct dirent *entry;
    DIR *dp = opendir(".");
    if (!dp) {
        printf("Error: Unable to open current directory.\n");
        return 1;
    }

    // List .prsm files
    char prsmFiles[100][256];
    int fileCount = 0;

    printf("Available .prsm files:\n");
    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".prsm") && strlen(entry->d_name) > 5) { // Check for .prsm extension
            printf("%d. %s\n", fileCount + 1, entry->d_name);
            strcpy(prsmFiles[fileCount], entry->d_name);
            fileCount++;
        }
    }
    closedir(dp);

    if (fileCount == 0) {
        printf("No .prsm files found in the current directory.\n");
        return 1;
    }

    // Prompt user to select a file
    int choice;
    printf("\nEnter the number of the file to analyze (1-%d): ", fileCount);
    scanf("%d", &choice);

    if (choice < 1 || choice > fileCount) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    // Load tokens from the selected .prsm file
    const char *selectedFile = prsmFiles[choice - 1];
    printf("\nSelected file: %s\n", selectedFile);
    loadTokensFromFile(selectedFile);

    // Begin parsing
    parseProgram();

    printf("Syntax Analysis Completed Successfully!\n");
    return 0;
}

