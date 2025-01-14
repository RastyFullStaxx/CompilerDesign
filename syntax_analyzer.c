#include <stdio.h>     // Standard I/O operations
#include <stdlib.h>    // Standard library functions
#include <string.h>    // String operations
#include <ctype.h>     // Character classification and conversion
#include <wctype.h>    // Wide character functions
#include <dirent.h>    // Directory operations

#include "syntax_analyzer.h" // Custom syntax analyzer header
#include "token.h"           // Custom token header

#define MAX_TOKENS 1000 // Adjust size based on expected token count

// Global Variables - Store tokens and tracking variables
int currentTokenIndex = 0; // Tracks the current token index
int totalTokens = 0;       // Total number of tokens
Token* tokenStream = tokens; // Token array pointing to tokens array

// Function prototypes for parse tree handling
void writeParseTreeToFile(ParseTreeNode* node, FILE* file);
void freeParseTree(ParseTreeNode* node);
void syntaxError(const char* message);
Token* peekNextToken();











// ---------------------------------------
// Utility Functions of Syntax Analyzer
// ---------------------------------------

Token* getNextToken() {
    if (currentTokenIndex < totalTokens) {
        Token* token = &tokens[currentTokenIndex++];
        printf("DEBUG: getNextToken: Retrieved Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex - 1, token->type, token->value, token->lineNumber);
        return token;
    }
    printf("DEBUG: getNextToken: No more tokens. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
}

Token* peekToken() {
    if (currentTokenIndex < totalTokens) {
        printf("DEBUG: peekToken: Current Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex, tokens[currentTokenIndex].type, tokens[currentTokenIndex].value,
               tokens[currentTokenIndex].lineNumber);
        return &tokens[currentTokenIndex];
    }
    printf("DEBUG: peekToken: No more tokens. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
}


Token* peekNextToken() {
    if (currentTokenIndex + 1 < totalTokens) {
        return &tokens[currentTokenIndex + 1];
    }
    return NULL; // No next token
}



// Function to trim leading and trailing whitespace from a string
void trimWhitespace(char *str) {
    if (!str || !*str) return;

    // Trim leading whitespace
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    // Move trimmed string to the beginning
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}


// Token maper function = Match the tokens from the symbol table and load them using the loadTokenFromFile
void mapToken(Token* token) {
    if (strcmp(token->type, "Right Parenthesis") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ")");
    } else if (strcmp(token->type, "Left Parenthesis") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "(");
    } else if (strcmp(token->type, "Left Curly Brace") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "{");
    } else if (strcmp(token->type, "Right Curly Brace") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "}");
    } else if (strcmp(token->type, "Left Bracket") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "[");
    } else if (strcmp(token->type, "Right Bracket") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, "]");
    } else if (strcmp(token->type, "Semicolon") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ";");
    } else if (strcmp(token->type, "Colon") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ":");
    } else if (strcmp(token->type, "Comma") == 0) {
        strcpy(token->type, "Delimiter");
        strcpy(token->value, ",");
    } else if (strcmp(token->type, "Arithmetic Operator (Addition)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "+");
    } else if (strcmp(token->type, "Arithmetic Operator (Subtraction)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "-");
    } else if (strcmp(token->type, "Arithmetic Operator (Multiplication)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "*");
    } else if (strcmp(token->type, "Arithmetic Operator (Division)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "/");
    } else if (strcmp(token->type, "Arithmetic Operator (Modulo)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "%");
    } else if (strcmp(token->type, "Arithmetic Operator (Exponentiation)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "^");
    } else if (strcmp(token->type, "Arithmetic Operator (Integer Division)") == 0) {
        strcpy(token->type, "ArithmeticOperator");
        strcpy(token->value, "//");
    } else if (strcmp(token->type, "Assignment Operator (Simple)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "=");
    } else if (strcmp(token->type, "Assignment Operator (Add)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "+=");
    } else if (strcmp(token->type, "Assignment Operator (Subtract)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "-=");
    } else if (strcmp(token->type, "Assignment Operator (Multiply)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "*=");
    } else if (strcmp(token->type, "Assignment Operator (Divide)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "/=");
    } else if (strcmp(token->type, "Assignment Operator (Modulo)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "%=");
    } else if (strcmp(token->type, "Assignment Operator (Integer Division)") == 0) {
        strcpy(token->type, "AssignmentOperator");
        strcpy(token->value, "//=");
    } else if (strcmp(token->type, "Logical Operator (NOT)") == 0) {
        strcpy(token->type, "LogicalOperator");
        strcpy(token->value, "!");
    } else if (strcmp(token->type, "Logical Operator (OR)") == 0) {
        strcpy(token->type, "LogicalOperator");
        strcpy(token->value, "||");
    } else if (strcmp(token->type, "Logical Operator (AND)") == 0) {
        strcpy(token->type, "LogicalOperator");
        strcpy(token->value, "&&");
    } else if (strcmp(token->type, "Relational Operator (Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "==");
    } else if (strcmp(token->type, "Relational Operator (Not Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "!=");
    } else if (strcmp(token->type, "Relational Operator (Greater Than)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, ">");
    } else if (strcmp(token->type, "Relational Operator (Less Than)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "<");
    } else if (strcmp(token->type, "Relational Operator (Greater Than or Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, ">=");
    } else if (strcmp(token->type, "Relational Operator (Less Than or Equal To)") == 0) {
        strcpy(token->type, "RelationalOperator");
        strcpy(token->value, "<=");
    } else if (strcmp(token->type, "Integer Literal") == 0) {
        strcpy(token->type, "INT_LITERAL");
    } else if (strcmp(token->type, "Float Literal") == 0) {
        strcpy(token->type, "FLOAT_LITERAL");
    } else if (strcmp(token->type, "Character Literal") == 0) {
        strcpy(token->type, "CHAR_LITERAL");
    } else if (strcmp(token->type, "String Literal") == 0) {
        strcpy(token->type, "STRING_LITERAL");
    } else if (strcmp(token->type, "Unary Operator (Increment)") == 0) {
        strcpy(token->type, "UnaryOperator");
        strcpy(token->value, "++");
    } else if (strcmp(token->type, "Unary Operator (Decrement)") == 0) {
        strcpy(token->type, "UnaryOperator");
        strcpy(token->value, "--");
    } else if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        strcpy(token->type, "SpecifierIdentifier");
    } else if (strcmp(token->type, "Reserved Word") == 0) {
        strcpy(token->type, "ReservedWord");
    } else if (strcmp(token->type, "Noise Word") == 0) {
        strcpy(token->type, "NoiseWord");
    } else if (strstr(token->type, "Comment") != NULL) {
        strcpy(token->type, "Comment");
    }
}



// Function to load tokens from a file
int loadTokensFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }

    char line[256];
    totalTokens = 0;

    printf("\nLoading tokens from %s...\n", filename);

    while (fgets(line, sizeof(line), file)) {
        // Debug: Print each line read
        //printf("DEBUG: Processing line: %s", line);

        // Trim whitespace from the line
        trimWhitespace(line);

        // Skip empty lines
        if (strlen(line) == 0) {
            printf("DEBUG: Skipping empty line.\n");
            continue;
        }

        // Parse the line into token fields (comma-separated)
        Token token;
        char type[50] = {0}, value[50] = {0};
        int lineNumber = 0;

        // Extract fields using comma as a delimiter
        char *firstComma = strchr(line, ',');
        char *lastComma = strrchr(line, ',');

        if (!firstComma || !lastComma || firstComma == lastComma) {
            printf("DEBUG: Invalid or unparsed line: %s\n", line);
            continue; // Skip invalid lines
        }

        // Extract token type
        *firstComma = '\0';
        strncpy(type, line, sizeof(type) - 1);
        type[sizeof(type) - 1] = '\0';

        // Extract token value
        *lastComma = '\0';
        strncpy(value, firstComma + 1, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';

        // Extract line number
        lineNumber = atoi(lastComma + 1);

        // Populate the token
        strncpy(token.type, type, sizeof(token.type) - 1);
        token.type[sizeof(token.type) - 1] = '\0';
        strncpy(token.value, value, sizeof(token.value) - 1);
        token.value[sizeof(token.value) - 1] = '\0';
        token.lineNumber = lineNumber;

        // Debug: Print the extracted token fields
        //printf("DEBUG: Extracted Token: Type='%s', Value='%s', Line=%d\n", token.type, token.value, token.lineNumber);

        // Add the token to the list
        if (totalTokens < MAX_TOKENS) {
            tokens[totalTokens++] = token;
        } else {
            printf("Error: Token limit exceeded. Increase MAX_TOKENS.\n");
            break;
        }
    }

    fclose(file);

    // Update totalTokens and print all loaded tokens
    totalTokens = totalTokens;
    printf("DEBUG: Total Tokens Loaded: %d\n", totalTokens);
    for (int i = 0; i < totalTokens; i++) {
        printf("DEBUG: Token[%d]: Type='%s', Value='%s', Line=%d\n",
               i, tokens[i].type, tokens[i].value, tokens[i].lineNumber);
    }

    printf("\n\nDEBUG: Completed loading tokens. Total loaded: %d from %s.\n", totalTokens, filename);
    return totalTokens;

    printf("DEBUG: After loading tokens - currentTokenIndex=%d, totalTokens=%d\n\n\n",
       currentTokenIndex, totalTokens);

    
}


ParseTreeNode* matchToken(const char* expectedType, const char* expectedValue) {
    printf("DEBUG: Matching Token. Expected Type='%s', Value='%s'\n", expectedType, expectedValue);

    Token* token = peekToken(); // Peek the current token without advancing
    if (!token) {
        syntaxError("No more tokens available when matching.");
        return NULL;
    }

    // Check if the token matches the expected type and value
    if (strcmp(token->type, expectedType) != 0 || strcmp(token->value, expectedValue) != 0) {
        char errorMessage[200];
        snprintf(errorMessage, sizeof(errorMessage),
                 "Syntax Error at line %d: Expected '%s' but found '%s'.",
                 token->lineNumber, expectedValue, token->value);
        syntaxError(errorMessage);
        return NULL;
    }

    printf("DEBUG: Found Token. Type='%s', Value='%s', Line=%d\n", token->type, token->value, token->lineNumber);

    // Consume the token
    token = getNextToken();

    // Create a parse tree node for the matched token
    return createParseTreeNode(token->type, token->value);
}









































// ---------------------------------------
// Top-Level Grammar Rules
// ---------------------------------------

// Recursive Descent Parsing Function
ParseTreeNode* parseProgram() {
    printf("DEBUG: Starting parseProgram...\n");

    // Create the root node for the parse tree
    ParseTreeNode* root = createParseTreeNode("Program", NULL);

    // Peek at the first token
    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Program is empty or no tokens available");
        return NULL;
    }
    printf("DEBUG: First token in program: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Parse the main function
    ParseTreeNode* mainFunctionNode = parseMainFunction();
    if (!mainFunctionNode) {
        reportSyntaxError("Failed to parse main function");
        recoverFromError();
        return NULL;
    }
    addChild(root, mainFunctionNode);

    printf("DEBUG: Completed parseProgram.\n");
    return root;
}



ParseTreeNode* parseMainFunction() {
    printf("DEBUG: Parsing Main Function...\n");

    ParseTreeNode* mainFunctionNode = createParseTreeNode("MainFunction", NULL);

    // Match 'void' keyword
    if (!matchToken("Keyword", "void")) {
        reportSyntaxError("Expected 'void' keyword at the start of main function.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }
    addChild(mainFunctionNode, createParseTreeNode("Keyword", "void"));

    // Match 'main' identifier
    if (!matchToken("Keyword", "main")) {
        reportSyntaxError("Expected 'main' keyword after 'void'.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }
    addChild(mainFunctionNode, createParseTreeNode("IDENTIFIER", "main"));

    // Match '(' and ')'
    if (!matchToken("Delimiter", "(")) {
        reportSyntaxError("Expected '(' after 'main'.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", "("));

    if (!matchToken("Delimiter", ")")) {
        reportSyntaxError("Expected ')' after '('.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", ")"));

    // Match '{'
    if (!matchToken("Delimiter", "{")) {
        reportSyntaxError("Expected '{' to start the main function block.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }
    addChild(mainFunctionNode, createParseTreeNode("Delimiter", "{"));

    // Parse the block inside main function
    printf("DEBUG: Parsing block inside main function...\n");
    ParseTreeNode* blockNode = parseBlock();
    if (!blockNode) {
        reportSyntaxError("Failed to parse main function block.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }
    addChild(mainFunctionNode, blockNode);

    // Ensure the closing '}'
    Token* token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
        printf("DEBUG: Matched closing '}' for main function.\n");
        matchToken("Delimiter", "}");
        addChild(mainFunctionNode, createParseTreeNode("Delimiter", "}"));
    } else {
        reportSyntaxError("Expected closing '}' for main function.");
        recoverFromError();
        freeParseTree(mainFunctionNode);
        return NULL;
    }

    printf("DEBUG: Completed parsing Main Function.\n");
    return mainFunctionNode;
}



ParseTreeNode* parseBlock() {
    printf("\n\nParsing Block...\n");

    // Create the block node
    ParseTreeNode* blockNode = createParseTreeNode("Block", "");

    // Parse statements inside the block
    Token* token;
    printf("DEBUG: Parsing statements inside block...\n");
    while ((token = peekToken()) && 
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}"))) {
        printf("DEBUG: Parsing statement. Current Token: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);

        if (strcmp(token->type, "Comment") == 0) {
            // Parse comment
            ParseTreeNode* commentNode = parseComment();
            if (commentNode) {
                addChild(blockNode, commentNode);
            } else {
                reportSyntaxError("Error parsing comment inside block.");
                recoverFromError();
            }
            continue; // Move to the next token after parsing the comment
        }

        // Parse a single statement
        ParseTreeNode* statementNode = parseStatement();
        if (statementNode) {
            addChild(blockNode, statementNode);
        } else {
            reportSyntaxError("Error parsing statement inside block.");
            recoverFromError();
            break; // Exit loop on error
        }
    }

    // Match the closing curly brace '}'
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
        printf("DEBUG: Matched '}'.\n");
        matchToken("Delimiter", "}");
        addChild(blockNode, createParseTreeNode("Delimiter", "}"));
    } else {
        reportSyntaxError("Expected '}' at the end of block.");
        recoverFromError();
        freeParseTree(blockNode);
        return NULL;
    }

    printf("DEBUG: Completed parsing Block.\n");
    return blockNode;
}




ParseTreeNode* parseStatementList() {
    printf("\n\nParsing Statement List...\n");

    // Create a node for the statement list
    ParseTreeNode* statementListNode = createParseTreeNode("StatementList", "");

    Token* token = peekToken();
    while (token && 
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}")) && 
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";"))) {
        // Debug: Current token being parsed
        printf("DEBUG: Parsing statement in list. Current Token: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);

        // Parse each statement and add it as a child
        ParseTreeNode* statementNode = parseStatement();
        if (statementNode) {
            addChild(statementListNode, statementNode);
        } else {
            reportSyntaxError("Error parsing statement in statement list.");
            recoverFromError();
            break; // Exit the loop on error
        }

        token = peekToken(); // Update the token to check for more statements
    }

    printf("DEBUG: Completed parsing Statement List.\n");
    return statementListNode;
}

















// ---------------------------------------
// Comments
// ---------------------------------------

ParseTreeNode* parseComment() {
    printf("Parsing Comment...\n");

    // Create a parse tree node for the comment
    ParseTreeNode* commentNode = createParseTreeNode("Comment", "");

    // Get the next token to determine the comment type
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing comment.");
        return NULL;
    }

    if (strcmp(token->type, "Comment") == 0) {
        // Single-line or multi-line comments are both labeled as "Comment"
        getNextToken(); // Consume the comment token
        addChild(commentNode, createParseTreeNode("Comment", token->value));
        printf("DEBUG: Parsed Comment: %s\n", token->value);
    } else {
        syntaxError("Expected a comment token but found something else.");
        freeParseTree(commentNode);
        return NULL;
    }

    return commentNode;
}
















// ---------------------------------------
// Initializer and Lists
// ---------------------------------------

ParseTreeNode* parseInitializer() {

}



ParseTreeNode* parseInitializerList() {

}
















// ---------------------------------------
// Declaration Statements
// ---------------------------------------

ParseTreeNode* parseDeclarationStatement() {
    printf("\n\nParsing Declaration Statement...\n");

    // Create a node for the declaration statement
    ParseTreeNode* declarationNode = createParseTreeNode("DeclarationStatement", "");

    // Match a type-specifier (e.g., int, float, etc.)
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 ||
          strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "char") == 0 ||
          strcmp(token->value, "bool") == 0 ||
          strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a valid type-specifier (e.g., int, float, etc.).");
        free(declarationNode); // Cleanup memory
        return NULL;
    }

    // Add the type-specifier to the declaration node
    addChild(declarationNode, matchToken("Keyword", token->value));

    // Match an identifier
    token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier after the type-specifier.");
        free(declarationNode); // Cleanup memory
        return NULL;
    }

    // Add the identifier to the declaration node
    addChild(declarationNode, matchToken("IDENTIFIER", NULL));

    // Match a semicolon (;)
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after the declaration.");
        free(declarationNode); // Cleanup memory
        return NULL;
    }

    // Add the semicolon to the declaration node
    addChild(declarationNode, matchToken("Delimiter", ";"));

    printf("DEBUG: Completed parsing Declaration Statement.\n");
    return declarationNode;
}



ParseTreeNode* parseVariableDeclaration() {
    printf("\n\nParsing Variable Declaration...\n");

    // Create a node for the variable declaration
    ParseTreeNode* variableDeclarationNode = createParseTreeNode("VariableDeclaration", "");

    // Parse the type specifier and add it as a child
    addChild(variableDeclarationNode, parseTypeSpecifier());

    // Parse at least one declaration
    do {
        Token* token = getNextToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            syntaxError("Expected an identifier in variable declaration.");
            freeParseTree(variableDeclarationNode);
            return NULL;
        }

        // Add the identifier as a child
        printf("DEBUG: Parsed identifier in variable declaration: %s\n", token->value);
        addChild(variableDeclarationNode, createParseTreeNode("IDENTIFIER", token->value));

        // Check for optional assignment
        token = peekToken();
        if (token && strcmp(token->type, "AssignmentOperator") == 0) {
            getNextToken(); // Consume '='
            printf("DEBUG: Assignment operator detected.\n");

            // Parse the initializer and add it as a child
            ParseTreeNode* initializerNode = parseExpression();
            if (!initializerNode) {
                syntaxError("Invalid initializer in variable declaration.");
                freeParseTree(variableDeclarationNode);
                return NULL;
            }
            addChild(variableDeclarationNode, initializerNode);
        }

        // Peek for the next token to check for commas
        token = peekToken();
    } while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0 && getNextToken());

    // Ensure the statement ends with a semicolon
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of variable declaration.");
        freeParseTree(variableDeclarationNode);
        return NULL;
    }
    addChild(variableDeclarationNode, createParseTreeNode("Delimiter", ";"));

    printf("DEBUG: Completed parsing variable declaration.\n");
    return variableDeclarationNode;
}



ParseTreeNode* parseTypeSpecifier() {
    printf("\n\nParsing Type Specifier...\n");

    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
          strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a type specifier (int, float, char, bool, or string).");
    }

    // Debug: Log the matched type specifier
    printf("DEBUG: Matched Type Specifier: %s\n", token->value);

    // Create a parse tree node for the type specifier
    return createParseTreeNode("TypeSpecifier", token->value);
}
















// ---------------------------------------
// Statement Types
// ---------------------------------------

ParseTreeNode* parseStatement() {
    printf("\n\nParsing Statement...\n");
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing statement.");
        return NULL;
    }

    printf("DEBUG: Current Token in parseStatement: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Handle Input Statement
    if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "input") == 0) {
        return parseInputStatement();
    }

    // Handle Output Statement
    if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "printf") == 0) {
        return parseOutputStatement();
    }

    // Handle Declaration Statements
    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        return parseDeclarationStatement();
    }

    // Handle Function Declaration or Call
    if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "void") == 0) {
        return parseFunctionStatement();
    }
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Peek ahead to determine if it's a function call
        Token* nextToken = peekNextToken();
        if (nextToken && strcmp(nextToken->type, "Delimiter") == 0 && strcmp(nextToken->value, "(") == 0) {
            return parseFunctionCall();
        }
    }

    // Handle Expression Statements
    ParseTreeNode* exprNode = parseExpressionStatement();
    if (exprNode) {
        return exprNode;
    }

    syntaxError("Failed to parse statement.");
    return NULL;
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
    addChild(assignmentNode, createParseTreeNode("IDENTIFIER", token->value));

    // Parse the assignment operator
    token = getNextToken();
    if (!token || strcmp(token->type, "AssignmentOperator") != 0) {
        syntaxError("Expected an assignment operator.");
    }
    addChild(assignmentNode, createParseTreeNode("AssignmentOperator", token->value));

    // Parse either an expression or a literal
    token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in assignment statement.");
    }

    if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Handle variable list (if necessary in grammar rules)
        addChild(assignmentNode, parseVariableList());
    } else {
        // Handle expression or literal
        addChild(assignmentNode, parseExpression());
    }

    // Parse the semicolon to terminate the assignment statement
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of assignment statement.");
    }
    addChild(assignmentNode, createParseTreeNode("Semicolon", ";"));

    return assignmentNode;
}



ParseTreeNode* parseInputStatement() {
    printf("Parsing Input Statement...\n");

    ParseTreeNode* inputNode = createParseTreeNode("InputStatement", "");

    // Match and add the "input" keyword
    matchToken("Keyword", "input");
    addChild(inputNode, createParseTreeNode("Keyword", "input"));

    // Match and add the opening parenthesis "("
    matchToken("Delimiter", "(");
    addChild(inputNode, createParseTreeNode("Delimiter", "("));

    // Parse the format string
    Token* token = peekToken();
    if (token && strcmp(token->type, "STRING_LITERAL") == 0) {
        matchToken("STRING_LITERAL", token->value);
        addChild(inputNode, createParseTreeNode("FormatString", token->value));
    } else {
        syntaxError("Expected a format string in input statement");
        freeParseTree(inputNode); // Clean up on error
        return NULL;
    }

    // Match the comma
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        matchToken("Delimiter", ",");
        addChild(inputNode, createParseTreeNode("Delimiter", ","));
    } else {
        syntaxError("Expected a comma after format string in input statement");
        freeParseTree(inputNode); // Clean up on error
        return NULL;
    }

    // Parse the SpecifierIdentifier list
    while (1) {
        token = peekToken();
        if (token && strcmp(token->type, "SpecifierIdentifier") == 0) {
            matchToken("SpecifierIdentifier", token->value);
            addChild(inputNode, createParseTreeNode("SpecifierIdentifier", token->value));
        } else {
            break; // Exit loop when no more specifier identifiers are found
        }

        // Check for another comma
        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            matchToken("Delimiter", ",");
            addChild(inputNode, createParseTreeNode("Delimiter", ","));
        } else {
            break; // Exit loop if no comma found
        }
    }

    // Match and add the closing parenthesis ")"
    matchToken("Delimiter", ")");
    addChild(inputNode, createParseTreeNode("Delimiter", ")"));

    // Match and add the semicolon ";"
    matchToken("Delimiter", ";");
    addChild(inputNode, createParseTreeNode("Delimiter", ";"));

    return inputNode;
}



ParseTreeNode* parseOutputStatement() {
    printf("Parsing Output Statement...\n");

    // Create a parse tree node for the output statement
    ParseTreeNode* outputNode = createParseTreeNode("OutputStatement", "");

    // Match and add the "printf" keyword
    matchToken("Keyword", "printf");
    addChild(outputNode, createParseTreeNode("Keyword", "printf"));

    // Match and add the opening parenthesis "("
    matchToken("Delimiter", "(");
    addChild(outputNode, createParseTreeNode("Delimiter", "("));

    // Parse the output list
    addChild(outputNode, parseOutputList());

    // Match and add the closing parenthesis ")"
    matchToken("Delimiter", ")");
    addChild(outputNode, createParseTreeNode("Delimiter", ")"));

    // Match and add the semicolon ";"
    matchToken("Delimiter", ";");
    addChild(outputNode, createParseTreeNode("Delimiter", ";"));

    return outputNode;
}



ParseTreeNode* parseConditionalStatement() {
    printf("Parsing Conditional Statement...\n");

    // Create the conditional statement node
    ParseTreeNode* conditionalNode = createParseTreeNode("ConditionalStatement", "");

    // Match "if"
    if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "if") == 0) {
        addChild(conditionalNode, matchToken("Keyword", "if"));
    } else {
        syntaxError("Expected 'if' keyword.");
    }

    // Match "("
    if (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, "(") == 0) {
        addChild(conditionalNode, matchToken("Delimiter", "("));
    } else {
        syntaxError("Expected '(' after 'if'.");
    }

    // Parse the condition expression
    addChild(conditionalNode, parseExpression());

    // Match ")"
    if (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ")") == 0) {
        addChild(conditionalNode, matchToken("Delimiter", ")"));
    } else {
        syntaxError("Expected ')' after condition.");
    }

    // Parse the statement block
    addChild(conditionalNode, parseStatementBlock());

    // Handle optional "else" or "else if"
    if (peekToken() && strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "else") == 0) {
        ParseTreeNode* elseNode = createParseTreeNode("ElseClause", "");
        addChild(conditionalNode, elseNode);
        addChild(elseNode, matchToken("Keyword", "else"));

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



ParseTreeNode* parseIterativeStatement() {
    printf("Parsing Iterative Statement...\n");

    // Create a parse tree node for the iterative statement
    ParseTreeNode* iterativeStatementNode = createParseTreeNode("IterativeStatement", "");

    // Peek at the token to decide which iterative structure to parse
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0) {
        syntaxError("Expected a valid keyword for an iterative statement.");
    }

    // Match and parse based on the iterative keyword
    if (strcmp(token->value, "for") == 0) {
        addChild(iterativeStatementNode, parseForLoop());
    } else if (strcmp(token->value, "while") == 0) {
        addChild(iterativeStatementNode, parseWhileLoop());
    } else if (strcmp(token->value, "do") == 0) {
        addChild(iterativeStatementNode, parseDoWhileLoop());
    } else {
        char errorMessage[100];
        snprintf(errorMessage, sizeof(errorMessage), 
                 "Expected 'for', 'while', or 'do' at the start of an iterative statement. Got '%s'.",
                 token->value);
        syntaxError(errorMessage);
    }

    return iterativeStatementNode;
}



ParseTreeNode* parseArrayStatement() {
    printf("\n\nParsing Array Statement...\n");

    // Create a parse tree node for the array statement
    ParseTreeNode* arrayStatementNode = createParseTreeNode("ArrayStatement", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing array statement.");
        return NULL;
    }

    // Check the type of the array statement
    if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "array") == 0) {
        // Handle array declaration
        addChild(arrayStatementNode, parseArrayDeclaration());
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Peek ahead to determine if it's an access or an assignment
        Token* nextToken = peekNextToken();
        if (nextToken && strcmp(nextToken->type, "Delimiter") == 0 && strcmp(nextToken->value, "[") == 0) {
            // Handle array access (with optional assignment)
            addChild(arrayStatementNode, parseArrayAccess());
        } else if (nextToken && strcmp(nextToken->type, "AssignmentOperator") == 0) {
            // Handle array assignment
            addChild(arrayStatementNode, parseArrayAssignment());
        } else {
            syntaxError("Invalid array statement. Expected '[' or assignment operator after identifier.");
            freeParseTree(arrayStatementNode);
            return NULL;
        }
    } else {
        syntaxError("Expected 'array' keyword or identifier for array statement.");
        freeParseTree(arrayStatementNode);
        return NULL;
    }

    return arrayStatementNode;
}



ParseTreeNode* parseFunctionStatement() {
    printf("Parsing Function Statement...\n");

    // Create a parse tree node for the function statement
    ParseTreeNode* functionStmtNode = createParseTreeNode("FunctionStatement", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing function statement.");
        return NULL;
    }

    // Check if it's a function declaration (starts with a type specifier or "void")
    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0 || strcmp(token->value, "void") == 0)) {
        printf("Detected Function Declaration...\n");
        ParseTreeNode* functionDeclNode = parseFunctionDeclaration();
        if (!functionDeclNode) {
            syntaxError("Error parsing function declaration.");
            freeParseTree(functionStmtNode);
            return NULL;
        }
        addChild(functionStmtNode, functionDeclNode);
    }
    // Otherwise, check if it's a function call (starts with an identifier)
    else if (strcmp(token->type, "IDENTIFIER") == 0) {
        printf("Detected Function Call...\n");
        ParseTreeNode* functionCallNode = parseFunctionCall();
        if (!functionCallNode) {
            syntaxError("Error parsing function call.");
            freeParseTree(functionStmtNode);
            return NULL;
        }
        addChild(functionStmtNode, functionCallNode);
    } else {
        syntaxError("Expected a function declaration or call in function statement.");
        freeParseTree(functionStmtNode);
        return NULL;
    }

    return functionStmtNode;
}



ParseTreeNode* parseExpressionStatement() {
    printf("Parsing Expression Statement...\n");

    // Create a parse tree node for the expression statement
    ParseTreeNode* expressionStatementNode = createParseTreeNode("ExpressionStatement", "");

    // Parse the expression and add it as a child
    addChild(expressionStatementNode, parseExpression());

    // Expect a semicolon to terminate the expression statement
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of an expression statement.");
    }

    // Add the semicolon as a child node
    addChild(expressionStatementNode, createParseTreeNode("Delimiter", token->value));

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
        addChild(jumpStatementNode, createParseTreeNode("Keyword", "return"));

        // Check if there's an expression to return
        if (peekToken() && (strcmp(peekToken()->type, "IDENTIFIER") == 0 ||
                            strcmp(peekToken()->type, "INT_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "FLOAT_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "CHAR_LITERAL") == 0 ||
                            strcmp(peekToken()->type, "STRING_LITERAL") == 0)) {
            addChild(jumpStatementNode, parseExpression());
        }

        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
            syntaxError("Expected ';' after return statement.");
        }

        // Add semicolon as a child node
        addChild(jumpStatementNode, createParseTreeNode("Delimiter", ";"));

    } else if (strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
        // Add 'break' or 'continue' keyword as a child node
        addChild(jumpStatementNode, createParseTreeNode("Keyword", token->value));

        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
            syntaxError("Expected ';' after break/continue statement.");
        }

        // Add semicolon as a child node
        addChild(jumpStatementNode, createParseTreeNode("Delimiter", ";"));

    } else {
        syntaxError("Unrecognized jump statement. Expected 'return', 'break', or 'continue'.");
    }

    return jumpStatementNode;
}
















// ---------------------------------------
// Conditional
// ---------------------------------------

ParseTreeNode* parseCompoundStatement() {

}



ParseTreeNode* parseIfStatement() {
    printf("\n\nParsing If Statement...\n");

    // Create a node for the if statement
    ParseTreeNode* ifNode = createParseTreeNode("IfStatement", "");

    // Match the 'if' keyword
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "if") == 0)) {
        syntaxError("Expected 'if' keyword.");
    }
    printf("Matched 'if' keyword.\n");
    addChild(ifNode, createParseTreeNode("Keyword", "if"));

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0)) {
        syntaxError("Expected '(' after 'if'.");
    }
    printf("Matched '(' after 'if'.\n");
    addChild(ifNode, createParseTreeNode("Delimiter", "("));

    // Parse the condition inside the parentheses
    ParseTreeNode* conditionNode = parseExpression();
    if (!conditionNode) {
        syntaxError("Invalid condition in 'if' statement.");
    }
    addChild(ifNode, conditionNode);

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        syntaxError("Expected ')' after condition.");
    }
    printf("Matched ')' after condition.\n");
    addChild(ifNode, createParseTreeNode("Delimiter", ")"));

    // Parse the statement block
    ParseTreeNode* statementBlock = parseStatement();
    if (!statementBlock) {
        syntaxError("Invalid statement block in 'if' statement.");
    }
    addChild(ifNode, statementBlock);

    // Check for optional 'else' or 'else if'
    token = peekToken();
    if (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "else") == 0) {
        getNextToken(); // Consume 'else'
        printf("Matched 'else' keyword.\n");
        ParseTreeNode* elseNode = createParseTreeNode("ElseClause", "");

        token = peekToken();
        if (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "if") == 0) {
            // Parse an 'else if' statement
            printf("Parsing 'else if' clause.\n");
            addChild(elseNode, parseIfStatement());
        } else {
            // Parse the 'else' statement block
            printf("Parsing 'else' clause.\n");
            ParseTreeNode* elseBlock = parseStatement();
            if (!elseBlock) {
                syntaxError("Invalid statement block in 'else' clause.");
            }
            addChild(elseNode, elseBlock);
        }

        addChild(ifNode, elseNode);
    }

    return ifNode;
}



ParseTreeNode* parseCaseList() {

}



ParseTreeNode* parseSwitchStatement() {
    printf("Parsing Switch Statement...\n");

    // Create a parse tree node for the switch statement
    ParseTreeNode* switchNode = createParseTreeNode("SwitchStatement", "");

    // Match the 'switch' keyword
    addChild(switchNode, matchToken("Keyword", "switch"));

    // Match the opening parenthesis '('
    addChild(switchNode, matchToken("Delimiter", "("));

    // Parse the controlling expression
    addChild(switchNode, parseExpression());

    // Match the closing parenthesis ')'
    addChild(switchNode, matchToken("Delimiter", ")"));

    // Match the opening curly brace '{'
    addChild(switchNode, matchToken("Delimiter", "{"));

    // Parse case statements
    while (peekToken() && strcmp(peekToken()->value, "case") == 0) {
        addChild(switchNode, parseCaseStatement());
    }

    // Optionally parse the default case
    if (peekToken() && strcmp(peekToken()->value, "default") == 0) {
        addChild(switchNode, parseDefaultCase());
    }

    // Match the closing curly brace '}'
    addChild(switchNode, matchToken("Delimiter", "}"));

    return switchNode;
}


ParseTreeNode* parseCaseStatement() {
    printf("\n\nParsing Case Statement...\n");

    // Create a node for the case statement
    ParseTreeNode* caseNode = createParseTreeNode("CaseStatement", "");

    // Match the 'case' keyword
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "case") == 0)) {
        syntaxError("Expected 'case' keyword.");
    }
    printf("Matched 'case' keyword.\n");
    addChild(caseNode, createParseTreeNode("Keyword", "case"));

    // Match the case value (INT_LITERAL or CHAR_LITERAL)
    token = getNextToken();
    if (!token || !(strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "CHAR_LITERAL") == 0)) {
        syntaxError("Expected an integer or character literal after 'case'.");
    }
    printf("Matched case value: %s\n", token->value);
    addChild(caseNode, createParseTreeNode("Literal", token->value));

    // Match the colon ':'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ":") == 0)) {
        syntaxError("Expected ':' after case value.");
    }
    printf("Matched ':' after case value.\n");
    addChild(caseNode, createParseTreeNode("Symbol", ":"));

    // Parse the statements inside the case
    while (peekToken() && !(strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "break") == 0)) {
        printf("Parsing statement inside case block.\n");
        addChild(caseNode, parseStatement());
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "break") == 0)) {
        syntaxError("Expected 'break' in case statement.");
    }
    printf("Matched 'break' keyword.\n");
    addChild(caseNode, createParseTreeNode("Keyword", "break"));

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0)) {
        syntaxError("Expected ';' after 'break'.");
    }
    printf("Matched ';' after 'break'.\n");
    addChild(caseNode, createParseTreeNode("Symbol", ";"));

    return caseNode;
}



ParseTreeNode* parseDefaultCase() {
    printf("\n\nParsing Default Case...\n");

    // Create a node for the default case
    ParseTreeNode* defaultNode = createParseTreeNode("DefaultCase", "");

    // Match the 'default' keyword
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "default") == 0)) {
        syntaxError("Expected 'default' keyword.");
    }
    printf("Matched 'default' keyword.\n");
    addChild(defaultNode, createParseTreeNode("Keyword", "default"));

    // Match the colon ':'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ":") == 0)) {
        syntaxError("Expected ':' after 'default'.");
    }
    printf("Matched ':' after 'default'.\n");
    addChild(defaultNode, createParseTreeNode("Symbol", ":"));

    // Parse the statements inside the default case
    while (peekToken() && !(strcmp(peekToken()->type, "Keyword") == 0 && strcmp(peekToken()->value, "break") == 0)) {
        printf("Parsing statement inside default block.\n");
        addChild(defaultNode, parseStatement());
    }

    // Match the 'break' keyword
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "break") == 0)) {
        syntaxError("Expected 'break' in default case.");
    }
    printf("Matched 'break' keyword.\n");
    addChild(defaultNode, createParseTreeNode("Keyword", "break"));

    // Match the semicolon ';'
    token = getNextToken();
    if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0)) {
        syntaxError("Expected ';' after 'break'.");
    }
    printf("Matched ';' after 'break'.\n");
    addChild(defaultNode, createParseTreeNode("Symbol", ";"));

    return defaultNode;
}



ParseTreeNode* parseStatementBlock() {
    printf("DEBUG: Parsing Statement Block...\n");

    // Create a node for the statement block
    ParseTreeNode* statementBlockNode = createParseTreeNode("StatementBlock", NULL);

    // Peek at the current token
    Token* token = peekToken();
    if (token == NULL) {
        reportSyntaxError("Unexpected end of input while parsing statement block.");
        return statementBlockNode; // Return empty node for robustness
    }

    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        // Match the opening curly brace
        printf("DEBUG: Detected compound statement (block) in statement block.\n");
        matchToken("Delimiter", "{");
        addChild(statementBlockNode, createParseTreeNode("Delimiter", "{"));

        // Parse the block contents
        ParseTreeNode* blockNode = parseBlock();
        if (blockNode) {
            addChild(statementBlockNode, blockNode);
        } else {
            reportSyntaxError("Failed to parse block inside statement block.");
            recoverFromError();
        }

        // Ensure the closing curly brace
        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
            matchToken("Delimiter", "}");
            addChild(statementBlockNode, createParseTreeNode("Delimiter", "}"));
        } else {
            reportSyntaxError("Expected '}' at the end of statement block.");
            recoverFromError();
        }
    } else {
        // Parse a single statement
        printf("DEBUG: Detected single statement in statement block.\n");
        ParseTreeNode* statementNode = parseStatement();
        if (statementNode) {
            addChild(statementBlockNode, statementNode);
        } else {
            reportSyntaxError("Failed to parse statement in statement block.");
            recoverFromError();
        }
    }

    printf("DEBUG: Completed parsing Statement Block.\n");
    return statementBlockNode;
}

















// ---------------------------------------
// Loop
// ---------------------------------------

ParseTreeNode* parseForLoop() {
    printf("Parsing For Loop...\n");

    // Create a parse tree node for the for loop
    ParseTreeNode* forNode = createParseTreeNode("ForLoop", "");

    // Match the 'for' keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "for") != 0) {
        syntaxError("Expected 'for' keyword.");
    }
    addChild(forNode, createParseTreeNode("Keyword", token->value));

    // Match the opening parenthesis '('
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'for'.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the optional initialization statement
    if (peekToken() && !(strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ";") == 0)) {
        addChild(forNode, parseForInit());
    }

    // Match the first semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after initialization in 'for' loop.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the optional condition
    if (peekToken() && !(strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ";") == 0)) {
        addChild(forNode, parseExpression());
    }

    // Match the second semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after condition in 'for' loop.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the optional update statement
    if (peekToken() && !(strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ")") == 0)) {
        addChild(forNode, parseForUpdate());
    }

    // Match the closing parenthesis ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after 'for' loop update.");
    }
    addChild(forNode, createParseTreeNode("Delimiter", token->value));

    // Parse the loop body
    addChild(forNode, parseStatementBlock());

    return forNode;
}



ParseTreeNode* parseWhileLoop() {
    printf("\n\nParsing While Loop...\n");

    // Create a parse tree node for the while loop
    ParseTreeNode* whileNode = createParseTreeNode("WhileLoop", "");

    // Match "while"
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword.");
    }
    addChild(whileNode, createParseTreeNode("Keyword", token->value));

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'while'.");
    }
    addChild(whileNode, createParseTreeNode("Delimiter", token->value));

    // Parse the condition expression
    addChild(whileNode, parseExpression());

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after condition expression in 'while' loop.");
    }
    addChild(whileNode, createParseTreeNode("Delimiter", token->value));

    // Parse the statement block
    addChild(whileNode, parseStatementBlock());

    return whileNode;
}



ParseTreeNode* parseDoWhileLoop() {
    printf("\n\nParsing Do-While Loop...\n");

    // Create a parse tree node for the do-while loop
    ParseTreeNode* doWhileNode = createParseTreeNode("DoWhileLoop", "");

    // Match "do"
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "do") != 0) {
        syntaxError("Expected 'do' keyword.");
    }
    addChild(doWhileNode, createParseTreeNode("Keyword", token->value));

    // Parse the statement block
    addChild(doWhileNode, parseStatementBlock());

    // Match "while"
    token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "while") != 0) {
        syntaxError("Expected 'while' keyword after 'do' block.");
    }
    addChild(doWhileNode, createParseTreeNode("Keyword", token->value));

    // Match '('
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after 'while' in do-while loop.");
    }
    addChild(doWhileNode, createParseTreeNode("Delimiter", token->value));

    // Parse the condition expression
    addChild(doWhileNode, parseExpression());

    // Match ')'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after condition expression in 'do-while' loop.");
    }
    addChild(doWhileNode, createParseTreeNode("Delimiter", token->value));

    // Match ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of 'do-while' loop.");
    }
    addChild(doWhileNode, createParseTreeNode("Delimiter", token->value));

    return doWhileNode;
}


ParseTreeNode* parseForInit() {
    printf("\n\nParsing For Loop Initialization...\n");

    // Create a node for the for loop initialization
    ParseTreeNode* forInitNode = createParseTreeNode("ForInit", "");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop initialization.");
    }

    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        // Variable declaration
        printf("Detected variable declaration in 'for' loop initialization.\n");
        addChild(forInitNode, parseVariableDeclaration());
    } else if (strcmp(token->type, "SpecifierIdentifier") == 0 ||
               strcmp(token->type, "IDENTIFIER") == 0) {
        // Assignment statement
        printf("Detected assignment statement or SpecifierIdentifier in 'for' loop initialization.\n");
        addChild(forInitNode, parseAssignmentStatement());
    } else {
        syntaxError("Invalid initialization in 'for' loop. Expected variable declaration, assignment statement, or SpecifierIdentifier.");
    }

    return forInitNode;
}






























// ---------------------------------------
// Expressions and Operators
// ---------------------------------------

ParseTreeNode* parseExpression() {
    printf("\n\nParsing Expression...\n");

    ParseTreeNode* exprNode = createParseTreeNode("Expression","");

    addChild(exprNode, parseLogicalOrExpr());

    return exprNode;
}



ParseTreeNode* parseLogicalOrExpr() {
    printf("\n\nParsing Logical OR Expression...\n");

    // Create a parse tree node for the Logical OR expression
    ParseTreeNode* logicalOrNode = createParseTreeNode("LogicalOrExpr", "");

    // Parse the left-hand side as a Logical AND expression
    addChild(logicalOrNode, parseLogicalAndExpr());

    // Check for multiple '||' operators
    while (peekToken() && strcmp(peekToken()->type, "LogicalOperator") == 0 && strcmp(peekToken()->value, "||") == 0) {
        Token* operatorToken = getNextToken(); // Consume the '||' operator
        printf("DEBUG: Logical OR operator detected: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(logicalOrNode, createParseTreeNode("LogicalOperator", operatorToken->value));

        // Parse the right-hand side as another Logical AND expression
        addChild(logicalOrNode, parseLogicalAndExpr());
    }

    return logicalOrNode;
}



ParseTreeNode* parseLogicalAndExpr() {
    printf("\n\nParsing Logical AND Expression...\n");

    // Create a parse tree node for the Logical AND expression
    ParseTreeNode* logicalAndNode = createParseTreeNode("LogicalAndExpr", "");

    // Parse the left-hand side as an Equality expression
    addChild(logicalAndNode, parseEqualityExpr());

    // Continuously check for '&&' operators and parse accordingly
    while (peekToken() && strcmp(peekToken()->type, "LogicalOperator") == 0 && strcmp(peekToken()->value, "&&") == 0) {
        // Consume the '&&' operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Logical AND operator detected: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(logicalAndNode, createParseTreeNode("LogicalOperator", operatorToken->value));

        // Parse the right-hand side as another Equality expression
        ParseTreeNode* rhs = parseEqualityExpr();
        if (!rhs) {
            printf("ERROR: Missing right-hand side of Logical AND expression.\n");
            break;
        }
        addChild(logicalAndNode, rhs);
    }

    return logicalAndNode;
}


ParseTreeNode* parseEqualityExpr() {
    printf("\n\nParsing Equality Expression...\n");

    // Create a parse tree node for the Equality expression
    ParseTreeNode* equalityNode = createParseTreeNode("EqualityExpr", "");

    // Parse the first operand (Relational expression)
    addChild(equalityNode, parseRelationalExpr());

    // Check for equality or relational operators
    while (peekToken() && strcmp(peekToken()->type, "RelationalOperator") == 0) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Relational Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(equalityNode, createParseTreeNode("RelationalOperator", operatorToken->value));

        // Parse the next operand (Relational expression)
        addChild(equalityNode, parseRelationalExpr());
    }

    return equalityNode;
}



ParseTreeNode* parseRelationalExpr() {
    printf("\n\nParsing Relational Expression...\n");

    // Create a parse tree node for the Relational expression
    ParseTreeNode* relationalNode = createParseTreeNode("RelationalExpr", "");

    // Parse the first operand (Additive expression)
    addChild(relationalNode, parseAdditiveExpr());

    // Check for relational operators (<, >, <=, >=, ==, !=)
    while (peekToken() && strcmp(peekToken()->type, "RelationalOperator") == 0 &&
           (strcmp(peekToken()->value, "<") == 0 || strcmp(peekToken()->value, ">") == 0 ||
            strcmp(peekToken()->value, "<=") == 0 || strcmp(peekToken()->value, ">=") == 0 ||
            strcmp(peekToken()->value, "==") == 0 || strcmp(peekToken()->value, "!=") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Relational Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(relationalNode, createParseTreeNode("RelationalOperator", operatorToken->value));

        // Parse the next operand (Additive expression)
        addChild(relationalNode, parseAdditiveExpr());
    }

    return relationalNode;
}



ParseTreeNode* parseAdditiveExpr() {
    printf("\n\nParsing Additive Expression...\n");

    // Create a parse tree node for the Additive expression
    ParseTreeNode* additiveNode = createParseTreeNode("AdditiveExpr", "");

    // Parse the first operand (Multiplicative expression)
    addChild(additiveNode, parseMultiplicativeExpr());

    // Check for additive operators ('+', '-')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           (strcmp(peekToken()->value, "+") == 0 || strcmp(peekToken()->value, "-") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        if (operatorToken) {
            printf("DEBUG: Matched Additive Operator: %s\n", operatorToken->value);
            addChild(additiveNode, createParseTreeNode("ArithmeticOperator", operatorToken->value));
        } else {
            fprintf(stderr, "ERROR: Unexpected NULL operator token in Additive Expression.\n");
            break;
        }

        // Parse the next operand (Multiplicative expression)
        ParseTreeNode* nextOperand = parseMultiplicativeExpr();
        if (nextOperand) {
            addChild(additiveNode, nextOperand);
        } else {
            fprintf(stderr, "ERROR: Failed to parse operand in Additive Expression.\n");
            break;
        }
    }

    return additiveNode;
}



ParseTreeNode* parseMultiplicativeExpr() {
    printf("\n\nParsing Multiplicative Expression...\n");

    // Create a parse tree node for the Multiplicative expression
    ParseTreeNode* multiplicativeNode = createParseTreeNode("MultiplicativeExpr", "");

    // Parse the first operand (Exponential expression)
    addChild(multiplicativeNode, parseExponentialExpr());

    // Check for multiplicative operators ('*', '/', '%', '//')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           (strcmp(peekToken()->value, "*") == 0 || 
            strcmp(peekToken()->value, "/") == 0 || 
            strcmp(peekToken()->value, "%") == 0 || 
            strcmp(peekToken()->value, "//") == 0)) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Multiplicative Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(multiplicativeNode, createParseTreeNode("ArithmeticOperator", operatorToken->value));

        // Parse the next operand (Exponential expression)
        addChild(multiplicativeNode, parseExponentialExpr());
    }

    return multiplicativeNode;
}




ParseTreeNode* parseExponentialExpr() {
    printf("\n\nParsing Exponential Expression...\n");

    // Create a parse tree node for the Exponential expression
    ParseTreeNode* exponentialNode = createParseTreeNode("ExponentialExpr", "");

    // Parse the first operand (Unary expression)
    addChild(exponentialNode, parseUnaryExpr());

    // Check for the exponential operator ('^')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           strcmp(peekToken()->value, "^") == 0) {
        // Consume the operator
        Token* operatorToken = getNextToken();
        printf("DEBUG: Matched Exponential Operator: %s\n", operatorToken->value);

        // Add the operator as a child node
        addChild(exponentialNode, createParseTreeNode("ArithmeticOperator", operatorToken->value));

        // Parse the next operand (Unary expression)
        addChild(exponentialNode, parseUnaryExpr());
    }

    return exponentialNode;
}



ParseTreeNode* parseUnaryExpr() {
    printf("\n\nParsing Unary Expression...\n");

    // Create a parse tree node for the Unary Expression
    ParseTreeNode* unaryNode = createParseTreeNode("UnaryExpr", "");

    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of input while parsing unary expression");
        return NULL; // Return NULL in case of an error
    }

    // Check for unary operators (Logical NOT `!`, Subtraction `-`, Increment `++`, Decrement `--`)
    if ((strcmp(token->type, "LogicalOperator") == 0 && strcmp(token->value, "!") == 0) ||
        (strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "-") == 0) ||
        (strcmp(token->type, "UnaryOperator") == 0 && 
         (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0))) {
        token = getNextToken(); // Consume the unary operator
        printf("DEBUG: Matched Unary Operator: %s\n", token->value);

        // Add the operator as a child node
        addChild(unaryNode, createParseTreeNode("UnaryOperator", token->value));

        // Parse the operand (recursively handle unary expressions)
        addChild(unaryNode, parseUnaryExpr());
    } else if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        // Handle SpecifierIdentifier (e.g., &number)
        token = getNextToken(); // Consume the SpecifierIdentifier
        printf("DEBUG: Matched SpecifierIdentifier: %s\n", token->value);

        // Add the SpecifierIdentifier as a child node
        addChild(unaryNode, createParseTreeNode("SpecifierIdentifier", token->value));
    } else {
        // If no unary operator, parse as a postfix expression
        addChild(unaryNode, parsePostfixExpr());
    }

    return unaryNode;
}



ParseTreeNode* parsePostfixExpr() {
    printf("\n\nParsing Postfix Expression...\n");

    // Create a parse tree node for the Postfix Expression
    ParseTreeNode* postfixNode = createParseTreeNode("PostfixExpr", "");

    // Parse the primary expression and add it as a child
    addChild(postfixNode, parsePrimaryExpr());

    // Handle optional postfix operations: function calls or unary operators
    Token* token = peekToken();
    while (token && 
           ((strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) ||
            strcmp(token->type, "UnaryOperator") == 0)) {

        if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
            printf("Parsing Function Call...\n");

            // Create a node for the function call
            ParseTreeNode* funcCallNode = createParseTreeNode("FunctionCall", "");

            getNextToken(); // Consume '('

            // Parse the argument list and add it as a child
            addChild(funcCallNode, parseArgumentList());

            token = getNextToken();
            if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
                syntaxError("Expected ')' after function arguments");
            }

            // Add the function call node as a child of the postfix node
            addChild(postfixNode, funcCallNode);

        } else if (strcmp(token->type, "UnaryOperator") == 0) {
            printf("Parsing Unary Operator...\n");

            // Create a node for the unary operator
            ParseTreeNode* unaryOpNode = createParseTreeNode("UnaryOperator", token->value);

            getNextToken(); // Consume the unary operator

            // Add the unary operator node as a child of the postfix node
            addChild(postfixNode, unaryOpNode);
        }

        token = peekToken(); // Check for more postfix operations
    }

    // Check for array access and delegate to parseArrayAccess
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
        printf("Delegating to parseArrayAccess...\n");
        addChild(postfixNode, parseArrayAccess());
    }

    return postfixNode;
}



ParseTreeNode* parseIncDec() {
    printf("Parsing Increment/Decrement...\n");

    // Create a parse tree node for increment/decrement
    ParseTreeNode* incDecNode = createParseTreeNode("IncDec", "");

    // Peek the next token to decide the order
    Token* token = peekToken();

    if (!token) {
        syntaxError("Unexpected end of tokens in increment/decrement statement.");
    }

    if (strcmp(token->type, "UnaryOperator") == 0 && 
        (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0)) {
        // Prefix increment/decrement
        token = getNextToken();
        addChild(incDecNode, createParseTreeNode("UnaryOperator", token->value));

        // Match and parse the identifier
        token = getNextToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            syntaxError("Expected an identifier after '++' or '--' in prefix increment/decrement.");
        }
        addChild(incDecNode, createParseTreeNode("IDENTIFIER", token->value));

    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Postfix increment/decrement
        token = getNextToken();
        addChild(incDecNode, createParseTreeNode("IDENTIFIER", token->value));

        // Match and parse the increment/decrement operator
        token = getNextToken();
        if (!token || !(strcmp(token->type, "UnaryOperator") == 0 && 
                       (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0))) {
            syntaxError("Expected '++' or '--' after identifier in postfix increment/decrement.");
        }
        addChild(incDecNode, createParseTreeNode("UnaryOperator", token->value));

    } else {
        syntaxError("Expected '++', '--', or an identifier in increment/decrement statement.");
    }

    return incDecNode;
}









































// ---------------------------------------
// Literals and Identifiers
// ---------------------------------------


ParseTreeNode* parseLiteral() {
    printf("\n\nParsing Literal...\n");

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
        printf("Matched Literal: Type='%s', Value='%s'\n", token->type, token->value);

        // Add the literal value to the node
        setNodeValue(literalNode, token->value);

        getNextToken(); // Consume the literal token
    } else if (strcmp(token->type, "ReservedWord") == 0) {
        if (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0) {
            printf("Matched Boolean Literal: Value='%s'\n", token->value);

            // Add the boolean value to the node
            setNodeValue(literalNode, token->value);

            getNextToken(); // Consume the boolean literal
        } else if (strcmp(token->value, "null") == 0) {
            printf("Matched Null Literal: Value='%s'\n", token->value);

            // Add the null value to the node
            setNodeValue(literalNode, token->value);

            getNextToken(); // Consume the null literal
        } else {
            syntaxError("Expected a literal (integer, float, char, string, boolean, or null).");
        }
    } else {
        syntaxError("Expected a literal (integer, float, char, string, boolean, or null).");
    }

    return literalNode;
}



ParseTreeNode* parseBoolLiteral() {
    printf("\n\nParsing Boolean Literal...\n");

    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing boolean literal.");
    }

    // Create a node for the boolean literal
    ParseTreeNode* boolLiteralNode = createParseTreeNode("BoolLiteral", "");

    // Match boolean literals ("true" or "false")
    if (strcmp(token->type, "ReservedWord") == 0 && 
        (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        printf("Matched Boolean Literal: Value='%s'\n", token->value);

        // Add the boolean value to the node
        setNodeValue(boolLiteralNode, token->value);

        getNextToken(); // Consume the boolean literal token
    } else {
        syntaxError("Expected a boolean literal ('true' or 'false').");
    }

    return boolLiteralNode;
}



ParseTreeNode* parsePrimaryExpr() {
    printf("\n\nParsing Primary Expression...\n");

    Token* token = getNextToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing a primary expression");
        return NULL;
    }

    // Create a node for the primary expression
    ParseTreeNode* primaryNode = createParseTreeNode("PrimaryExpr", "");

    if (strcmp(token->type, "INT_LITERAL") == 0 || 
        strcmp(token->type, "FLOAT_LITERAL") == 0 || 
        strcmp(token->type, "CHAR_LITERAL") == 0 || 
        strcmp(token->type, "STRING_LITERAL") == 0 || 
        strcmp(token->type, "BOOLEAN_LITERAL") == 0) {
        // It's a literal
        printf("Literal detected: Type='%s', Value='%s'\n", token->type, token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // It's an identifier
        printf("IDENTIFIER detected: %s\n", token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        // It's a specifier identifier (e.g., &variable)
        printf("SpecifierIdentifier detected: %s\n", token->value);
        setNodeValue(primaryNode, token->value);
    } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
        // It's a grouped expression
        printf("Detected '(' indicating a grouped expression.\n");
        addChild(primaryNode, parseExpression()); // Parse the inner expression

        token = getNextToken();
        if (!token || !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
            syntaxError("Expected ')' to close the grouped expression");
            freeParseTree(primaryNode); // Clean up on error
            return NULL;
        }
        printf("Detected closing ')' for grouped expression.\n");
    } else {
        syntaxError("Expected a literal, identifier, specifier identifier, or grouped expression");
        freeParseTree(primaryNode); // Clean up on error
        return NULL;
    }

    return primaryNode;
}
























// ---------------------------------------
// Input and Output Specific Functions
// ---------------------------------------

ParseTreeNode* parseInputList() {
    printf("Parsing Input List...\n");

    // Create a parse tree node for the input list
    ParseTreeNode* inputListNode = createParseTreeNode("InputList", "");

    // Parse at least one format-variable pair
    addChild(inputListNode, parseFormatVariablePair());

    // Parse additional format-variable pairs separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        getNextToken(); // Consume the comma
        addChild(inputListNode, createParseTreeNode("Delimiter", ",")); // Add the comma as a child

        // Parse the next format-variable pair
        addChild(inputListNode, parseFormatVariablePair());
        token = peekToken(); // Update token for the next iteration
    }

    return inputListNode;
}



ParseTreeNode* parseFormatVariablePair() {
    printf("Parsing Format-Variable Pair...\n");

    // Create a parse tree node for the format-variable pair
    ParseTreeNode* pairNode = createParseTreeNode("FormatVariablePair", "");

    // Match and add the format string (STRING_LITERAL)
    Token* token = peekToken();
    if (token && strcmp(token->type, "STRING_LITERAL") == 0) {
        getNextToken(); // Consume the format string
        addChild(pairNode, createParseTreeNode("STRING_LITERAL", token->value));
    } else {
        fprintf(stderr, "Error: Expected STRING_LITERAL in format-variable pair.\n");
        exit(EXIT_FAILURE);
    }

    // Match and add the variable (SpecifierIdentifier)
    token = peekToken();
    if (token && strcmp(token->type, "SpecifierIdentifier") == 0) {
        getNextToken(); // Consume the specifier identifier
        addChild(pairNode, createParseTreeNode("SpecifierIdentifier", token->value));
    } else {
        fprintf(stderr, "Error: Expected SpecifierIdentifier in format-variable pair.\n");
        exit(EXIT_FAILURE);
    }

    return pairNode;
}



ParseTreeNode* parseFormatSpecifier() {

}


ParseTreeNode* parseAddressVariable() {

}



ParseTreeNode* parseOutputList() {
    printf("Parsing Output List...\n");

    // Create a parse tree node for the output list
    ParseTreeNode* outputListNode = createParseTreeNode("OutputList", "");

    Token* token = peekToken();

    // Parse the first argument (STRING_LITERAL or expression)
    if (token && strcmp(token->type, "STRING_LITERAL") == 0) {
        // Parse as a string literal
        token = getNextToken();
        addChild(outputListNode, createParseTreeNode("STRING_LITERAL", token->value));
    } else if (token && 
               (strcmp(token->type, "IDENTIFIER") == 0 || 
                strcmp(token->type, "INT_LITERAL") == 0 || 
                strcmp(token->type, "FLOAT_LITERAL") == 0 || 
                strcmp(token->type, "CHAR_LITERAL") == 0 || 
                strcmp(token->type, "SpecifierIdentifier") == 0)) {
        // Parse as an expression
        addChild(outputListNode, parseExpression());
    } else {
        syntaxError("Expected STRING_LITERAL or expression as the first argument in output list.");
        return NULL;
    }

    // Parse additional arguments separated by commas
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ",") == 0) {
        // Consume the comma
        Token* commaToken = getNextToken();
        addChild(outputListNode, createParseTreeNode("Delimiter", commaToken->value));

        // Parse the next argument (expression)
        addChild(outputListNode, parseExpression());
    }

    return outputListNode;
}



ParseTreeNode* parseStringConcat() {
    printf("Parsing String Concatenation...\n");

    // Create a parse tree node for string concatenation
    ParseTreeNode* stringConcatNode = createParseTreeNode("StringConcat", "");

    // Parse at least one string literal or format string
    Token* token = getNextToken();
    if (!token || !(strcmp(token->type, "FormatString") == 0 || strcmp(token->type, "STRING_LITERAL") == 0)) {
        syntaxError("Expected a format string or string literal for concatenation");
    }
    addChild(stringConcatNode, createParseTreeNode(token->type, token->value));

    // Parse additional strings separated by ADD_OP ('+') (if any)
    token = peekToken();
    while (token && strcmp(token->type, "ADD_OP") == 0) {
        getNextToken(); // Consume '+'
        addChild(stringConcatNode, createParseTreeNode("AddOp", "+"));

        token = getNextToken();
        if (!token || !(strcmp(token->type, "FormatString") == 0 || strcmp(token->type, "STRING_LITERAL") == 0)) {
            syntaxError("Expected a format string or string literal after '+'");
        }
        addChild(stringConcatNode, createParseTreeNode(token->type, token->value));

        token = peekToken(); // Check for more concatenations
    }

    return stringConcatNode;
}



ParseTreeNode* parseExpressionList() {
    printf("Parsing Expression List...\n");

    // Create a parse tree node for the expression list
    ParseTreeNode* exprListNode = createParseTreeNode("ExpressionList", "");

    // Parse the first expression
    addChild(exprListNode, parseExpression());

    // Parse subsequent expressions separated by commas
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ",") == 0) {
        Token* token = getNextToken(); // Consume the comma
        addChild(exprListNode, createParseTreeNode("Delimiter", token->value)); // Add the comma as a child
        addChild(exprListNode, parseExpression()); // Parse the next expression
    }

    return exprListNode;
}
















































































// ---------------------------------------
// Array Handling
// ---------------------------------------

ParseTreeNode* parseArrayDeclaration() {
    printf("Parsing Array Declaration...\n");

    // Create a parse tree node for the array declaration
    ParseTreeNode* arrayDeclNode = createParseTreeNode("ArrayDeclaration", "");

    // Match the "array" keyword
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "array") != 0) {
        syntaxError("Expected 'array' keyword for array declaration.");
        freeParseTree(arrayDeclNode);
        return NULL;
    }
    addChild(arrayDeclNode, createParseTreeNode("Keyword", token->value));

    // Parse the array type
    ParseTreeNode* typeNode = parseArrayType();
    if (!typeNode) {
        syntaxError("Failed to parse array type in declaration.");
        freeParseTree(arrayDeclNode);
        return NULL;
    }
    addChild(arrayDeclNode, typeNode);

    // Match the identifier (array name)
    token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for the array name.");
        freeParseTree(arrayDeclNode);
        return NULL;
    }
    addChild(arrayDeclNode, createParseTreeNode("IDENTIFIER", token->value));

    // Parse the array dimensions
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
        ParseTreeNode* dimensionsNode = parseArrayAccess(); // Use access logic for dimensions
        if (!dimensionsNode) {
            syntaxError("Failed to parse array dimensions.");
            freeParseTree(arrayDeclNode);
            return NULL;
        }
        addChild(arrayDeclNode, dimensionsNode);
    }

    // Check for optional initializer
    token = peekToken();
    if (token && strcmp(token->type, "AssignmentOperator") == 0 && strcmp(token->value, "=") == 0) {
        getNextToken(); // Consume '='
        addChild(arrayDeclNode, createParseTreeNode("AssignmentOperator", token->value));

        // Parse initializer values enclosed in '{}'
        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "{") != 0) {
            syntaxError("Expected '{' to start array initializer.");
            freeParseTree(arrayDeclNode);
            return NULL;
        }
        addChild(arrayDeclNode, createParseTreeNode("Delimiter", "{"));

        // Parse initializer values
        do {
            ParseTreeNode* exprNode = parseExpression();
            if (!exprNode) {
                syntaxError("Invalid expression in array initializer.");
                freeParseTree(arrayDeclNode);
                return NULL;
            }
            addChild(arrayDeclNode, exprNode);

            token = peekToken();
            if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
                getNextToken(); // Consume ','
                addChild(arrayDeclNode, createParseTreeNode("Delimiter", ","));
            }
        } while (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0));

        // Match the closing curly brace '}'
        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "}") != 0) {
            syntaxError("Expected '}' to close array initializer.");
            freeParseTree(arrayDeclNode);
            return NULL;
        }
        addChild(arrayDeclNode, createParseTreeNode("Delimiter", "}"));
    }

    // Match semicolon
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of array declaration.");
        freeParseTree(arrayDeclNode);
        return NULL;
    }
    addChild(arrayDeclNode, createParseTreeNode("Delimiter", ";"));

    return arrayDeclNode;
}



ParseTreeNode* parseArrayOperations() {
    printf("\n\nParsing Array Operations...\n");

    // Create a parse tree node for array operations
    ParseTreeNode* arrayOpsNode = createParseTreeNode("ArrayOperations", "");

    // Peek at the current token to determine operation type
    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for array operations.");
        freeParseTree(arrayOpsNode);
        return NULL;
    }

    // Parse the array identifier
    token = getNextToken(); // Consume the identifier
    addChild(arrayOpsNode, createParseTreeNode("IDENTIFIER", token->value));

    // Peek to determine the operation type: access or assignment
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
        // Parse array access
        ParseTreeNode* accessNode = parseArrayAccess();
        if (!accessNode) {
            syntaxError("Failed to parse array access.");
            freeParseTree(arrayOpsNode);
            return NULL;
        }
        addChild(arrayOpsNode, accessNode);
    } else if (token && strcmp(token->type, "AssignmentOperator") == 0 && strcmp(token->value, "=") == 0) {
        // Parse array assignment
        token = getNextToken(); // Consume '='
        addChild(arrayOpsNode, createParseTreeNode("AssignmentOperator", token->value));

        // Parse the assigned value
        ParseTreeNode* valueNode = parseExpression();
        if (!valueNode) {
            syntaxError("Expected an expression for array assignment.");
            freeParseTree(arrayOpsNode);
            return NULL;
        }
        addChild(arrayOpsNode, valueNode);

        // Match the semicolon
        token = getNextToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
            syntaxError("Expected ';' at the end of array assignment.");
            freeParseTree(arrayOpsNode);
            return NULL;
        }
        addChild(arrayOpsNode, createParseTreeNode("Delimiter", ";"));
    } else {
        syntaxError("Invalid array operation. Expected array access or assignment.");
        freeParseTree(arrayOpsNode);
        return NULL;
    }

    return arrayOpsNode;
}



ParseTreeNode* parseArrayAccess() {
    printf("\n\nParsing Array Access...\n");

    // Create a parse tree node for array access
    ParseTreeNode* arrayAccessNode = createParseTreeNode("ArrayAccess", "");

    // Validate and consume the identifier for the array name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for array access.");
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, createParseTreeNode("IDENTIFIER", token->value));

    // Match the left bracket '['
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "[") != 0) {
        syntaxError("Expected '[' to start array access.");
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, createParseTreeNode("Delimiter", "["));

    // Parse the index expression
    ParseTreeNode* indexExprNode = parseExpression();
    if (!indexExprNode) {
        syntaxError("Expected a valid expression for array index.");
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, indexExprNode);

    // Match the right bracket ']'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "]") != 0) {
        syntaxError("Expected ']' to close array access.");
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, createParseTreeNode("Delimiter", "]"));

    return arrayAccessNode;
}



ParseTreeNode* parseArrayAssignment() {
    printf("Parsing Array Assignment...\n");

    // Create a parse tree node for array assignment
    ParseTreeNode* arrayAssignNode = createParseTreeNode("ArrayAssignment", "");

    // Validate and consume the identifier for the array
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for array assignment.");
        freeParseTree(arrayAssignNode);
        return NULL;
    }
    addChild(arrayAssignNode, createParseTreeNode("IDENTIFIER", token->value));

    // Parse array index (if applicable)
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
        ParseTreeNode* arrayAccessNode = parseArrayAccess();
        if (!arrayAccessNode) {
            syntaxError("Failed to parse array access in assignment.");
            freeParseTree(arrayAssignNode);
            return NULL;
        }
        addChild(arrayAssignNode, arrayAccessNode);
    }

    // Match and consume the assignment operator '='
    token = getNextToken();
    if (!token || strcmp(token->type, "AssignmentOperator") != 0 || strcmp(token->value, "=") != 0) {
        syntaxError("Expected '=' in array assignment.");
        freeParseTree(arrayAssignNode);
        return NULL;
    }
    addChild(arrayAssignNode, createParseTreeNode("AssignmentOperator", token->value));

    // Parse the value or expression being assigned
    ParseTreeNode* valueNode = parseExpression();
    if (!valueNode) {
        syntaxError("Invalid expression in array assignment.");
        freeParseTree(arrayAssignNode);
        return NULL;
    }
    addChild(arrayAssignNode, valueNode);

    // Match and consume the semicolon ';'
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' at the end of array assignment.");
        freeParseTree(arrayAssignNode);
        return NULL;
    }
    addChild(arrayAssignNode, createParseTreeNode("Delimiter", ";"));

    return arrayAssignNode;
}



ParseTreeNode* parseForUpdate() {
    printf("\n\nParsing For Loop Update...\n");

    // Create a node for the For Update
    ParseTreeNode* forUpdateNode = createParseTreeNode("ForUpdate", "");

    // Peek at the current token
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input in 'for' loop update.");
        freeParseTree(forUpdateNode);
        return NULL;
    }

    // Match and parse the identifier
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        Token* nextToken = peekSecondToken(); // Peek at the next token

        if (nextToken && strcmp(nextToken->type, "AssignmentOperator") == 0) {
            // Parse an assignment statement
            ParseTreeNode* assignmentNode = parseAssignmentStatement();
            if (!assignmentNode) {
                syntaxError("Invalid assignment in 'for' loop update.");
                freeParseTree(forUpdateNode);
                return NULL;
            }
            addChild(forUpdateNode, assignmentNode);
        } else if (nextToken && strcmp(nextToken->type, "UnaryOperator") == 0 &&
                   (strcmp(nextToken->value, "++") == 0 || strcmp(nextToken->value, "--") == 0)) {
            // Parse an increment/decrement operation
            ParseTreeNode* incDecNode = createParseTreeNode("IncDec", "");
            getNextToken(); // Consume the IDENTIFIER
            addChild(incDecNode, createParseTreeNode("IDENTIFIER", token->value));

            getNextToken(); // Consume the unary operator
            addChild(incDecNode, createParseTreeNode("UnaryOperator", nextToken->value));

            addChild(forUpdateNode, incDecNode);
        } else if (nextToken &&
                   (strcmp(nextToken->type, "ArithmeticOperator") == 0) &&
                   (strcmp(nextToken->value, "+") == 0 || strcmp(nextToken->value, "-") == 0)) {
            // Parse an increment/decrement with an expression
            ParseTreeNode* arithmeticNode = createParseTreeNode("ArithmeticUpdate", "");
            getNextToken(); // Consume the IDENTIFIER
            addChild(arithmeticNode, createParseTreeNode("IDENTIFIER", token->value));

            getNextToken(); // Consume the arithmetic operator
            addChild(arithmeticNode, createParseTreeNode("ArithmeticOperator", nextToken->value));

            ParseTreeNode* exprNode = parseExpression();
            if (!exprNode) {
                syntaxError("Invalid expression in arithmetic update.");
                freeParseTree(forUpdateNode);
                return NULL;
            }
            addChild(arithmeticNode, exprNode);

            addChild(forUpdateNode, arithmeticNode);
        } else {
            syntaxError("Invalid update in 'for' loop.");
            freeParseTree(forUpdateNode);
            return NULL;
        }
    } else {
        syntaxError("Expected an identifier in 'for' loop update.");
        freeParseTree(forUpdateNode);
        return NULL;
    }

    return forUpdateNode;
}



ParseTreeNode* parseArrayType() {
    printf("Parsing Array Type...\n");

    // Create a parse tree node for array type
    ParseTreeNode* arrayTypeNode = createParseTreeNode("ArrayType", "");

    // Match the type specifier (e.g., "int", "float", etc.)
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
          strcmp(token->value, "string") == 0)) {
        syntaxError("Expected a valid type specifier (e.g., 'int', 'float', 'char', 'bool', 'string').");
    }

    // Add the matched type to the parse tree
    addChild(arrayTypeNode, createParseTreeNode("TypeSpecifier", token->value));

    return arrayTypeNode;
}















// ---------------------------------------
// Functions
// ---------------------------------------

ParseTreeNode* parseFunctionDeclaration() {
    printf("Parsing Function Declaration...\n");

    // Create a parse tree node for the function declaration
    ParseTreeNode* functionDeclNode = createParseTreeNode("FunctionDeclaration", "");

    // Parse the return type
    ParseTreeNode* returnTypeNode = parseTypeSpecifier();
    if (!returnTypeNode) {
        syntaxError("Expected a return type in function declaration");
        return NULL;
    }
    addChild(functionDeclNode, returnTypeNode);

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for the function name");
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, createParseTreeNode("IDENTIFIER", token->value));

    // Expect '(' to start the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after the function name");
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, createParseTreeNode("Delimiter", "("));

    // Parse the parameter list if present
    ParseTreeNode* parameterListNode = parseParameterList();
    addChild(functionDeclNode, parameterListNode);

    // Expect ')' to end the parameter list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after the parameter list");
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, createParseTreeNode("Delimiter", ")"));

    // Check for either a semicolon (declaration-only) or a function body (compound statement)
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0) {
        // Declaration-only: consume the semicolon
        getNextToken();
        addChild(functionDeclNode, createParseTreeNode("Delimiter", ";"));
    } else if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        // Function body: delegate to parseBlock()
        ParseTreeNode* blockNode = parseBlock();
        if (!blockNode) {
            syntaxError("Error parsing function body");
            freeParseTree(functionDeclNode);
            return NULL;
        }
        addChild(functionDeclNode, blockNode);
    } else {
        syntaxError("Expected ';' or a function body in function declaration");
        freeParseTree(functionDeclNode);
        return NULL;
    }

    return functionDeclNode;
}



ParseTreeNode* parseFunctionCall() {
    printf("Parsing Function Call...\n");

    // Create a parse tree node for the function call
    ParseTreeNode* functionCallNode = createParseTreeNode("FunctionCall", "");

    // Expect an identifier for the function name
    Token* token = getNextToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        syntaxError("Expected an identifier for function name");
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("FunctionName", token->value));

    // Expect '(' to start the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        syntaxError("Expected '(' after function name");
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the argument list if present
    token = peekToken();
    if (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        ParseTreeNode* argumentListNode = parseArgumentList();
        if (!argumentListNode) {
            syntaxError("Error parsing argument list");
            freeParseTree(functionCallNode);
            return NULL;
        }
        addChild(functionCallNode, argumentListNode);
    }

    // Expect ')' to end the argument list
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        syntaxError("Expected ')' after argument list");
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("RightParenthesis", ")"));

    // Expect a semicolon to end the function call
    token = getNextToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        syntaxError("Expected ';' after function call");
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("Semicolon", ";"));

    return functionCallNode;
}



ParseTreeNode* parseArgumentList() {
    printf("Parsing Argument List...\n");

    // Create a parse tree node for the argument list
    ParseTreeNode* argumentListNode = createParseTreeNode("ArgumentList", "");

    // Parse the first argument
    Token* token = peekToken();
    if (!token) {
        syntaxError("Unexpected end of input while parsing argument list.");
        return argumentListNode; // Return empty argument list
    }

    if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        // Handle specifier identifier (e.g., `&var`)
        getNextToken(); // Consume the SpecifierIdentifier
        addChild(argumentListNode, createParseTreeNode("SpecifierIdentifier", token->value));
    } else {
        // Handle an expression as an argument
        ParseTreeNode* firstArgumentNode = parseExpression();
        if (!firstArgumentNode) {
            syntaxError("Expected a valid expression as the first argument.");
            return NULL;
        }
        addChild(argumentListNode, firstArgumentNode);
    }

    // Parse additional arguments separated by commas
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        getNextToken(); // Consume the comma
        addChild(argumentListNode, createParseTreeNode("Delimiter", ",")); // Add the comma to the tree

        token = peekToken();
        if (!token) {
            syntaxError("Unexpected end of input while parsing additional arguments.");
            return NULL;
        }

        if (strcmp(token->type, "SpecifierIdentifier") == 0) {
            getNextToken(); // Consume the SpecifierIdentifier
            addChild(argumentListNode, createParseTreeNode("SpecifierIdentifier", token->value));
        } else {
            ParseTreeNode* nextArgumentNode = parseExpression(); // Parse as an expression if not a SpecifierIdentifier
            if (!nextArgumentNode) {
                syntaxError("Expected a valid expression in the argument list.");
                return NULL;
            }
            addChild(argumentListNode, nextArgumentNode);
        }
    }

    return argumentListNode;
}



ParseTreeNode* parseParameterList() {
    printf("Parsing Parameter List...\n");

    // Create a node for the parameter list
    ParseTreeNode* parameterListNode = createParseTreeNode("ParameterList", "");

    // Peek at the next token to check if parameters are present
    Token* token = peekToken();
    if (!token || (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        // No parameters; return an empty ParameterList node
        return parameterListNode;
    }

    // Parse the first parameter
    ParseTreeNode* parameterNode = parseParameter();
    if (!parameterNode) {
        syntaxError("Expected a valid parameter in parameter list.");
        return NULL;
    }
    addChild(parameterListNode, parameterNode);

    // Parse additional parameters separated by commas
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        getNextToken(); // Consume the comma
        addChild(parameterListNode, createParseTreeNode("Delimiter", ",")); // Add the comma to the tree

        // Parse the next parameter
        parameterNode = parseParameter();
        if (!parameterNode) {
            syntaxError("Expected a valid parameter after ','.");
            return NULL;
        }
        addChild(parameterListNode, parameterNode);
    }

    return parameterListNode;
}



ParseTreeNode* parseParameter() {
    printf("Parsing Parameter...\n");

    // Create a parse tree node for the parameter
    ParseTreeNode* parameterNode = createParseTreeNode("Parameter", "");

    // Parse the type specifier (e.g., "int", "float", etc.)
    Token* token = peekToken();
    if (token && strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        getNextToken(); // Consume the type specifier
        addChild(parameterNode, createParseTreeNode("TypeSpecifier", token->value));
    } else {
        syntaxError("Expected type specifier for parameter");
        return parameterNode; // Return the node for error recovery
    }

    // Match and add the parameter name (IDENTIFIER or SpecifierIdentifier)
    token = peekToken();
    if (token && (strcmp(token->type, "SpecifierIdentifier") == 0 || strcmp(token->type, "IDENTIFIER") == 0)) {
        getNextToken(); // Consume the parameter name
        addChild(parameterNode, createParseTreeNode(token->type, token->value));
    } else {
        syntaxError("Expected parameter name (identifier or specifier identifier)");
        return parameterNode; // Return the node for error recovery
    }

    return parameterNode;
}



ParseTreeNode* parseReturnType(){

}



























// ---------------------------------------
// Error Handling
// ---------------------------------------

// Syntax Error Notice
void reportSyntaxError(const char* message) {
    // Retrieve the current token
    Token* token = peekToken();

    if (token) {
        // Print detailed error information
        printf("Syntax Error at line %d: %s\n", token->lineNumber, message);
        printf("DEBUG: Current Token - Type='%s', Value='%s'\n",
               token->type, token->value);
    } else {
        // Handle the case where no token is available
        printf("Syntax Error: %s\n", message);
        printf("DEBUG: No more tokens available for context.\n");
    }

    // Log additional debugging info if necessary
    printf("Attempting to recover...\n");
}


// Panic Mode Recovery
void recoverFromError() {
    printf("DEBUG: Initiating error recovery...\n");

    // Define recovery points
    const char* recoveryDelimiters[] = {";", "{", "}", NULL}; // Add more as needed
    const char* recoveryKeywords[] = {"if", "for", "while", "return", "input", "printf", "array", NULL};

    // Skip tokens until a recovery point is found
    Token* token;
    while ((token = peekToken()) != NULL) {
        // Check if the current token is a recovery delimiter
        if (strcmp(token->type, "Delimiter") == 0) {
            for (int i = 0; recoveryDelimiters[i] != NULL; i++) {
                if (strcmp(token->value, recoveryDelimiters[i]) == 0) {
                    printf("DEBUG: Recovered at delimiter: '%s' on line %d\n", token->value, token->lineNumber);
                    return; // Stop skipping tokens
                }
            }
        }

        // Check if the current token is a recovery keyword
        if (strcmp(token->type, "Keyword") == 0) {
            for (int i = 0; recoveryKeywords[i] != NULL; i++) {
                if (strcmp(token->value, recoveryKeywords[i]) == 0) {
                    printf("DEBUG: Recovered at keyword: '%s' on line %d\n", token->value, token->lineNumber);
                    return; // Stop skipping tokens
                }
            }
        }

        // Skip the token
        printf("DEBUG: Skipping Token: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);
        getNextToken();
    }

    // If no recovery point is found, report unrecoverable error
    printf("ERROR: Unable to recover from syntax error. Reached end of input.\n");
}































// Main function
int main(int argc, char* argv[]) {
    printf("\n\nStarting Syntax Analysis...\n");

    // Open the specified directory or current directory to look for .prsm files
    struct dirent *entry;
    DIR *dp = NULL;
    const char* directory = (argc > 1) ? argv[1] : ".";
    dp = opendir(directory);
    if (!dp) {
        printf("Error: Unable to open directory %s.\n", directory);
        return 1;
    }

    // List .prsm files
    char prsmFiles[100][256];
    int fileCount = 0;

    printf("Available .prsm files in directory '%s':\n", directory);
    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".prsm") && strlen(entry->d_name) > 5) {
            printf("%d. %s\n", fileCount + 1, entry->d_name);
            snprintf(prsmFiles[fileCount], sizeof(prsmFiles[fileCount]), "%s/%s", directory, entry->d_name);
            fileCount++;
        }
    }
    closedir(dp);

    if (fileCount == 0) {
        printf("No .prsm files found in directory %s.\n", directory);
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

    // Validate the root parse tree
    if (!root) {
        printf("Parsing failed. No parse tree generated.\n");
        return 1;
    }

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

    printf("\nSyntax Analysis Completed Successfully!\n");
    return 0;
}





