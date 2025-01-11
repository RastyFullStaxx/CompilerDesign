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

// Function prototypes for parse tree handling
void writeParseTreeToFile(ParseTreeNode* node, FILE* file);
void freeParseTree(ParseTreeNode* node);
void syntaxError(const char* message);



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
#include <string.h> // Add this for strstr

// Function to load tokens from a file
int loadTokensFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char line[256];
    int tokenCount = 0;

    printf("Loading tokens from %s...\n", filename);

    while (fgets(line, sizeof(line), file)) {
        // Debug: Print each line read
        printf("Processing line: %s", line);

        // Skip header and separator lines
        if (strstr(line, "Token Type") || strstr(line, "Value") || strstr(line, "Line Number") || strstr(line, "-")) {
            printf("Skipping header/separator line.\n");
            continue;
        }

        // Parse valid token lines
        Token token;
        int matched = sscanf(line, "%49s %49s %d", token.type, token.value, &token.lineNumber);

        if (matched == 3) {
            tokens[tokenCount++] = token;
            printf("Loaded token: Type=%s, Value=%s, Line=%d\n", token.type, token.value, token.lineNumber);
        } else {
            printf("Skipping invalid line: %s", line);
        }
    }

    fclose(file);

    printf("Loaded %d tokens from %s.\n", tokenCount, filename);
    return tokenCount;
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
ParseTreeNode* parseProgram() {
    printf("Parsing Program...\n");

    ParseTreeNode* programNode = createParseTreeNode("Program", "");

    while (peekToken() && strcmp(peekToken()->type, "Keyword") == 0) {
        addChild(programNode, parseDeclarationStatement());
    }

    addChild(programNode, parseMainFunction());

    return programNode;
}


ParseTreeNode* matchToken(const char* expectedType) {
    Token* token = getNextToken();
    if (!token || strcmp(token->type, expectedType) != 0) {
        char errorMessage[100];
        snprintf(errorMessage, sizeof(errorMessage), 
                 "Expected token of type '%s', but got '%s' at line %d.", 
                 expectedType, token ? token->type : "EOF", 
                 token ? token->lineNumber : -1);
        syntaxError(errorMessage);
    }

    return createParseTreeNode(token->type, token->value); // Create and return the node
}




ParseTreeNode* parseBlock() {
    printf("Parsing Block...\n");

    ParseTreeNode* blockNode = createParseTreeNode("Block", "");

    matchToken("L_CURLY"); // Match '{'
    addChild(blockNode, createParseTreeNode("Symbol", "{"));

    while (peekToken() && strcmp(peekToken()->type, "R_CURLY") != 0) {
        addChild(blockNode, parseStatement());
    }

    matchToken("R_CURLY"); // Match '}'
    addChild(blockNode, createParseTreeNode("Symbol", "}"));

    return blockNode;
}


ParseTreeNode* parseStatementList() {
    printf("Parsing Statement List...\n");

    // Create a node for the statement list
    ParseTreeNode* statementListNode = createParseTreeNode("StatementList", "");

    while (peekToken() && strcmp(peekToken()->type, "SEMICOLON") != 0) {
        // Parse each statement and add it as a child
        addChild(statementListNode, parseStatement());
    }

    return statementListNode;
}


ParseTreeNode* parseDeclarationStatement() {
    printf("Parsing Declaration Statement...\n");

    ParseTreeNode* declarationNode = createParseTreeNode("DeclarationStatement","");

    while (peekToken() && strcmp(peekToken()->type, "Keyword") == 0) {
        addChild(declarationNode, parseVariableDeclaration());
    }

    return declarationNode;
}



ParseTreeNode* parseExpression() {
    printf("Parsing Expression...\n");

    ParseTreeNode* exprNode = createParseTreeNode("Expression","");

    addChild(exprNode, parseLogicalOrExpr());

    return exprNode;
}



ParseTreeNode* parseVariableDeclaration() {
    printf("Parsing Variable Declaration...\n");

    // Create a node for the variable declaration
    ParseTreeNode* variableDeclarationNode = createParseTreeNode("VariableDeclaration", "");

    // Parse the type specifier and add it as a child
    addChild(variableDeclarationNode, parseTypeSpecifier());

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in variable declaration.");
    }

    // Add the identifier as a child
    addChild(variableDeclarationNode, createParseTreeNode("Identifier", token->value));

    // Check for optional assignment
    token = peekToken();
    if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        getNextToken(); // Consume '='
        // Parse the initializer and add it as a child
        addChild(variableDeclarationNode, parseExpression());
    }

    // Handle multiple declarations separated by commas
    while (peekToken() && strcmp(peekToken()->type, "COMMA") == 0) {
        getNextToken(); // Consume ','

        token = getNextToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            syntaxError("Expected an identifier after ',' in variable declaration.");
        }

        // Add the identifier as a child
        addChild(variableDeclarationNode, createParseTreeNode("Identifier", token->value));

        // Check for optional assignment
        token = peekToken();
        if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
            getNextToken(); // Consume '='
            // Parse the initializer and add it as a child
            addChild(variableDeclarationNode, parseExpression());
        }
    }

    // Ensure the statement ends with a semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of variable declaration.");
    }

    return variableDeclarationNode;
}



ParseTreeNode* parseTypeSpecifier() {
    printf("Parsing Type Specifier...\n");

    Token* token = getNextToken();
    if (!token || !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
                    strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
                    strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a type specifier (int, float, etc.).");
    }

    // Create a parse tree node for the type specifier
    return createParseTreeNode("TypeSpecifier", token->value);
}


ParseTreeNode* parseStatement() {
    printf("Parsing Statement...\n");
    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing statement");
    }

    // Create a parse tree node for the statement
    ParseTreeNode* statementNode = createParseTreeNode("Statement", "");

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        addChild(statementNode, parseAssignmentStatement());
    } else if (strcmp(token->value, "input") == 0) {
        addChild(statementNode, parseInputStatement());
    } else if (strcmp(token->value, "printf") == 0) {
        addChild(statementNode, parseOutputStatement());
    } else if (strcmp(token->value, "if") == 0) {
        addChild(statementNode, parseConditionalStatement());
    } else if (strcmp(token->value, "for") == 0 || strcmp(token->value, "while") == 0 || strcmp(token->value, "do") == 0) {
        addChild(statementNode, parseIterativeStatement());
    } else if (strcmp(token->value, "return") == 0 || strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
        addChild(statementNode, parseJumpStatement());
    } else {
        addChild(statementNode, parseExpressionStatement());
    }

    return statementNode;
}


ParseTreeNode* parseLogicalOrExpr() {
    printf("Parsing Logical OR Expression...\n");

    // Create a parse tree node for the Logical OR expression
    ParseTreeNode* logicalOrNode = createParseTreeNode("LogicalOrExpr", "");

    // Parse the left-hand side as a Logical AND expression
    addChild(logicalOrNode, parseLogicalAndExpr());

    // Check for multiple '||' operators
    while (peekToken() && strcmp(peekToken()->type, "LOGICAL_OR") == 0) {
        Token* operatorToken = getNextToken(); // Consume the '||' operator
        printf("Logical OR operator detected: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(logicalOrNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the right-hand side as another Logical AND expression
        addChild(logicalOrNode, parseLogicalAndExpr());
    }

    return logicalOrNode;
}



// TODO: Implement remaining functions
ParseTreeNode* parseLogicalAndExpr() {
    printf("Parsing Logical AND Expression...\n");

    // Create a parse tree node for the Logical AND expression
    ParseTreeNode* logicalAndNode = createParseTreeNode("LogicalAndExpr", "");

    // Parse the left-hand side as an Equality expression
    addChild(logicalAndNode, parseEqualityExpr());

    // Check for multiple '&&' operators
    while (peekToken() && strcmp(peekToken()->type, "LOGICAL_AND") == 0) {
        Token* operatorToken = getNextToken(); // Consume the '&&' operator
        printf("Logical AND operator detected: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(logicalAndNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the right-hand side as another Equality expression
        addChild(logicalAndNode, parseEqualityExpr());
    }

    return logicalAndNode;
}


ParseTreeNode* parseEqualityExpr() {
    printf("Parsing Equality Expression...\n");

    // Create a parse tree node for the Equality expression
    ParseTreeNode* equalityNode = createParseTreeNode("EqualityExpr", "");

    // Parse the first operand (Relational expression)
    addChild(equalityNode, parseRelationalExpr());

    // Check for equality operators (REL_EQ or REL_NEQ tokens)
    while (peekToken() &&
           (strcmp(peekToken()->type, "REL_EQ") == 0 || strcmp(peekToken()->type, "REL_NEQ") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("Matched Equality Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(equalityNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the next operand (Relational expression)
        addChild(equalityNode, parseRelationalExpr());
    }

    return equalityNode;
}



ParseTreeNode* parseRelationalExpr() {
    printf("Parsing Relational Expression...\n");

    // Create a parse tree node for the Relational expression
    ParseTreeNode* relationalNode = createParseTreeNode("RelationalExpr", "");

    // Parse the first operand (Additive expression)
    addChild(relationalNode, parseAdditiveExpr());

    // Check for relational operators (REL_LT, REL_GT, REL_LTE, REL_GTE tokens)
    while (peekToken() &&
           (strcmp(peekToken()->type, "REL_LT") == 0 || strcmp(peekToken()->type, "REL_GT") == 0 ||
            strcmp(peekToken()->type, "REL_LTE") == 0 || strcmp(peekToken()->type, "REL_GTE") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("Matched Relational Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(relationalNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the next operand (Additive expression)
        addChild(relationalNode, parseAdditiveExpr());
    }

    return relationalNode;
}



ParseTreeNode* parseAdditiveExpr() {
    printf("Parsing Additive Expression...\n");

    // Create a parse tree node for the Additive expression
    ParseTreeNode* additiveNode = createParseTreeNode("AdditiveExpr", "");

    // Parse the first operand (Multiplicative expression)
    addChild(additiveNode, parseMultiplicativeExpr());

    // Check for additive operators (ADD_OP, SUB_OP tokens)
    while (peekToken() &&
           (strcmp(peekToken()->type, "ADD_OP") == 0 || strcmp(peekToken()->type, "SUB_OP") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("Matched Additive Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(additiveNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the next operand (Multiplicative expression)
        addChild(additiveNode, parseMultiplicativeExpr());
    }

    return additiveNode;
}



ParseTreeNode* parseMultiplicativeExpr() {
    printf("Parsing Multiplicative Expression...\n");

    // Create a parse tree node for the Multiplicative expression
    ParseTreeNode* multiplicativeNode = createParseTreeNode("MultiplicativeExpr", "");

    // Parse the first operand (Exponential expression)
    addChild(multiplicativeNode, parseExponentialExpr());

    // Check for multiplicative operators (MULT_OP, DIV_OP, MOD_OP, INTDIV_OP tokens)
    while (peekToken() &&
           (strcmp(peekToken()->type, "MULT_OP") == 0 || 
            strcmp(peekToken()->type, "DIV_OP") == 0 || 
            strcmp(peekToken()->type, "MOD_OP") == 0 || 
            strcmp(peekToken()->type, "INTDIV_OP") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("Matched Multiplicative Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(multiplicativeNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the next operand (Exponential expression)
        addChild(multiplicativeNode, parseExponentialExpr());
    }

    return multiplicativeNode;
}



ParseTreeNode* parseExponentialExpr() {
    printf("Parsing Exponential Expression...\n");

    // Create a parse tree node for the Exponential expression
    ParseTreeNode* exponentialNode = createParseTreeNode("ExponentialExpr", "");

    // Parse the first operand (Unary expression)
    addChild(exponentialNode, parseUnaryExpr());

    // Check for the exponential operator (EXP_OP token)
    while (peekToken() && strcmp(peekToken()->type, "EXP_OP") == 0) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("Matched Exponential Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(exponentialNode, createParseTreeNode("Operator", operatorToken->value));

        // Parse the next operand (Unary expression)
        addChild(exponentialNode, parseUnaryExpr());
    }

    return exponentialNode;
}



ParseTreeNode* parseUnaryExpr() {
    printf("Parsing Unary Expression...\n");

    // Create a parse tree node for the Unary Expression
    ParseTreeNode* unaryNode = createParseTreeNode("UnaryExpr", "");

    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing unary expression");
        return NULL; // Return NULL in case of an error
    }

    // Check for unary operators (LOGICAL_NOT, SUB_OP, UNARY_INC, UNARY_DEC)
    if (strcmp(token->type, "LOGICAL_NOT") == 0 || strcmp(token->type, "SUB_OP") == 0 ||
        strcmp(token->type, "UNARY_INC") == 0 || strcmp(token->type, "UNARY_DEC") == 0) {
        token = getNextToken(); // Consume the unary operator
        printf("Matched Unary Operator: %s\n", token->value);

        // Add the operator as a child node
        addChild(unaryNode, createParseTreeNode("Operator", token->value));

        // Parse the operand (recursively handle unary expressions)
        addChild(unaryNode, parseUnaryExpr());
    } else {
        // If no unary operator, parse as a postfix expression
        addChild(unaryNode, parsePostfixExpr());
    }

    return unaryNode;
}



ParseTreeNode* parsePostfixExpr() {
    printf("Parsing Postfix Expression...\n");

    // Create a parse tree node for the Postfix Expression
    ParseTreeNode* postfixNode = createParseTreeNode("PostfixExpr", "");

    // Parse the primary expression and add it as a child
    addChild(postfixNode, parsePrimaryExpr());

    // Handle optional postfix operators (e.g., function calls or array accesses)
    Token* token = peekToken();
    while (token && (strcmp(token->type, "L_PAREN") == 0 || strcmp(token->type, "L_BRACKET") == 0)) {
        if (strcmp(token->type, "L_PAREN") == 0) {
            printf("Parsing Function Call...\n");

            // Create a node for the function call
            ParseTreeNode* funcCallNode = createParseTreeNode("FunctionCall", "");

            getNextToken(); // Consume '('

            // Parse the argument list and add it as a child
            addChild(funcCallNode, parseArgumentList());

            token = getNextToken();
            if (!token || strcmp(token->type, "R_PAREN") != 0) {
                syntaxError("Expected ')' after function arguments");
            }

            // Add the function call node as a child of the postfix node
            addChild(postfixNode, funcCallNode);

        } else if (strcmp(token->type, "L_BRACKET") == 0) {
            printf("Parsing Array Access...\n");

            // Create a node for the array access
            ParseTreeNode* arrayAccessNode = createParseTreeNode("ArrayAccess", "");

            getNextToken(); // Consume '['

            // Parse the index expression and add it as a child
            addChild(arrayAccessNode, parseExpression());

            token = getNextToken();
            if (!token || strcmp(token->type, "R_BRACKET") != 0) {
                syntaxError("Expected ']' after array index expression");
            }

            // Add the array access node as a child of the postfix node
            addChild(postfixNode, arrayAccessNode);
        }

        token = peekToken(); // Check for more postfix operations
    }

    return postfixNode;
}



ParseTreeNode* parseLiteral() {
    printf("Parsing Literal...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing literal.");
    }

    // Create a node for the literal
    ParseTreeNode* literalNode = createParseTreeNode("Literal", "");

    if (strcmp(token->type, "INT_LITERAL") == 0 ||
        strcmp(token->type, "FLOAT_LITERAL") == 0 ||
        strcmp(token->type, "CHAR_LITERAL") == 0 ||
        strcmp(token->type, "STRING_LITERAL") == 0) {
        printf("Literal detected: %s\n", token->value);

        // Add the literal value to the node
        setNodeValue(literalNode, token->value);

        getNextToken(); // Consume the literal token
    } else if (strcmp(token->type, "Keyword") == 0 &&
               (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        printf("Boolean literal detected: %s\n", token->value);

        // Add the boolean value to the node
        setNodeValue(literalNode, token->value);

        getNextToken(); // Consume the boolean literal
    } else {
        syntaxError("Expected a literal (integer, float, char, string, or boolean).");
    }

    return literalNode;
}



ParseTreeNode* parseBoolLiteral() {
    printf("Parsing Boolean Literal...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing boolean literal.");
    }

    // Create a node for the boolean literal
    ParseTreeNode* boolLiteralNode = createParseTreeNode("BoolLiteral", "");

    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        printf("Boolean literal detected: %s\n", token->value);

        // Add the boolean value to the node
        setNodeValue(boolLiteralNode, token->value);

        getNextToken(); // Consume the boolean literal token
    } else {
        syntaxError("Expected a boolean literal ('true' or 'false').");
    }

    return boolLiteralNode;
}



ParseTreeNode* parsePrimaryExpr() {
    printf("Parsing Primary Expression...\n");

    Token* token = getNextToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing a primary expression");
    }

    // Create a node for the primary expression
    ParseTreeNode* primaryNode = createParseTreeNode("PrimaryExpr", "");

    if (strcmp(token->type, "INT_LITERAL") == 0 || 
        strcmp(token->type, "FLOAT_LITERAL") == 0 || 
        strcmp(token->type, "CHAR_LITERAL") == 0 || 
        strcmp(token->type, "STRING_LITERAL") == 0 || 
        (strcmp(token->type, "Keyword") == 0 && 
         (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0))) {
        // It's a literal
        printf("Literal detected: %s\n", token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // It's an identifier
        printf("Identifier detected: %s\n", token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "L_PAREN") == 0) {
        // It's a grouped expression
        addChild(primaryNode, parseExpression()); // Parse the inner expression

        token = getNextToken();
        if (!token || strcmp(token->type, "R_PAREN") != 0) {
            syntaxError("Expected ')' to close the grouped expression");
        }
    } else {
        syntaxError("Expected a literal, identifier, or grouped expression");
    }

    return primaryNode;
}



ParseTreeNode* parseIfStatement() {
    printf("Parsing If Statement...\n");

    // Create a node for the if statement
    ParseTreeNode* ifNode = createParseTreeNode("IfStatement", "");

    // Match the 'if' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "if") != 0) {
        syntaxError("Expected 'if' keyword.");
    }
    addChild(ifNode, createParseTreeNode("Keyword", "if"));

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'if'.");
    }

    // Parse the condition inside the parentheses
    addChild(ifNode, parseExpression());

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after condition.");
    }

    // Parse the statement block
    addChild(ifNode, parseStatement());

    // Check for optional 'else' or 'else if'
    token = peekToken();
    if (token && strcmp(token->value, "else") == 0) {
        getNextToken(); // Consume 'else'
        ParseTreeNode* elseNode = createParseTreeNode("ElseClause", "");

        token = peekToken();
        if (token && strcmp(token->value, "if") == 0) {
            // Parse an 'else if' statement
            addChild(elseNode, parseIfStatement());
        } else {
            // Parse the 'else' statement block
            addChild(elseNode, parseStatement());
        }

        addChild(ifNode, elseNode);
    }

    return ifNode;
}



ParseTreeNode* parseCaseStatement() {
    printf("Parsing Case Statement...\n");

    // Create a node for the case statement
    ParseTreeNode* caseNode = createParseTreeNode("CaseStatement", "");

    // Match the 'case' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "case") != 0) {
        syntaxError("Expected 'case' keyword.");
    }
    addChild(caseNode, createParseTreeNode("Keyword", "case"));

    // Match the case value (INT_LITERAL or CHAR_LITERAL)
    token = getNextToken();
    if (!token || !(strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "CHAR_LITERAL") == 0)) {
        syntaxError("Expected a literal value after 'case'.");
    }
    addChild(caseNode, createParseTreeNode("Literal", token->value));

    // Match the colon ':'
    token = getNextToken();
    if (!token || strcmp(token->type, "COLON") != 0) {
        syntaxError("Expected ':' after case value.");
    }
    addChild(caseNode, createParseTreeNode("Symbol", ":"));

    // Parse the statements inside the case
    while (peekToken() && strcmp(peekToken()->value, "break") != 0) {
        addChild(caseNode, parseStatement());
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || strcmp(token->value, "break") != 0) {
        syntaxError("Expected 'break' in case statement.");
    }
    addChild(caseNode, createParseTreeNode("Keyword", "break"));

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after 'break'.");
    }
    addChild(caseNode, createParseTreeNode("Symbol", ";"));

    return caseNode;
}



ParseTreeNode* parseDefaultCase() {
    printf("Parsing Default Case...\n");

    // Create a node for the default case
    ParseTreeNode* defaultNode = createParseTreeNode("DefaultCase", "");

    // Match the 'default' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "default") != 0) {
        syntaxError("Expected 'default' keyword.");
    }
    addChild(defaultNode, createParseTreeNode("Keyword", "default"));

    // Match the colon ':'
    token = getNextToken();
    if (!token || strcmp(token->type, "COLON") != 0) {
        syntaxError("Expected ':' after 'default'.");
    }
    addChild(defaultNode, createParseTreeNode("Symbol", ":"));

    // Parse the statements inside the default case
    while (peekToken() && strcmp(peekToken()->value, "break") != 0) {
        addChild(defaultNode, parseStatement());
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || strcmp(token->value, "break") != 0) {
        syntaxError("Expected 'break' in default case.");
    }
    addChild(defaultNode, createParseTreeNode("Keyword", "break"));

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after 'break'.");
    }
    addChild(defaultNode, createParseTreeNode("Symbol", ";"));

    return defaultNode;
}


ParseTreeNode* parseForInit() {
    printf("Parsing For Loop Initialization...\n");

    // Create a node for the for loop initialization
    ParseTreeNode* forInitNode = createParseTreeNode("ForInit", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop initialization.");
    }

    if (strcmp(token->type, "Keyword") == 0) {
        // Variable declaration
        addChild(forInitNode, parseVariableDeclaration());
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Assignment statement
        addChild(forInitNode, parseAssignmentStatement());
    } else {
        syntaxError("Invalid initialization in 'for' loop.");
    }

    return forInitNode;
}



ParseTreeNode* parseStatementBlock() {
    printf("Parsing Statement Block...\n");

    // Create a node for the statement block
    ParseTreeNode* statementBlockNode = createParseTreeNode("StatementBlock", "");

    Token* token = peekToken();
    if (token && strcmp(token->type, "L_CURLY") == 0) {
        // Add the block to the statement block node
        addChild(statementBlockNode, parseBlock());
    } else {
        // Add the single statement to the statement block node
        addChild(statementBlockNode, parseStatement());
    }

    return statementBlockNode;
}



ParseTreeNode* parseForLoop() {
    printf("Parsing For Loop...\n");

    // Create a parse tree node for the for loop
    ParseTreeNode* forNode = createParseTreeNode("ForLoop", "");

    // Match the 'for' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "for") != 0) {
        syntaxError("Expected 'for' keyword.");
    }
    addChild(forNode, createParseTreeNode("Keyword", token->value));

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'for'.");
    }
    addChild(forNode, createParseTreeNode("Symbol", token->value));

    // Parse the optional initialization statement
    if (peekToken() && strcmp(peekToken()->type, "SEMICOLON") != 0) {
        addChild(forNode, parseForInit());
    }

    // Match the first semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after initialization in 'for' loop.");
    }
    addChild(forNode, createParseTreeNode("Symbol", token->value));

    // Parse the optional condition
    if (peekToken() && strcmp(peekToken()->type, "SEMICOLON") != 0) {
        addChild(forNode, parseExpression());
    }

    // Match the second semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after condition in 'for' loop.");
    }
    addChild(forNode, createParseTreeNode("Symbol", token->value));

    // Parse the optional update statement
    if (peekToken() && strcmp(peekToken()->type, "R_PAREN") != 0) {
        addChild(forNode, parseForUpdate());
    }

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after 'for' loop update.");
    }
    addChild(forNode, createParseTreeNode("Symbol", token->value));

    // Parse the loop body
    addChild(forNode, parseStatementBlock());

    return forNode;
}

ParseTreeNode* parseIncDec() {
    printf("Parsing Increment/Decrement...\n");

    // Create a parse tree node for increment/decrement
    ParseTreeNode* incDecNode = createParseTreeNode("IncDec", "");

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in increment/decrement statement.");
    }
    addChild(incDecNode, createParseTreeNode("Identifier", token->value));

    token = getNextToken();
    if (!token || !(strcmp(token->type, "UNARY_INC") == 0 || strcmp(token->type, "UNARY_DEC") == 0)) {
        syntaxError("Expected '++' or '--' after identifier.");
    }
    addChild(incDecNode, createParseTreeNode("Operator", token->value));

    return incDecNode;
}

ParseTreeNode* parseForUpdate() {
    printf("Parsing For Loop Update...\n");

    // Create a node for the For Update
    ParseTreeNode* forUpdateNode = createParseTreeNode("ForUpdate", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop update.");
    }

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        Token* nextToken = peekToken();
        if (nextToken && (strcmp(nextToken->type, "ASSIGNMENT_OP") == 0 ||
                          strcmp(nextToken->type, "ADD_ASSIGNOP") == 0 ||
                          strcmp(nextToken->type, "SUB_ASSIGNOP") == 0)) {
            // Parse an assignment statement and add it as a child
            addChild(forUpdateNode, parseAssignmentStatement());
        } else if (nextToken && (strcmp(nextToken->type, "UNARY_INC") == 0 ||
                                 strcmp(nextToken->type, "UNARY_DEC") == 0)) {
            // Parse an increment/decrement statement and add it as a child
            addChild(forUpdateNode, parseIncDec());
        } else {
            syntaxError("Invalid update in 'for' loop.");
        }
    } else {
        syntaxError("Expected an identifier in 'for' loop update.");
    }

    return forUpdateNode;
}



ParseTreeNode* parseWhileLoop() {
    printf("Parsing While Loop...\n");

    // Create a parse tree node for the while loop
    ParseTreeNode* whileNode = createParseTreeNode("WhileLoop", "");

    // Match "while"
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword");
    }
    addChild(whileNode, createParseTreeNode("Keyword", token->value));

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'while'");
    }
    addChild(whileNode, createParseTreeNode("Symbol", token->value));

    // Parse the condition expression
    addChild(whileNode, parseExpression());

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after condition expression in 'while' loop");
    }
    addChild(whileNode, createParseTreeNode("Symbol", token->value));

    // Parse the statement block
    addChild(whileNode, parseStatementBlock());

    return whileNode;
}



ParseTreeNode* parseDoWhileLoop() {
    printf("Parsing Do-While Loop...\n");

    // Create a parse tree node for the do-while loop
    ParseTreeNode* doWhileNode = createParseTreeNode("DoWhileLoop", "");

    // Match "do"
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "do") != 0) {
        syntaxError("Expected 'do' keyword");
    }
    addChild(doWhileNode, createParseTreeNode("Keyword", token->value));

    // Parse the statement block
    addChild(doWhileNode, parseStatementBlock());

    // Match "while"
    token = getNextToken();
    if (!token || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword after 'do' block");
    }
    addChild(doWhileNode, createParseTreeNode("Keyword", token->value));

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'while' in do-while loop");
    }
    addChild(doWhileNode, createParseTreeNode("Symbol", token->value));

    // Parse the condition expression
    addChild(doWhileNode, parseExpression());

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after condition expression in 'do-while' loop");
    }
    addChild(doWhileNode, createParseTreeNode("Symbol", token->value));

    // Match ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of 'do-while' loop");
    }
    addChild(doWhileNode, createParseTreeNode("Symbol", token->value));

    return doWhileNode;
}


ParseTreeNode* parseArrayDimensions() {
    printf("Parsing Array Dimensions...\n");

    // Create a parse tree node for array dimensions
    ParseTreeNode* dimensionsNode = createParseTreeNode("ArrayDimensions", "");

    Token* token = peekToken();
    while (token && strcmp(token->type, "L_BRACKET") == 0) {
        // Match '['
        getNextToken(); // Consume '['
        addChild(dimensionsNode, createParseTreeNode("Symbol", "["));

        // Parse the dimension expression
        addChild(dimensionsNode, parseExpression());

        // Match ']'
        token = getNextToken();
        if (!token || strcmp(token->type, "R_BRACKET") != 0) {
            syntaxError("Expected ']' after array dimension expression");
        }
        addChild(dimensionsNode, createParseTreeNode("Symbol", "]"));

        token = peekToken();
    }

    return dimensionsNode;
}



ParseTreeNode* parseArrayType() {
    printf("Parsing Array Type...\n");

    // Create a parse tree node for array type
    ParseTreeNode* arrayTypeNode = createParseTreeNode("ArrayType", "");

    // Match the type specifier (e.g., "int", "float", etc.)
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0)) {
        syntaxError("Expected a valid type specifier (e.g., 'int', 'float').");
    }

    // Add the matched type to the parse tree
    addChild(arrayTypeNode, createParseTreeNode("Type", token->value));

    return arrayTypeNode;
}



ParseTreeNode* parseArrayInitializer() {
    printf("Parsing Array Initializer...\n");

    // Create a parse tree node for the array initializer
    ParseTreeNode* initializerNode = createParseTreeNode("ArrayInitializer", "");

    // Match the opening curly brace '{'
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "L_CURLY") != 0) {
        syntaxError("Expected '{' to start array initializer");
    }
    addChild(initializerNode, createParseTreeNode("Symbol", token->value));

    // Parse initializer values
    do {
        ParseTreeNode* exprNode = parseExpression(); // Parse the initializer value
        addChild(initializerNode, exprNode);

        token = peekToken();
        if (token && strcmp(token->type, "COMMA") == 0) {
            getNextToken(); // Consume ','
            addChild(initializerNode, createParseTreeNode("Symbol", token->value));
        }
    } while (token && strcmp(token->type, "R_CURLY") != 0);

    // Match the closing curly brace '}'
    token = getNextToken();
    if (!token || strcmp(token->type, "R_CURLY") != 0) {
        syntaxError("Expected '}' to end array initializer");
    }
    addChild(initializerNode, createParseTreeNode("Symbol", token->value));

    return initializerNode;
}



ParseTreeNode* parseArrayDeclaration() {
    printf("Parsing Array Declaration...\n");

    // Create a parse tree node for the array declaration
    ParseTreeNode* arrayDeclNode = createParseTreeNode("ArrayDeclaration", "");

    // Match the "array" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "array") != 0) {
        syntaxError("Expected 'array' keyword for array declaration");
    }
    addChild(arrayDeclNode, createParseTreeNode("Keyword", token->value));

    // Parse the array type
    ParseTreeNode* typeNode = parseArrayType();
    addChild(arrayDeclNode, typeNode);

    // Match the identifier (array name)
    token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for the array name");
    }
    addChild(arrayDeclNode, createParseTreeNode("Identifier", token->value));

    // Parse the array dimensions
    ParseTreeNode* dimensionsNode = parseArrayDimensions();
    addChild(arrayDeclNode, dimensionsNode);

    // Check for optional assignment
    token = peekToken();
    if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        getNextToken(); // Consume '='
        addChild(arrayDeclNode, createParseTreeNode("Symbol", token->value));

        // Parse the initializer
        ParseTreeNode* initializerNode = parseArrayInitializer();
        addChild(arrayDeclNode, initializerNode);
    }

    // Match semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of the array declaration");
    }
    addChild(arrayDeclNode, createParseTreeNode("Symbol", token->value));

    return arrayDeclNode;
}



ParseTreeNode* parseArrayOperations() {
    printf("Parsing Array Operations...\n");

    // Create a parse tree node for array operations
    ParseTreeNode* arrayOpsNode = createParseTreeNode("ArrayOperations", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing array operations");
    }

    // Determine the type of array operation
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        token = getNextToken(); // Consume the identifier
        addChild(arrayOpsNode, createParseTreeNode("Identifier", token->value));

        // Check if it's an array access operation
        token = peekToken();
        if (token && strcmp(token->type, "L_BRACKET") == 0) {
            ParseTreeNode* accessNode = parseArrayAccess();
            addChild(arrayOpsNode, accessNode);
        } else if (token && strcmp(token->type, "ASSIGNMENT_OP") == 0) {
            // Check if it's an array assignment operation
            token = getNextToken(); // Consume the '='
            addChild(arrayOpsNode, createParseTreeNode("AssignmentOperator", token->value));

            // Parse the expression to be assigned
            ParseTreeNode* exprNode = parseExpression();
            addChild(arrayOpsNode, exprNode);

            // Match the semicolon
            token = getNextToken();
            if (!token || strcmp(token->type, "SEMICOLON") != 0) {
                syntaxError("Expected ';' at the end of array assignment");
            }
            addChild(arrayOpsNode, createParseTreeNode("Symbol", token->value));
        } else {
            syntaxError("Invalid array operation");
        }
    } else {
        syntaxError("Expected an identifier for array operations");
    }

    return arrayOpsNode;
}



ParseTreeNode* parseArrayAccess() {
    printf("Parsing Array Access...\n");

    // Create a parse tree node for array access
    ParseTreeNode* arrayAccessNode = createParseTreeNode("ArrayAccess", "");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for array access");
    }

    token = getNextToken(); // Consume the identifier
    addChild(arrayAccessNode, createParseTreeNode("Identifier", token->value));

    // Parse array indices enclosed in brackets
    while ((token = peekToken()) && strcmp(token->type, "L_BRACKET") == 0) {
        token = getNextToken(); // Consume '['
        addChild(arrayAccessNode, createParseTreeNode("Symbol", "["));

        // Parse the index expression
        ParseTreeNode* indexExprNode = parseExpression();
        addChild(arrayAccessNode, indexExprNode);

        token = getNextToken(); // Consume ']'
        if (!token || strcmp(token->type, "R_BRACKET") != 0) {
            syntaxError("Expected ']' after array index expression");
        }
        addChild(arrayAccessNode, createParseTreeNode("Symbol", "]"));
    }

    // Check for valid termination of array access
    token = peekToken();
    if (!token || (strcmp(token->type, "ASSIGNMENT_OP") != 0 && strcmp(token->type, "SEMICOLON") != 0)) {
        syntaxError("Invalid token following array access");
    }

    if (strcmp(token->type, "ASSIGNMENT_OP") == 0) {
        token = getNextToken(); // Consume '='
        addChild(arrayAccessNode, createParseTreeNode("AssignmentOperator", token->value));

        // Parse the value to be assigned
        ParseTreeNode* valueNode = parseExpression();
        addChild(arrayAccessNode, valueNode);

        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after array assignment");
        }
        addChild(arrayAccessNode, createParseTreeNode("Symbol", ";"));
    } else if (strcmp(token->type, "SEMICOLON") == 0) {
        token = getNextToken(); // Consume the semicolon
        addChild(arrayAccessNode, createParseTreeNode("Symbol", ";"));
    }

    return arrayAccessNode;
}


ParseTreeNode* parseParameter() {
    printf("Parsing Parameter...\n");

    // Create a parse tree node for the parameter
    ParseTreeNode* parameterNode = createParseTreeNode("Parameter", "");

    // Parse the type specifier
    ParseTreeNode* typeNode = parseTypeSpecifier();
    addChild(parameterNode, typeNode);

    // Match the identifier
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for parameter");
    }
    addChild(parameterNode, createParseTreeNode("Identifier", token->value));

    return parameterNode;
}



ParseTreeNode* parseFunctionDeclaration() {
    printf("Parsing Function Declaration...\n");

    // Create a parse tree node for the function declaration
    ParseTreeNode* functionNode = createParseTreeNode("FunctionDeclaration", "");

    // Parse the return type
    ParseTreeNode* returnTypeNode = parseTypeSpecifier();
    addChild(functionNode, returnTypeNode);

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for function name");
    }
    addChild(functionNode, createParseTreeNode("Identifier", token->value));

    // Expect '(' to start the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after function name");
    }
    addChild(functionNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the parameter list
    token = peekToken();
    if (token && strcmp(token->type, "R_PAREN") != 0) {
        ParseTreeNode* parameterListNode = parseParameterList(); // Parse the parameters if any
        addChild(functionNode, parameterListNode);
    }

    // Expect ')' to end the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after parameter list");
    }
    addChild(functionNode, createParseTreeNode("RightParenthesis", ")"));

    // Check for a semicolon or a compound statement
    token = peekToken();
    if (token && strcmp(token->type, "SEMICOLON") == 0) {
        getNextToken(); // Consume the semicolon (function declaration only)
        addChild(functionNode, createParseTreeNode("Semicolon", ";"));
    } else {
        ParseTreeNode* blockNode = parseBlock(); // Parse the function body (compound statement)
        addChild(functionNode, blockNode);
    }

    return functionNode;
}



ParseTreeNode* parseFunctionCall() {
    printf("Parsing Function Call...\n");

    // Create a parse tree node for the function call
    ParseTreeNode* functionCallNode = createParseTreeNode("FunctionCall", "");

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for function name");
    }
    addChild(functionCallNode, createParseTreeNode("FunctionName", token->value));

    // Expect '(' to start the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after function name");
    }
    addChild(functionCallNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the argument list if there are any
    token = peekToken();
    if (token && strcmp(token->type, "R_PAREN") != 0) {
        ParseTreeNode* argumentListNode = parseArgumentList();
        addChild(functionCallNode, argumentListNode);
    }

    // Expect ')' to end the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' after argument list");
    }
    addChild(functionCallNode, createParseTreeNode("RightParenthesis", ")"));

    // Expect a semicolon to end the function call
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' after function call");
    }
    addChild(functionCallNode, createParseTreeNode("Semicolon", ";"));

    return functionCallNode;
}



ParseTreeNode* parseParameterList() {
    printf("Parsing Parameter List...\n");

    // Create a parse tree node for the parameter list
    ParseTreeNode* parameterListNode = createParseTreeNode("ParameterList", "");

    // Parse the first parameter
    ParseTreeNode* firstParameterNode = parseParameter();
    addChild(parameterListNode, firstParameterNode);

    // Check for additional parameters separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "COMMA") == 0) {
        getNextToken(); // Consume the comma
        addChild(parameterListNode, createParseTreeNode("Comma", ",")); // Add the comma to the tree

        ParseTreeNode* nextParameterNode = parseParameter(); // Parse the next parameter
        addChild(parameterListNode, nextParameterNode);

        token = peekToken(); // Update the token to check for more commas
    }

    return parameterListNode;
}



ParseTreeNode* parseArgumentList() {
    printf("Parsing Argument List...\n");

    // Create a parse tree node for the argument list
    ParseTreeNode* argumentListNode = createParseTreeNode("ArgumentList", "");

    // Parse the first argument
    ParseTreeNode* firstArgumentNode = parseExpression(); // Arguments are typically expressions
    addChild(argumentListNode, firstArgumentNode);

    // Check for additional arguments separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "COMMA") == 0) {
        getNextToken(); // Consume the comma
        addChild(argumentListNode, createParseTreeNode("Comma", ",")); // Add the comma to the tree

        ParseTreeNode* nextArgumentNode = parseExpression(); // Parse the next argument
        addChild(argumentListNode, nextArgumentNode);

        token = peekToken(); // Update the token to check for more commas
    }

    return argumentListNode;
}



ParseTreeNode* parseVariableList() {
    printf("Parsing Variable List...\n");

    // Create a parse tree node for the variable list
    ParseTreeNode* variableListNode = createParseTreeNode("VariableList", "");

    // Parse the first variable (identifier)
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in variable list");
    }
    addChild(variableListNode, createParseTreeNode("Identifier", token->value));

    // Check for additional variables separated by commas
    while (peekToken() && strcmp(peekToken()->type, "COMMA") == 0) {
        getNextToken(); // Consume ',' and add it to the tree
        addChild(variableListNode, createParseTreeNode("Comma", ","));

        token = getNextToken(); // Parse the next variable (identifier)
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            syntaxError("Expected an identifier after ',' in variable list");
        }
        addChild(variableListNode, createParseTreeNode("Identifier", token->value));
    }

    return variableListNode;
}



ParseTreeNode* parseAssignmentStatement() {
    printf("Parsing Assignment Statement...\n");

    // Create a parse tree node for the assignment statement
    ParseTreeNode* assignmentNode = createParseTreeNode("AssignmentStatement", "");

    // Parse the identifier at the start of the assignment
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier in assignment statement.");
    }
    addChild(assignmentNode, createParseTreeNode("Identifier", token->value));

    // Parse the assignment operator
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
    addChild(assignmentNode, createParseTreeNode("AssignmentOperator", token->value));

    // Parse either a variable list or an expression
    token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in assignment statement.");
    }

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Handle variable list
        addChild(assignmentNode, parseVariableList());
    } else {
        // Handle expression or literal
        addChild(assignmentNode, parseExpression());
    }

    // Parse the semicolon to terminate the assignment statement
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of assignment statement.");
    }
    addChild(assignmentNode, createParseTreeNode("Semicolon", ";"));

    return assignmentNode;
}



ParseTreeNode* parseInputStatement() {
    printf("Parsing Input Statement...\n");

    // Create a parse tree node for the input statement
    ParseTreeNode* inputNode = createParseTreeNode("InputStatement", "");

    // Parse the "input" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->value, "input") != 0) {
        syntaxError("Expected 'input' keyword at the start of an input statement.");
    }
    addChild(inputNode, createParseTreeNode("Keyword", "input"));

    // Parse the opening parenthesis
    token = getNextToken();
    if (!token || strcmp(token->type, "L_PAREN") != 0) {
        syntaxError("Expected '(' after 'input'.");
    }
    addChild(inputNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the variable list
    addChild(inputNode, parseVariableList());

    // Parse the closing parenthesis
    token = getNextToken();
    if (!token || strcmp(token->type, "R_PAREN") != 0) {
        syntaxError("Expected ')' to close the input statement.");
    }
    addChild(inputNode, createParseTreeNode("RightParenthesis", ")"));

    // Parse the semicolon to terminate the input statement
    token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of input statement.");
    }
    addChild(inputNode, createParseTreeNode("Semicolon", ";"));

    return inputNode;
}



ParseTreeNode* parseOutputList() {
    printf("Parsing Output List...\n");

    // Create a parse tree node for the output list
    ParseTreeNode* outputListNode = createParseTreeNode("OutputList", "");

    // Parse the first expression
    addChild(outputListNode, parseExpression());

    // Parse subsequent expressions separated by commas
    while (peekToken() && strcmp(peekToken()->type, "COMMA") == 0) {
        getNextToken(); // Consume ','
        addChild(outputListNode, createParseTreeNode("Comma", ","));
        addChild(outputListNode, parseExpression());
    }

    return outputListNode;
}



ParseTreeNode* parseOutputStatement() {
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


ParseTreeNode* parseSwitchStatement() {
    printf("Parsing Switch Statement...\n");

    // Match the 'switch' keyword
    matchToken("switch");

    // Match the opening parenthesis '('
    matchToken("L_PAREN");

    // Parse the controlling expression
    parseExpression();

    // Match the closing parenthesis ')'
    matchToken("R_PAREN");

    // Match the opening curly brace '{'
    matchToken("L_CURLY");

    // Parse case statements
    while (peekToken() && strcmp(peekToken()->type, "case") == 0) {
        parseCaseStatement();
    }

    // Optionally parse the default case
    if (peekToken() && strcmp(peekToken()->type, "default") == 0) {
        parseDefaultCase();
    }

    // Match the closing curly brace '}'
    matchToken("R_CURLY");
}




ParseTreeNode* parseIterativeStatement() {
    printf("Parsing Iterative Statement...\n");

    // Create a parse tree node for the iterative statement
    ParseTreeNode* iterativeStatementNode = createParseTreeNode("IterativeStatement", "");

    // Peek at the token to decide which iterative structure to parse
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing an iterative statement.");
    }

    if (strcmp(token->value, "for") == 0) {
        addChild(iterativeStatementNode, parseForLoop());
    } else if (strcmp(token->value, "while") == 0) {
        addChild(iterativeStatementNode, parseWhileLoop());
    } else if (strcmp(token->value, "do") == 0) {
        addChild(iterativeStatementNode, parseDoWhileLoop());
    } else {
        syntaxError("Expected 'for', 'while', or 'do' at the start of an iterative statement.");
    }

    return iterativeStatementNode;
}



ParseTreeNode* parseExpressionStatement() {
    printf("Parsing Expression Statement...\n");

    // Create a parse tree node for the expression statement
    ParseTreeNode* expressionStatementNode = createParseTreeNode("ExpressionStatement", "");

    // Parse the expression and add it as a child
    addChild(expressionStatementNode, parseExpression());

    // Expect a semicolon to terminate the expression statement
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "SEMICOLON") != 0) {
        syntaxError("Expected ';' at the end of an expression statement.");
    }

    // Add the semicolon as a child node
    addChild(expressionStatementNode, createParseTreeNode("SEMICOLON", token->value));

    return expressionStatementNode;
}



ParseTreeNode* parseJumpStatement() {
    printf("Parsing Jump Statement...\n");

    // Create a parse tree node for the jump statement
    ParseTreeNode* jumpStatementNode = createParseTreeNode("JumpStatement", "");

    Token* token = getNextToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing jump statement.");
    }

    if (strcmp(token->value, "return") == 0) {
        // Add 'return' keyword as a child node
        addChild(jumpStatementNode, createParseTreeNode("RETURN", token->value));

        // Check if there's an expression to return
        if (peekToken() && (strcmp(peekToken()->type, "IDENTIFIER") == 0 ||
                            strcmp(peekToken()->type, "INT_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "FLOAT_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "CHAR_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "STRING_LITERAL") == 0)) {
            addChild(jumpStatementNode, parseExpression());
        }

        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after return statement.");
        }

        // Add semicolon as a child node
        addChild(jumpStatementNode, createParseTreeNode("SEMICOLON", token->value));

    } else if (strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
        // Add 'break' or 'continue' keyword as a child node
        addChild(jumpStatementNode, createParseTreeNode(token->value, token->value));

        token = getNextToken();
        if (!token || strcmp(token->type, "SEMICOLON") != 0) {
            syntaxError("Expected ';' after break/continue statement.");
        }

        // Add semicolon as a child node
        addChild(jumpStatementNode, createParseTreeNode("SEMICOLON", token->value));

    } else {
        syntaxError("Unrecognized jump statement. Expected 'return', 'break', or 'continue'.");
    }

    return jumpStatementNode;
}


// Placeholder implementations
ParseTreeNode* parseMainFunction() {
    printf("Parsing Main Function...\n");

    // Create the main function node
    ParseTreeNode* mainNode = createParseTreeNode("MainFunction", "");

    // Match "void" keyword
    if (strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "void") == 0) {
        addChild(mainNode, createParseTreeNode("Keyword", "void"));
        matchToken("Keyword");
    } else {
        printf("Syntax Error: Expected 'void'.\n");
        return mainNode; // Return with whatever is parsed so far
    }

    // Match "main" keyword
    if (strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "main") == 0) {
        addChild(mainNode, createParseTreeNode("Keyword", "main"));
        matchToken("Keyword");
    } else {
        printf("Syntax Error: Expected 'main'.\n");
        return mainNode; // Return with whatever is parsed so far
    }

    // Match "("
    if (strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, "(") == 0) {
        addChild(mainNode, createParseTreeNode("Delimiter", "("));
        matchToken("Delimiter");
    } else {
        printf("Syntax Error: Expected '('.\n");
        return mainNode; // Return with whatever is parsed so far
    }

    // Match ")"
    if (strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ")") == 0) {
        addChild(mainNode, createParseTreeNode("Delimiter", ")"));
        matchToken("Delimiter");
    } else {
        printf("Syntax Error: Expected ')'.\n");
        return mainNode; // Return with whatever is parsed so far
    }

    // Match "{"
    if (strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, "{") == 0) {
        addChild(mainNode, createParseTreeNode("Delimiter", "{"));
        matchToken("Delimiter");
    } else {
        printf("Syntax Error: Expected '{'.\n");
        return mainNode; // Return with whatever is parsed so far
    }

    // Parse statements inside the block
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") != 0 && strcmp(peekToken()->value, "}") != 0) {
        addChild(mainNode, parseStatement());
    }

    // Match "}"
    if (strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, "}") == 0) {
        addChild(mainNode, createParseTreeNode("Delimiter", "}"));
        matchToken("Delimiter");
    } else {
        printf("Syntax Error: Expected '}'.\n");
    }

    return mainNode;
}


ParseTreeNode* parseConditionalStatement() {
    printf("Parsing Conditional Statement...\n");

    // Create the conditional statement node
    ParseTreeNode* conditionalNode = createParseTreeNode("ConditionalStatement", "");

    // Match "if"
    if (strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "if") == 0) {
        addChild(conditionalNode, createParseTreeNode("Keyword", "if"));
        matchToken("Keyword");
    } else {
        printf("Syntax Error: Expected 'if'.\n");
        return conditionalNode;
    }

    // Match "("
    if (strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, "(") == 0) {
        addChild(conditionalNode, createParseTreeNode("Delimiter", "("));
        matchToken("Delimiter");
    } else {
        printf("Syntax Error: Expected '('.\n");
        return conditionalNode;
    }

    // Parse the condition expression
    addChild(conditionalNode, parseExpression());

    // Match ")"
    if (strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ")") == 0) {
        addChild(conditionalNode, createParseTreeNode("Delimiter", ")"));
        matchToken("Delimiter");
    } else {
        printf("Syntax Error: Expected ')'.\n");
        return conditionalNode;
    }

    // Parse the statement block
    addChild(conditionalNode, parseStatementBlock());

    // Handle optional "else" or "else if"
    if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "else") == 0) {
        ParseTreeNode* elseNode = createParseTreeNode("ElseClause", "");
        addChild(conditionalNode, elseNode);
        matchToken("Keyword");

        // Check if it's an "else if" block
        if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "if") == 0) {
            addChild(elseNode, parseConditionalStatement());
        } else {
            // Otherwise, parse a regular "else" block
            addChild(elseNode, parseStatementBlock());
        }
    }

    return conditionalNode;
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
        if (strstr(entry->d_name, ".prsm") && strlen(entry->d_name) > 5) {
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
    const char* selectedFile = prsmFiles[choice - 1];
    printf("\nSelected file: %s\n", selectedFile);
    loadTokensFromFile(selectedFile);

    // Parse and build the parse tree
    ParseTreeNode* root = parseProgram();

    // Write the parse tree to a file
    FILE* parseTreeFile = fopen("parse_tree.txt", "w");
    if (!parseTreeFile) {
        printf("Error: Unable to create parse_tree.txt\n");
        freeParseTree(root);
        return 1;
    }

    writeParseTreeToFile(root, parseTreeFile);
    fclose(parseTreeFile);

    printf("\nParse tree written to parse_tree.txt\n");

    // Free the parse tree
    freeParseTree(root);

    printf("Syntax Analysis Completed Successfully!\n");
    return 0;
}




