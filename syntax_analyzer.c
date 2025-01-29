#include <stdio.h>     // Standard I/O operations
#include <stdlib.h>    // Standard library functions
#include <string.h>    // String operations
#include <ctype.h>     // Character classification and conversion
#include <wctype.h>    // Wide character functions
#include <dirent.h>    // Directory operations
#include <stdbool.h>

#include "syntax_analyzer.h" // Custom syntax analyzer header
#include "token.h"           // Custom token header

// Global Variables
int currentTokenIndex = 0;        // Tracks the current token
int totalTokens = 0;              // Total tokens available
Token tokens[MAX_TOKENS];         // Token array
Token* tokenStream = tokens;      // Pointer to the token array
int skipToMatchingDelimiter(const char* delimiter);

// Function prototypes specific to syntax_analyzer.c
Token* peekNextToken();                      // Peek at the next token

// ---------------------------------------
// Utility Functions of Syntax Analyzer
// ---------------------------------------

// Function to peek at the next token without advancing the current index
Token* peekNextToken() {
    // Ensure there's at least one more token to peek
    if (currentTokenIndex + 1 < totalTokens) {
        return &tokens[currentTokenIndex + 1];
    }
    printf("DEBUG: No next token available. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
}

// Function to retrieve the next token
Token* getNextToken() {
    if (currentTokenIndex < totalTokens) {
        Token* currentToken = &tokens[currentTokenIndex];
        printf("[DEBUG] getNextToken: Retrieved Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex, currentToken->type, currentToken->value, currentToken->lineNumber);

        // Advance the token index
        currentTokenIndex++;

        // Check for whitespace or comments and skip if necessary
        while (currentTokenIndex < totalTokens) {
            Token* nextToken = &tokens[currentTokenIndex];
            if (strcmp(nextToken->type, "Whitespace") == 0 || strcmp(nextToken->type, "Comment") == 0) {
                printf("[DEBUG] Skipping Token[%d]: Type='%s', Value='%s', Line=%d\n",
                       currentTokenIndex, nextToken->type, nextToken->value, nextToken->lineNumber);
                currentTokenIndex++;
            } else {
                break;
            }
        }

        return currentToken;
    } else {
        printf("[DEBUG] getNextToken: End of token stream reached. Current Index=%d, Total Tokens=%d\n",
               currentTokenIndex, totalTokens);
        return NULL; // No more tokens available
    }
}

Token* peekToken() {
    static int previousTokenIndex = -1;
    static int repeatCounter = 0;

    if (currentTokenIndex < totalTokens) {
        Token* token = &tokens[currentTokenIndex];

        // Detect repetitive token peeks
        if (currentTokenIndex == previousTokenIndex) {
            repeatCounter++;
            if (repeatCounter > 10) { // Threshold to detect endless loops
                printf("[CRITICAL] Endless loop detected at Token[%d]: Type='%s', Value='%s', Line=%d\n",
                       currentTokenIndex, token->type, token->value, token->lineNumber);
                exit(1); // Forcefully terminate to analyze the issue
            }
        } else {
            repeatCounter = 0; // Reset counter if token index changes
        }

        previousTokenIndex = currentTokenIndex;

        // Debug output
        printf("[DEBUG] peekToken: Current Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex, token->type, token->value, token->lineNumber);
        return token;
    }

    printf("[DEBUG] peekToken: End of token stream reached. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
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
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

// Token maper function = Match the tokens from the symbol table and load them using the loadTokenFromFile
void mapToken(Token* token) {
    if (strcmp(token->type, "Delimiter") == 0) {
        if (strcmp(token->value, "(") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, "(");
        } else if (strcmp(token->value, ")") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, ")");
        } else if (strcmp(token->value, "{") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, "{");
        } else if (strcmp(token->value, "}") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, "}");
        } else if (strcmp(token->value, "[") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, "[");
        } else if (strcmp(token->value, "]") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, "]");
        } else if (strcmp(token->value, ";") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, ";");
        } else if (strcmp(token->value, ":") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, ":");
        } else if (strcmp(token->value, ",") == 0) {
            strcpy(token->type, "Delimiter");
            strcpy(token->value, ",");
        }
    } else if (strcmp(token->type, "ArithmeticOperator") == 0) {
        if (strcmp(token->value, "+") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "+");
        } else if (strcmp(token->value, "-") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "-");
        } else if (strcmp(token->value, "*") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "*");
        } else if (strcmp(token->value, "/") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "/");
        } else if (strcmp(token->value, "%") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "%");
        } else if (strcmp(token->value, "^") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "^");
        } else if (strcmp(token->value, "//") == 0) {
            strcpy(token->type, "ArithmeticOperator");
            strcpy(token->value, "//");
        }
    } else if (strcmp(token->type, "AssignmentOperator") == 0) {
        if (strcmp(token->value, "=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "=");
        } else if (strcmp(token->value, "+=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "+=");
        } else if (strcmp(token->value, "-=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "-=");
        } else if (strcmp(token->value, "*=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "*=");
        } else if (strcmp(token->value, "/=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "/=");
        } else if (strcmp(token->value, "%=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "%=");
        } else if (strcmp(token->value, "//=") == 0) {
            strcpy(token->type, "AssignmentOperator");
            strcpy(token->value, "//=");
        }
    } else if (strcmp(token->type, "LogicalOperator") == 0) {
        if (strcmp(token->value, "!") == 0) {
            strcpy(token->type, "LogicalOperator");
            strcpy(token->value, "!");
        } else if (strcmp(token->value, "||") == 0) {
            strcpy(token->type, "LogicalOperator");
            strcpy(token->value, "||");
        } else if (strcmp(token->value, "&&") == 0) {
            strcpy(token->type, "LogicalOperator");
            strcpy(token->value, "&&");
        }
    } else if (strcmp(token->type, "RelationalOperator") == 0) {
        if (strcmp(token->value, "==") == 0) {
            strcpy(token->type, "RelationalOperator");
            strcpy(token->value, "==");
        } else if (strcmp(token->value, "!=") == 0) {
            strcpy(token->type, "RelationalOperator");
            strcpy(token->value, "!=");
        } else if (strcmp(token->value, ">") == 0) {
            strcpy(token->type, "RelationalOperator");
            strcpy(token->value, ">");
        } else if (strcmp(token->value, "<") == 0) {
            strcpy(token->type, "RelationalOperator");
            strcpy(token->value, "<");
        } else if (strcmp(token->value, ">=") == 0) {
            strcpy(token->type, "RelationalOperator");
            strcpy(token->value, ">=");
        } else if (strcmp(token->value, "<=") == 0) {
            strcpy(token->type, "RelationalOperator");
            strcpy(token->value, "<=");
        }
    } else if (strcmp(token->type, "INT_LITERAL") == 0) {
        strcpy(token->type, "INT_LITERAL");
    } else if (strcmp(token->type, "FLOAT_LITERAL") == 0) {
        strcpy(token->type, "FLOAT_LITERAL");
    } else if (strcmp(token->type, "CHAR_LITERAL") == 0) {
        strcpy(token->type, "CHAR_LITERAL");
    } else if (strcmp(token->type, "STRING_LITERAL") == 0) {
        strcpy(token->type, "STRING_LITERAL");
    } else if (strcmp(token->type, "ReservedWord") == 0) {
        strcpy(token->type, "ReservedWord");
    } else if (strcmp(token->type, "NoiseWord") == 0) {
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

    printf("\n[DEBUG] Loading tokens from %s...\n", filename);

    while (fgets(line, sizeof(line), file)) {
        // Trim whitespace from the line
        trimWhitespace(line);

        // Skip empty lines
        if (strlen(line) == 0) {
            printf("[DEBUG] Skipping empty line.\n");
            continue;
        }

        // Parse the line into token fields (comma-separated)
        Token token;
        char type[50] = {0}, value[50] = {0};
        int lineNumber = 0;

        char *firstComma = strchr(line, ',');
        char *lastComma = strrchr(line, ',');

        // Handle invalid lines
        if (!firstComma || !lastComma || firstComma == lastComma) {
            reportSyntaxError("Invalid or unparsed token line.");
            continue; // Skip to the next line
        }

        // Extract fields
        *firstComma = '\0';
        strncpy(type, line, sizeof(type) - 1);
        *lastComma = '\0';
        strncpy(value, firstComma + 1, sizeof(value) - 1);
        lineNumber = atoi(lastComma + 1);

        // Populate the token
        strncpy(token.type, type, sizeof(token.type) - 1);
        strncpy(token.value, value, sizeof(token.value) - 1);
        token.lineNumber = lineNumber;

        // Map the token to handle value-specific tokens
        mapToken(&token);

        // Add the token to the list
        if (totalTokens < MAX_TOKENS) {
            tokens[totalTokens++] = token;
        } else {
            printf("Error: Token limit exceeded. Increase MAX_TOKENS.\n");
            break;
        }
    }

    fclose(file);

    // Print loaded tokens
    printf("\n[DEBUG] Total Tokens Loaded: %d\n", totalTokens);
    for (int i = 0; i < totalTokens; i++) {
        printf("[DEBUG] Token[%d]: Type='%s', Value='%s', Line=%d\n",
               i, tokens[i].type, tokens[i].value, tokens[i].lineNumber);
    }

    printf("[DEBUG] Completed loading tokens from %s.\n", filename);
    return totalTokens;
}

// Token Matching
ParseTreeNode* matchToken(const char* expectedType, const char* expectedValue) {
    printf("[DEBUG] Matching Token. Expected Type='%s', Value='%s'\n", expectedType, expectedValue);

    Token* token = peekToken(); // Peek the current token without advancing
    if (!token) {
        printf("[ERROR] No tokens available for matching.\n");
        reportSyntaxError("Unexpected end of input while matching token.");
        recoverFromError();
        return NULL;
    }

    // Log the current token
    printf("[DEBUG] Current Token: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Check if the token matches the expected type and value
    if (strcmp(token->type, expectedType) != 0 || strcmp(token->value, expectedValue) != 0) {
        printf("[ERROR] Token mismatch. Expected Type='%s', Value='%s'. Found Type='%s', Value='%s'\n",
               expectedType, expectedValue, token->type, token->value);
        char errorMessage[256];
        snprintf(errorMessage, sizeof(errorMessage),
                 "Syntax Error at line %d: Expected '%s' of type '%s' but found '%s' of type '%s'.",
                 token->lineNumber, expectedValue, expectedType, token->value, token->type);
        reportSyntaxError(errorMessage);
        recoverFromError();
        return NULL;
    }

    // If token matches, log the match
    printf("[DEBUG] Token matched successfully. Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    // Consume the token by advancing to the next
    token = getNextToken();
    if (!token) {
        printf("[WARNING] getNextToken returned NULL after successful match.\n");
    } else {
        printf("[DEBUG] Token advanced to: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);
    }

    // Create and return a parse tree node for the matched token
    ParseTreeNode* node = createParseTreeNode(expectedType, token->value);
    if (!node) {
        printf("[ERROR] Failed to create parse tree node for matched token.\n");
        reportSyntaxError("Internal error: Unable to create parse tree node.");
        recoverFromError();
        return NULL;
    }

    return node;
}


// ---------------------------------------
// Error Handling
// ---------------------------------------

// Syntax Error Notice
int reportSyntaxError(const char* message) {
    Token* token = peekToken();

    if (token) {
        // Apply token mapping if necessary
        mapToken(token); // Ensure the token is correctly mapped before displaying

        printf("Syntax Error at line %d: %s\n", token->lineNumber, message);
        printf("DEBUG: Current Token - Type='%s', Value='%s'\n",
               token->type, token->value);
    } else {
        printf("Syntax Error: %s\n", message);
        printf("DEBUG: No more tokens available for context.\n");
    }

    printf("Attempting to recover...\n");
}

// Panic Mode Recovery
int recoverFromError() {
    printf("DEBUG: Initiating error recovery...\n");

    // Recovery delimiters for statement endings or block boundaries
    const char* recoveryDelimiters[] = {";", "{", "}", ")", NULL};

    // Recovery keywords for starting new statements
    const char* recoveryKeywords[] = {"if", "else", "for", "while", "do", "return", "input", "printf", "array", "int", "float", "char", "bool", "string", NULL};

    Token* token;
    while ((token = peekToken()) != NULL) {
        printf("DEBUG: Token during recovery: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);

        // Recovery at delimiters: ';', '}', etc.
        if (strcmp(token->type, "Delimiter") == 0) {
            for (int i = 0; recoveryDelimiters[i] != NULL; i++) {
                if (strcmp(token->value, recoveryDelimiters[i]) == 0) {
                    printf("DEBUG: Recovered at delimiter: '%s' on line %d\n", token->value, token->lineNumber);
                    getNextToken(); // Consume the delimiter
                    return 1; // Recovery succeeded
                }
            }
        }

        // Recovery at keywords: 'if', 'else', etc.
        if (strcmp(token->type, "Keyword") == 0) {
            for (int i = 0; recoveryKeywords[i] != NULL; i++) {
                if (strcmp(token->value, recoveryKeywords[i]) == 0) {
                    printf("DEBUG: Recovered at keyword: '%s' on line %d\n", token->value, token->lineNumber);
                    return 1; // Recovery succeeded
                }
            }
        }

        // Skip over comments explicitly
        if (strcmp(token->type, "Comment") == 0) {
            printf("DEBUG: Skipping comment token during recovery: '%s' on line %d\n", token->value, token->lineNumber);
            getNextToken(); // Consume the comment token
            continue; // Continue recovery process
        }

        // Skip the current token if no recovery point is found
        printf("DEBUG: Skipping Token: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);
        getNextToken();
    }

    printf("ERROR: Unable to recover from syntax error. Reached end of input.\n");
    return 0; // Recovery failed
}

// ---------------------------------------
// Top-Level Grammar Rules
// ---------------------------------------

// Recursive Descent Parsing Function
// Recursive Descent Parsing Function
ParseTreeNode* parseProgram() {
    printf("[DEBUG] Starting parseProgram...\n");

    // Create the root node for the program
    ParseTreeNode* root = createParseTreeNode("Program", NULL);

    Token* lastToken = NULL;
    int loopSafetyCounter = 0;

    // Loop through the tokens and parse statements
    while (peekToken()) {
        Token* currentToken = peekToken();

        // Safety check to prevent infinite loops
        if (lastToken == currentToken) {
            loopSafetyCounter++;
            if (loopSafetyCounter > 100) {
                printf("[ERROR] Infinite loop detected at Token: Type='%s', Value='%s', Line=%d\n",
                       currentToken->type, currentToken->value, currentToken->lineNumber);
                break;
            }
        } else {
            loopSafetyCounter = 0; // Reset counter if token advances
        }
        lastToken = currentToken;

        printf("[DEBUG] Parsing statement starting with Token: Type='%s', Value='%s', Line=%d\n",
               currentToken->type, currentToken->value, currentToken->lineNumber);

        // Parse the current statement
        ParseTreeNode* statementNode = parseStatement();

        if (!statementNode) {
            printf("[WARNING] Failed to parse statement at Token: Type='%s', Value='%s', Line=%d\n",
                   currentToken->type, currentToken->value, currentToken->lineNumber);

            // Attempt recovery from error
            if (!recoverFromError()) {
                printf("[ERROR] Unable to recover from syntax error. Ending parsing.\n");
                break;
            }
            continue; // Retry parsing from the next valid token
        }

        // Add the successfully parsed statement to the program tree
        addChild(root, statementNode);
        printf("[DEBUG] Added child to root node. Current children count: %d\n", root->childCount);
    }

    printf("[DEBUG] Completed parsing program.\n");
    return root;
}

// ---------------------------------------
// Main Function
// ---------------------------------------

int main(int argc, char* argv[]) {

    // Global Variables - Store tokens and tracking variables
    currentTokenIndex = 0;
    totalTokens = 0;
    tokenStream = tokens;

    printf("\n\n[DEBUG] Starting Syntax Analysis...\n");

    // Open the specified directory or current directory to look for .prsm files
    struct dirent *entry;
    DIR *dp = NULL;
    const char* directory = (argc > 1) ? argv[1] : ".";
    dp = opendir(directory);
    if (!dp) {
        printf("Error: Unable to open directory %s.\n", directory);
        return 1;
    }

    // List .txt files
    char prsmFiles[100][256]; // Fixed buffer for simplicity
    int fileCount = 0;

    printf("Available .txt files in directory '%s':\n", directory);
    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".txt") && strlen(entry->d_name) > 5) {
            printf("%d. %s\n", fileCount + 1, entry->d_name);
            snprintf(prsmFiles[fileCount], sizeof(prsmFiles[fileCount]), "%s/%s", directory, entry->d_name);
            fileCount++;
        }
    }
    closedir(dp);

    if (fileCount == 0) {
        printf("No .txt files found in directory %s.\n", directory);
        return 1;
    }

    // Prompt user to select a file
    int choice;
    printf("\nEnter the number of the file to analyze (1-%d): ", fileCount);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > fileCount) {
        printf("Invalid choice. Exiting.\n");
        return 1;
    }

    // Load tokens from the selected .prsm file
    const char* selectedFile = prsmFiles[choice - 1];
    printf("\nSelected file: %s\n", selectedFile);
    if (!loadTokensFromFile(selectedFile)) {
        printf("Error: Failed to load tokens from file %s.\n", selectedFile);
        return 1;
    }

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

    writeParseTreeToFile(root, parseTreeFile, 0);
    fclose(parseTreeFile);

    printf("\nParse tree written to parse_tree.txt\n");

    // Free the parse tree
    freeParseTree(root);

    printf("\n[DEBUG] Syntax Analysis Completed Successfully!\n");
    return 0;
}

// ---------------------------------------
// Declaration Statements
// ---------------------------------------

ParseTreeNode* parseVariableDeclaration() {
    printf("[DEBUG] Parsing Variable Declaration...\n");

    // Create a node for the variable declaration
    ParseTreeNode* varDeclNode = createParseTreeNode("VariableDeclaration", "");

    // Match the type specifier (e.g., int, float, char, etc.)
    ParseTreeNode* typeSpecifierNode = parseTypeSpecifier();
    if (!typeSpecifierNode) {
        reportSyntaxError("Expected type specifier in variable declaration.");
        recoverFromError();
        freeParseTree(varDeclNode);
        return NULL;
    }
    addChild(varDeclNode, typeSpecifierNode); // Add type specifier to parse tree

    // Start parsing the <dec-list>
    while (true) {
        // Match identifier (variable name)
        Token* token = peekToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            reportSyntaxError("Expected an identifier in variable declaration.");
            recoverFromError();
            freeParseTree(varDeclNode);
            return NULL;
        }
        addChild(varDeclNode, matchToken("IDENTIFIER", token->value));

        // Check for optional initialization (e.g., `= 10` or `= x + y`)
        token = peekToken();
        if (token && strcmp(token->type, "AssignmentOperator") == 0) {
            printf("[DEBUG] Detected assignment operator for initialization.\n");
            addChild(varDeclNode, matchToken("AssignmentOperator", token->value)); // Match '='

            // Parse the full arithmetic or identifier expression
            ParseTreeNode* exprNode = parseExpression();
            if (!exprNode) {
                reportSyntaxError("Expected an initializer value (expression, identifier, or literal).");
                recoverFromError();
                freeParseTree(varDeclNode);
                return NULL;
            }
            addChild(varDeclNode, exprNode);
        }

        // Check for a comma (to continue with another variable) or semicolon (to end declaration)
        token = peekToken();
        if (!token) {
            reportSyntaxError("Unexpected end of input in variable declaration.");
            recoverFromError();
            freeParseTree(varDeclNode);
            return NULL;
        }

        if (strcmp(token->type, "Delimiter") == 0) {
            if (strcmp(token->value, ",") == 0) {
                printf("[DEBUG] Detected ',' for multiple variable declarations.\n");
                addChild(varDeclNode, matchToken("Delimiter", ",")); // Continue parsing more variables
            } else if (strcmp(token->value, ";") == 0) {
                printf("[DEBUG] Detected ';' to end variable declaration.\n");
                addChild(varDeclNode, matchToken("Delimiter", ";")); // End parsing
                break; // Exit loop as declaration ends
            } else {
                reportSyntaxError("Unexpected delimiter in variable declaration.");
                recoverFromError();
                freeParseTree(varDeclNode);
                return NULL;
            }
        } else {
            reportSyntaxError("Expected ',' or ';' in variable declaration.");
            recoverFromError();
            freeParseTree(varDeclNode);
            return NULL;
        }
    }

    printf("[DEBUG] Successfully parsed Variable Declaration.\n");
    return varDeclNode;
}



ParseTreeNode* parseBlock() {
    printf("[DEBUG] Parsing Block...\n");

    // Create a parse tree node for the block
    ParseTreeNode* blockNode = createParseTreeNode("Block", "");

    // Match '{' to start the block
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "{") != 0) {
        reportSyntaxError("Expected '{' to start block.");
        recoverFromError();
        freeParseTree(blockNode);
        return NULL;
    }
    addChild(blockNode, matchToken("Delimiter", "{"));

    // Parse statements inside the block
    while (true) {
        token = peekToken();
        if (!token) {
            reportSyntaxError("Unexpected end of input inside block.");
            recoverFromError();
            freeParseTree(blockNode);
            return NULL;
        }

        // Break on encountering '}' (end of block)
        if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
            break;
        }

        // Delegate statement parsing
        ParseTreeNode* statementNode = parseStatement();
        if (!statementNode) {
            printf("[DEBUG] Failed to parse statement inside block. Attempting recovery...\n");
            recoverFromError();
            continue; // Skip invalid statements and attempt to recover
        }

        // Add parsed statement to the block node
        addChild(blockNode, statementNode);
    }

    // Match '}' to close the block
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "}") != 0) {
        reportSyntaxError("Expected '}' to close block.");
        recoverFromError();
        freeParseTree(blockNode);
        return NULL;
    }
    addChild(blockNode, matchToken("Delimiter", "}"));

    printf("[DEBUG] Successfully parsed Block.\n");
    return blockNode;
}

ParseTreeNode* parseStatementList() {
    printf("[DEBUG] Parsing Statement List...\n");

    // Create a node for the statement list
    ParseTreeNode* statementListNode = createParseTreeNode("StatementList", "");

    while (peekToken()) {
        Token* token = peekToken();

        // Stop parsing when encountering a closing curly brace '}'
        if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
            printf("[DEBUG] End of statement list detected at '}'.\n");
            break;
        }

        // **Step 1: Try parsing a declaration statement first (as per grammar rule #24)**
        if (token && strcmp(token->type, "Keyword") == 0 &&
            (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
             strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
             strcmp(token->value, "string") == 0)) {
            
            printf("[DEBUG] Detected declaration keyword: '%s'. Parsing declaration statement...\n", token->value);
            ParseTreeNode* declarationNode = parseDeclarationStatement();
            if (!declarationNode) {
                reportSyntaxError("Failed to parse declaration statement.");
                recoverFromError();
                continue; // Attempt to parse the next valid statement
            }
            addChild(statementListNode, declarationNode);
            printf("[DEBUG] Added Declaration Statement to Statement List. Current children count: %d\n",
                   statementListNode->childCount);
            continue; // Move to the next statement
        }

        // **Step 2: Otherwise, parse a regular statement**
        printf("[DEBUG] Parsing a regular statement...\n");
        ParseTreeNode* statementNode = parseStatement();
        if (!statementNode) {
            reportSyntaxError("Failed to parse a statement in the statement list.");
            recoverFromError();
            continue; // Attempt to parse the next statement
        }

        addChild(statementListNode, statementNode);
        printf("[DEBUG] Added Statement to Statement List. Current children count: %d\n", statementListNode->childCount);
    }

    printf("[DEBUG] Successfully parsed Statement List.\n");
    return statementListNode;
}

ParseTreeNode* parseDeclarationStatement() {
    printf("[DEBUG] Parsing Declaration Statement...\n");

    // Create a node for the declaration statement
    ParseTreeNode* declarationNode = createParseTreeNode("DeclarationStatement", "");

    // Delegate to parseVariableDeclaration
    ParseTreeNode* varDeclNode = parseVariableDeclaration();
    if (!varDeclNode) {
        reportSyntaxError("Invalid variable declaration.");
        recoverFromError();
        freeParseTree(declarationNode);
        return NULL;
    }
    addChild(declarationNode, varDeclNode);

    printf("[DEBUG] Successfully parsed Declaration Statement.\n");
    return declarationNode;
}

ParseTreeNode* parseTypeSpecifier() {
    printf("[DEBUG] Parsing Type Specifier...\n");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
          strcmp(token->value, "string") == 0)) {
        reportSyntaxError("Expected a type specifier (int, float, char, bool, or string).");
        recoverFromError();
        return NULL;
    }

    // Match and consume the type specifier token, adding it as a parse tree node
    ParseTreeNode* typeSpecifierNode = matchToken("Keyword", token->value);
    if (!typeSpecifierNode) {
        reportSyntaxError("Failed to match a valid type specifier.");
        recoverFromError();
        return NULL;
    }

    // Debug: Log the matched type specifier
    printf("[DEBUG] Matched Type Specifier: %s\n", token->value);

    return typeSpecifierNode; // Return the matched parse tree node
}

// ---------------------------------------
// Statement Types
// ---------------------------------------

// Enhanced parseStatement Function
ParseTreeNode* parseStatement() {
    printf("[DEBUG] Parsing Statement...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing a statement.");
        recoverFromError();
        return NULL;
    }

    printf("[DEBUG] Current Token in parseStatement: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    ParseTreeNode* statementNode = NULL;

    // Match specific statement types
    if (strcmp(token->type, "Keyword") == 0) {
        if (strcmp(token->value, "if") == 0) {
            statementNode = parseConditionalStatement();
        } else if (strcmp(token->value, "input") == 0) {
            statementNode = parseInputStatement();
        } else if (strcmp(token->value, "printf") == 0) {
            statementNode = parseOutputStatement();
        } else if (strcmp(token->value, "for") == 0 || strcmp(token->value, "while") == 0 || strcmp(token->value, "do") == 0) {
            statementNode = parseIterativeStatement();
        } else if (strcmp(token->value, "return") == 0 || strcmp(token->value, "break") == 0 || strcmp(token->value, "continue") == 0) {
            statementNode = parseJumpStatement();
        } else if (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
                   strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
                   strcmp(token->value, "string") == 0) {
            // Handle variable declarations
            printf("[DEBUG] Detected declaration keyword: '%s'. Delegating to parseDeclarationStatement.\n", token->value);
            statementNode = parseDeclarationStatement();
        }
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Handle assignment statements
        Token* nextToken = peekNextToken();
        if (nextToken && strcmp(nextToken->type, "AssignmentOperator") == 0) {
            statementNode = parseAssignmentStatement();
        } else {
            reportSyntaxError("Unrecognized identifier usage. Expected an assignment.");
            recoverFromError();
            return NULL;
        }
    } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        statementNode = parseBlock();
    } else if (strcmp(token->type, "Comment") == 0) {
        statementNode = parseComment();
    } else {
        reportSyntaxError("Unrecognized statement.");
        recoverFromError();
        return NULL;
    }

    if (!statementNode) {
        printf("[WARNING] Failed to parse statement.\n");
        return NULL;
    }

    printf("[DEBUG] Successfully parsed a statement.\n");
    return statementNode;
}


ParseTreeNode* parseExponentialExpr() {
    printf("[DEBUG] Parsing Exponential Expression...\n");

    // Parse the base operand first
    ParseTreeNode* baseNode = parseBase(); // Base handles literals, identifiers, or grouped expressions
    if (!baseNode) {
        reportSyntaxError("Failed to parse the base of the exponential expression.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();

    // Check and process exponentiation operators (supporting chains of '^')
    while (token && strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "^") == 0) {
        printf("[DEBUG] Detected exponentiation operator '^'.\n");

        // Create a node for the exponential expression
        ParseTreeNode* exponentialNode = createParseTreeNode("ExponentialExpr", "^");

        // Add the base operand as the left child
        addChild(exponentialNode, baseNode);

        // Match and consume the exponentiation operator
        if (!matchToken("ArithmeticOperator", "^")) {
            reportSyntaxError("Failed to match the exponentiation operator '^'.");
            recoverFromError();
            freeParseTree(exponentialNode);
            return NULL;
        }

        // Parse the next operand (right-hand side of the exponentiation)
        ParseTreeNode* rightOperand = parseBase();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right operand in the exponential expression.");
            recoverFromError();
            freeParseTree(exponentialNode);
            return NULL;
        }
        addChild(exponentialNode, rightOperand);

        // Update baseNode to exponentialNode for potential recursion
        baseNode = exponentialNode;

        // Update token for the next iteration
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Exponential Expression.\n");
    return baseNode;
}

ParseTreeNode* parseInputStatement() {
    printf("[DEBUG] Parsing Input Statement...\n");

    // Create the node for the input statement
    ParseTreeNode* inputNode = createParseTreeNode("InputStatement", "");

    // Match "input"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "input") != 0) {
        reportSyntaxError("Expected 'input' in input statement.");
        recoverFromError();
        freeParseTree(inputNode);
        return NULL;
    }
    addChild(inputNode, matchToken("Keyword", "input"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'input'.");
        recoverFromError();
        freeParseTree(inputNode);
        return NULL;
    }
    addChild(inputNode, matchToken("Delimiter", "("));

    // Parse input-list (optional)
    token = peekToken();
    if (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        // If there's no immediate closing ')', attempt to parse an input list
        ParseTreeNode* inputListNode = parseInputList();
        if (!inputListNode) {
            reportSyntaxError("Invalid input list in input statement.");
            recoverFromError();
            freeParseTree(inputNode);
            return NULL;
        }
        addChild(inputNode, inputListNode);
    }

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' to close input statement.");
        recoverFromError();
        freeParseTree(inputNode);
        return NULL;
    }
    addChild(inputNode, matchToken("Delimiter", ")"));

    // Match ';'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' after input statement.");
        recoverFromError();
        freeParseTree(inputNode);
        return NULL;
    }
    addChild(inputNode, matchToken("Delimiter", ";"));

    printf("[DEBUG] Successfully parsed Input Statement.\n");
    return inputNode;
}

ParseTreeNode* parseExpressionList() {
    printf("[DEBUG] Parsing Expression List...\n");

    ParseTreeNode* expressionListNode = createParseTreeNode("ExpressionList", "");

    // Parse the first expression
    ParseTreeNode* expressionNode = parseExpression();
    if (!expressionNode) {
        reportSyntaxError("Invalid expression in expression list.");
        recoverFromError();
        freeParseTree(expressionListNode);
        return NULL;
    }
    addChild(expressionListNode, expressionNode);

    // Match and parse additional expressions separated by commas
    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        addChild(expressionListNode, matchToken("Delimiter", ",")); // Consume ','

        expressionNode = parseExpression();
        if (!expressionNode) {
            reportSyntaxError("Invalid expression in expression list.");
            recoverFromError();
            freeParseTree(expressionListNode);
            return NULL;
        }
        addChild(expressionListNode, expressionNode);

        token = peekToken(); // Update token for the next iteration
    }

    printf("[DEBUG] Successfully parsed Expression List.\n");
    return expressionListNode;
}

ParseTreeNode* parseInputList() {
    printf("[DEBUG] Parsing Input List...\n");

    ParseTreeNode* inputListNode = createParseTreeNode("InputList", "");

    // Parse the first format-variable pair
    ParseTreeNode* pairNode = parseFormatVariablePair();
    if (!pairNode) {
        reportSyntaxError("Expected a valid format-variable pair in input list.");
        recoverFromError();
        freeParseTree(inputListNode);
        return NULL;
    }
    addChild(inputListNode, pairNode);

    // Handle additional format-variable pairs separated by commas
    Token* token;
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        addChild(inputListNode, matchToken("Delimiter", ",")); // Add the comma

        pairNode = parseFormatVariablePair();
        if (!pairNode) {
            reportSyntaxError("Expected a valid format-variable pair after ',' in input list.");
            recoverFromError();
            freeParseTree(inputListNode);
            return NULL;
        }
        addChild(inputListNode, pairNode);
    }

    printf("[DEBUG] Successfully parsed Input List.\n");
    return inputListNode;
}

ParseTreeNode* parseAddressVariable() {
    printf("[DEBUG] Parsing Address Variable...\n");

    // Create a node for the address variable
    ParseTreeNode* addressNode = createParseTreeNode("AddressVariable", "");

    // Peek the token to check for '&'
    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing address variable.");
        recoverFromError();
        freeParseTree(addressNode);
        return NULL;
    }

    // Check if the token is incorrectly recognized as a single "SpecifierIdentifier"
    if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        printf("[DEBUG] Detected SpecifierIdentifier: '%s'\n", token->value);

        // Manually extract the '&' and the actual identifier
        if (token->value[0] == '&') {
            addChild(addressNode, createParseTreeNode("Delimiter", "&")); // Add '&' symbol

            // Extract the identifier from "&value" (skip '&' and use remaining string)
            char identifier[50];
            strcpy(identifier, token->value + 1); // Copy the rest after '&'

            // Ensure there's an actual identifier
            if (strlen(identifier) == 0) {
                reportSyntaxError("Expected variable name after '&' in address variable.");
                recoverFromError();
                freeParseTree(addressNode);
                return NULL;
            }

            // Add extracted identifier as a node
            addChild(addressNode, createParseTreeNode("IDENTIFIER", identifier));

            // Consume the "SpecifierIdentifier" token
            getNextToken();
        } else {
            reportSyntaxError("Malformed address variable: Expected '&' at the start.");
            recoverFromError();
            freeParseTree(addressNode);
            return NULL;
        }
    } else {
        // Standard parsing if lexer is correctly splitting '&' and the identifier
        if (strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "&") != 0) {
            reportSyntaxError("Expected '&' at the start of address variable.");
            recoverFromError();
            freeParseTree(addressNode);
            return NULL;
        }
        addChild(addressNode, matchToken("Delimiter", "&"));

        // Match the identifier following '&'
        token = peekToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            reportSyntaxError("Expected variable name after '&' in address variable.");
            recoverFromError();
            freeParseTree(addressNode);
            return NULL;
        }
        addChild(addressNode, matchToken("IDENTIFIER", token->value));
    }

    printf("[DEBUG] Successfully parsed Address Variable.\n");
    return addressNode;
}


ParseTreeNode* parseFormatVariablePair() {
    printf("[DEBUG] Parsing Format-Variable Pair...\n");

    ParseTreeNode* pairNode = createParseTreeNode("FormatVariablePair", "");

    // Match format string
    ParseTreeNode* formatStringNode = parseFormatString();
    if (!formatStringNode) {
        reportSyntaxError("Expected format string in format-variable pair.");
        recoverFromError();
        freeParseTree(pairNode);
        return NULL;
    }
    addChild(pairNode, formatStringNode);

    // Match ',' delimiter
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ",") != 0) {
        reportSyntaxError("Expected ',' in format-variable pair.");
        recoverFromError();
        freeParseTree(pairNode);
        return NULL;
    }
    addChild(pairNode, matchToken("Delimiter", ","));

    // Match address variable
    ParseTreeNode* addressNode = parseAddressVariable();
    if (!addressNode) {
        reportSyntaxError("Expected address variable in format-variable pair.");
        recoverFromError();
        freeParseTree(pairNode);
        return NULL;
    }
    addChild(pairNode, addressNode);

    printf("[DEBUG] Successfully parsed Format-Variable Pair.\n");
    return pairNode;
}

ParseTreeNode* parseOutputStatement() {
    printf("[DEBUG] Parsing Output Statement...\n");

    // Create the node for the output statement
    ParseTreeNode* outputNode = createParseTreeNode("OutputStatement", "");

    // Match "printf"
    if (!matchToken("Keyword", "printf")) {
        reportSyntaxError("Expected 'printf' in output statement.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, createParseTreeNode("Keyword", "printf"));

    // Match '('
    if (!matchToken("Delimiter", "(")) {
        reportSyntaxError("Expected '(' after 'printf'.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, createParseTreeNode("Delimiter", "("));

    // Parse the output list manually (instead of calling parseExpressionList)
    ParseTreeNode* outputListNode = createParseTreeNode("OutputList", "");

    Token* token = peekToken();
    while (token && (strcmp(token->type, "STRING_LITERAL") == 0 ||
                     strcmp(token->type, "IDENTIFIER") == 0 ||
                     strcmp(token->type, "ArithmeticOperator") == 0 ||
                     strcmp(token->type, "RelationalOperator") == 0 ||
                     strcmp(token->type, "LogicalOperator") == 0 ||
                     strcmp(token->type, "Delimiter") == 0)) {

        // Parse literals, identifiers, or expressions directly
        if (strcmp(token->type, "STRING_LITERAL") == 0) {
            addChild(outputListNode, matchToken("STRING_LITERAL", token->value));
        } else if (strcmp(token->type, "IDENTIFIER") == 0) {
            addChild(outputListNode, matchToken("IDENTIFIER", token->value));
        } else if (strcmp(token->value, "&") == 0) {  // Handle <address-variable>
            ParseTreeNode* addressVarNode = createParseTreeNode("AddressVariable", "&");
            addChild(addressVarNode, matchToken("Delimiter", "&"));

            token = peekToken();
            if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
                reportSyntaxError("Expected identifier after '&' in output statement.");
                recoverFromError();
                freeParseTree(addressVarNode);
                freeParseTree(outputListNode);
                freeParseTree(outputNode);
                return NULL;
            }
            addChild(addressVarNode, matchToken("IDENTIFIER", token->value));
            addChild(outputListNode, addressVarNode);
        } else {
            // If it's an expression, delegate to parseExpression()
            ParseTreeNode* exprNode = parseExpression();
            if (!exprNode) {
                reportSyntaxError("Expected valid expression in output statement.");
                recoverFromError();
                freeParseTree(outputListNode);
                freeParseTree(outputNode);
                return NULL;
            }
            addChild(outputListNode, exprNode);
        }

        // Peek for a comma (`,`) indicating more items
        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            addChild(outputListNode, matchToken("Delimiter", ","));
            token = peekToken(); // Move to the next expected value
        } else {
            break; // No more output items
        }
    }

    addChild(outputNode, outputListNode);

    // Match ')'
    if (!matchToken("Delimiter", ")")) {
        reportSyntaxError("Expected ')' after output list.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, createParseTreeNode("Delimiter", ")"));

    // Match the semicolon
    if (!matchToken("Delimiter", ";")) {
        reportSyntaxError("Expected ';' after output statement.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, createParseTreeNode("Delimiter", ";"));

    printf("[DEBUG] Successfully parsed Output Statement.\n");
    return outputNode;
}

ParseTreeNode* parseAssignmentStatement() {
    printf("[DEBUG] Parsing Assignment Statement...\n");

    // Create a node for the assignment statement
    ParseTreeNode* assignmentNode = createParseTreeNode("AssignmentStatement", "");

    // Match an identifier (the left-hand side of the assignment)
    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing assignment statement.");
        freeParseTree(assignmentNode);
        return NULL;
    }
    if (strcmp(token->type, "IDENTIFIER") != 0) {
        reportSyntaxError("Expected an identifier in assignment statement.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }
    printf("[DEBUG] Matching identifier for assignment: '%s'\n", token->value);
    addChild(assignmentNode, matchToken("IDENTIFIER", token->value));

    // Match the assignment operator (e.g., =, +=, -=, etc.)
    token = peekToken();
    if (!token || strcmp(token->type, "AssignmentOperator") != 0) {
        reportSyntaxError("Expected an assignment operator in assignment statement.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }
    printf("[DEBUG] Matching assignment operator: '%s'\n", token->value);
    addChild(assignmentNode, matchToken("AssignmentOperator", token->value));

    // Recursively parse expressions allowing chained assignments
    printf("[DEBUG] Parsing assignment expression for the assignment statement...\n");
    ParseTreeNode* exprNode = parseExpression();
    if (!exprNode) {
        reportSyntaxError("Expected an expression as the right-hand side of assignment.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }
    addChild(assignmentNode, exprNode);

    // **Handle Chained Assignments**
    token = peekToken();
    while (token && strcmp(token->type, "AssignmentOperator") == 0) {
        printf("[DEBUG] Detected Chained Assignment Operator: '%s'\n", token->value);

        // Create a new AssignmentStatement node
        ParseTreeNode* chainedAssignNode = createParseTreeNode("AssignmentStatement", "");
        addChild(chainedAssignNode, assignmentNode);  // Previous assignment becomes part of new assignment

        // Match assignment operator
        addChild(chainedAssignNode, matchToken("AssignmentOperator", token->value));

        // Parse the next expression (right-hand side)
        ParseTreeNode* nextExprNode = parseExpression();
        if (!nextExprNode) {
            reportSyntaxError("Expected expression after chained assignment operator.");
            recoverFromError();
            freeParseTree(chainedAssignNode);
            return NULL;
        }
        addChild(chainedAssignNode, nextExprNode);

        // Update the main assignment node
        assignmentNode = chainedAssignNode;
        
        // Peek for further chaining
        token = peekToken();
    }

    // Match the semicolon (statement terminator)
    token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while expecting ';' in assignment statement.");
        freeParseTree(assignmentNode);
        return NULL;
    }
    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0) {
        printf("[DEBUG] Matching semicolon at the end of assignment statement.\n");
        addChild(assignmentNode, matchToken("Delimiter", ";"));
    } else {
        reportSyntaxError("Expected ';' after assignment statement.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Assignment Statement.\n");
    return assignmentNode;
}

ParseTreeNode* parseConditionalStatement() {
    printf("[DEBUG] Parsing Conditional Statement...\n");

    // Create a node for the conditional statement
    ParseTreeNode* conditionalNode = createParseTreeNode("ConditionalStatement", "");

    // Match "if"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "if") != 0) {
        reportSyntaxError("Expected 'if' in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, matchToken("Keyword", "if"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'if'.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, matchToken("Delimiter", "("));

    // Match the Boolean expression
    ParseTreeNode* boolExprNode = parseBoolExpr();
    if (!boolExprNode) {
        reportSyntaxError("Invalid Boolean expression in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, boolExprNode);

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' after Boolean expression in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, matchToken("Delimiter", ")"));

    // Match the block or single statement
    ParseTreeNode* blockNode = parseBlock();
    if (!blockNode) {
        reportSyntaxError("Invalid block or statement in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, blockNode);

    // Check for "else" or "else if"
    token = peekToken();
    while (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "else") == 0) {
        addChild(conditionalNode, matchToken("Keyword", "else"));

        // Peek ahead to check for "if"
        token = peekToken();
        if (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "if") == 0) {
            addChild(conditionalNode, matchToken("Keyword", "if"));

            // Match '('
            token = peekToken();
            if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
                reportSyntaxError("Expected '(' after 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, matchToken("Delimiter", "("));

            // Match the Boolean expression
            boolExprNode = parseBoolExpr();
            if (!boolExprNode) {
                reportSyntaxError("Invalid Boolean expression in 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, boolExprNode);

            // Match ')'
            token = peekToken();
            if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
                reportSyntaxError("Expected ')' after Boolean expression in 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, matchToken("Delimiter", ")"));

            // Match the block or single statement
            blockNode = parseBlock();
            if (!blockNode) {
                reportSyntaxError("Invalid block or statement in 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, blockNode);

        } else {
            // Match the block or single statement for "else"
            blockNode = parseBlock();
            if (!blockNode) {
                reportSyntaxError("Invalid block or statement in 'else'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, blockNode);
            break; // Only one "else" is allowed
        }

        token = peekToken(); // Update token for the next iteration
    }

    printf("[DEBUG] Successfully parsed Conditional Statement.\n");
    return conditionalNode;
}

ParseTreeNode* parseIterativeStatement() {
    printf("[DEBUG] Parsing Iterative Statement...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing iterative statement.");
        recoverFromError();
        return NULL;
    }

    ParseTreeNode* iterativeNode = NULL;

    // Handle "for" loop
    if (strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "for") == 0) {
        printf("[DEBUG] Detected 'for' keyword. Delegating to parseForLoop().\n");
        iterativeNode = parseForLoop();
    }

    if (!iterativeNode) {
        reportSyntaxError("Invalid or unsupported iterative statement.");
        recoverFromError();
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Iterative Statement.\n");
    return iterativeNode;
}

ParseTreeNode* parseJumpStatement() {
    printf("[DEBUG] Parsing Jump Statement...\n");

    // Create the node for the jump statement
    ParseTreeNode* jumpNode = createParseTreeNode("JumpStatement", "");

    // Match "return", "break", or "continue"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        (strcmp(token->value, "return") != 0 &&
         strcmp(token->value, "break") != 0 &&
         strcmp(token->value, "continue") != 0)) {
        reportSyntaxError("Expected 'return', 'break', or 'continue' in jump statement.");
        recoverFromError();
        freeParseTree(jumpNode);
        return NULL;
    }

    addChild(jumpNode, matchToken("Keyword", token->value));

    // If the statement is "return", it may have an <expression>
    if (strcmp(token->value, "return") == 0) {
        ParseTreeNode* expressionNode = parseExpression();
        if (expressionNode) {
            addChild(jumpNode, expressionNode);
        } else {
            printf("[DEBUG] No expression found after 'return', which is acceptable.\n");
        }
    }

    // Match the semicolon
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' after jump statement.");
        recoverFromError();
        freeParseTree(jumpNode);
        return NULL;
    }
    addChild(jumpNode, matchToken("Delimiter", ";"));

    printf("[DEBUG] Successfully parsed Jump Statement.\n");
    return jumpNode;
}


// ---------------------------------------
// Conditional                                  
// ---------------------------------------

ParseTreeNode* parseStatementBlock() {
    printf("[DEBUG] Parsing Statement Block...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing Statement Block.");
        recoverFromError();
        return NULL;
    }

    ParseTreeNode* statementBlockNode = createParseTreeNode("StatementBlock", "");

    // If the next token is a `{`, parse it as a Block
    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        printf("[DEBUG] Detected '{', delegating to parseBlock().\n");
        ParseTreeNode* blockNode = parseBlock();
        if (!blockNode) {
            reportSyntaxError("Failed to parse Block in Statement Block.");
            recoverFromError();
            freeParseTree(statementBlockNode);
            return NULL;
        }
        addChild(statementBlockNode, blockNode);
    } else {
        // Otherwise, parse a single statement
        printf("[DEBUG] Detected standalone statement, delegating to parseStatement().\n");
        ParseTreeNode* statementNode = parseStatement();
        if (!statementNode) {
            reportSyntaxError("Failed to parse standalone statement in Statement Block.");
            recoverFromError();
            freeParseTree(statementBlockNode);
            return NULL;
        }
        addChild(statementBlockNode, statementNode);
    }

    printf("[DEBUG] Successfully parsed Statement Block.\n");
    return statementBlockNode;
}


ParseTreeNode* parseIfStatement() {
    printf("[DEBUG] Parsing If Statement...\n");

    // Create a node for the If Statement
    ParseTreeNode* ifNode = createParseTreeNode("IfStatement", "");

    // Match 'if' keyword
    if (!matchToken("Keyword", "if")) {
        reportSyntaxError("Expected 'if' keyword.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, createParseTreeNode("Keyword", "if"));

    // Match '('
    if (!matchToken("Delimiter", "(")) {
        reportSyntaxError("Expected '(' after 'if' keyword.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, createParseTreeNode("Delimiter", "("));

    // Parse condition
    ParseTreeNode* conditionNode = parseExpression();
    if (!conditionNode) {
        reportSyntaxError("Failed to parse condition in if statement.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, conditionNode);

    // Match ')'
    if (!matchToken("Delimiter", ")")) {
        reportSyntaxError("Expected ')' after condition in if statement.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, createParseTreeNode("Delimiter", ")"));

    // Parse 'if' statement block
    ParseTreeNode* ifBlockNode = parseStatementBlock();
    if (!ifBlockNode) {
        reportSyntaxError("Failed to parse statement block in if statement.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, ifBlockNode);

    // Check for optional 'else'
    Token* token = peekToken();
    if (token && strcmp(token->type, "Keyword") == 0 && strcmp(token->value, "else") == 0) {
        // Match 'else' keyword
        if (!matchToken("Keyword", "else")) {
            reportSyntaxError("Expected 'else' keyword.");
            recoverFromError();
            freeParseTree(ifNode);
            return NULL;
        }
        addChild(ifNode, createParseTreeNode("Keyword", "else"));

        // Parse 'else' statement block
        ParseTreeNode* elseBlockNode = parseStatementBlock();
        if (!elseBlockNode) {
            reportSyntaxError("Failed to parse statement block in else statement.");
            recoverFromError();
            freeParseTree(ifNode);
            return NULL;
        }
        addChild(ifNode, elseBlockNode);
    }

    printf("[DEBUG] Successfully parsed If Statement.\n");
    return ifNode;
}

// ---------------------------------------
// Loop                                         
// ---------------------------------------

// Parse For Loop
ParseTreeNode* parseForLoop() {
    printf("[DEBUG] Parsing For Loop...\n");

    // Create a node for the for loop
    ParseTreeNode* forLoopNode = createParseTreeNode("ForLoop", "");

    // Match "for"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "for") != 0) {
        reportSyntaxError("Expected 'for' keyword in for loop.");
        recoverFromError();
        freeParseTree(forLoopNode);
        return NULL;
    }
    addChild(forLoopNode, matchToken("Keyword", "for"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'for'.");
        recoverFromError();
        freeParseTree(forLoopNode);
        return NULL;
    }
    addChild(forLoopNode, matchToken("Delimiter", "("));

    // Parse optional noise words "from"
    token = peekToken();
    if (token && strcmp(token->type, "NoiseWord") == 0 && strcmp(token->value, "from") == 0) {
        addChild(forLoopNode, matchToken("NoiseWord", "from"));
    }

    // Parse initialization (forInit)
    ParseTreeNode* forInitNode = parseForInit();
    if (forInitNode) {
        addChild(forLoopNode, forInitNode);
    }

    // Match ';'
    addChild(forLoopNode, matchToken("Delimiter", ";"));

    // Parse condition (Expression instead of BoolExpr)
    ParseTreeNode* conditionNode = parseExpression();
    if (conditionNode) {
        addChild(forLoopNode, conditionNode);
    }

    // Match ';'
    addChild(forLoopNode, matchToken("Delimiter", ";"));

    // Parse optional noise words "until"
    token = peekToken();
    if (token && strcmp(token->type, "NoiseWord") == 0 && strcmp(token->value, "until") == 0) {
        addChild(forLoopNode, matchToken("NoiseWord", "until"));
    }

    // Parse optional update (forUpdate)
    ParseTreeNode* forUpdateNode = parseForUpdate();
    if (forUpdateNode) {
        addChild(forLoopNode, forUpdateNode);
    }

    // Match ')'
    addChild(forLoopNode, matchToken("Delimiter", ")"));

    // Parse block or single statement
    addChild(forLoopNode, parseBlock());

    printf("[DEBUG] Successfully parsed For Loop.\n");
    return forLoopNode;
}


ParseTreeNode* parseForInit() {
    printf("[DEBUG] Parsing For Init...\n");

    Token* token = peekToken();
    if (!token) {
        printf("[DEBUG] No tokens available for for-init.\n");
        return NULL;
    }

    ParseTreeNode* forInitNode = createParseTreeNode("ForInit", "");

    // Check for a variable declaration (for-var-declaration)
    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        printf("[DEBUG] For-init detected as a variable declaration.\n");

        // Parse type specifier
        ParseTreeNode* typeNode = parseTypeSpecifier();
        if (!typeNode) {
            reportSyntaxError("Expected type specifier in for-init declaration.");
            recoverFromError();
            freeParseTree(forInitNode);
            return NULL;
        }
        addChild(forInitNode, typeNode);

        // Match the identifier for the variable
        token = peekToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            reportSyntaxError("Expected identifier in for-init variable declaration.");
            recoverFromError();
            freeParseTree(forInitNode);
            return NULL;
        }
        addChild(forInitNode, matchToken("IDENTIFIER", token->value));

        // Optional initialization (for assignment)
        token = peekToken();
        if (token && strcmp(token->type, "AssignmentOperator") == 0) {
            addChild(forInitNode, matchToken("AssignmentOperator", token->value)); // Match assignment operator

            // Parse the initialization expression
            ParseTreeNode* exprNode = parseExpression();
            if (!exprNode) {
                reportSyntaxError("Expected expression for initialization in for-init declaration.");
                recoverFromError();
                freeParseTree(forInitNode);
                return NULL;
            }
            addChild(forInitNode, exprNode);
        }

        printf("[DEBUG] Successfully parsed variable declaration in for-init.\n");
        return forInitNode;
    }

    // Otherwise, check for an assignment (for-assignment)
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        printf("[DEBUG] For-init detected as an assignment.\n");

        // Match identifier
        ParseTreeNode* identifierNode = matchToken("IDENTIFIER", token->value);
        if (!identifierNode) {
            reportSyntaxError("Expected identifier in for-init assignment.");
            recoverFromError();
            freeParseTree(forInitNode);
            return NULL;
        }
        addChild(forInitNode, identifierNode);

        // Match assignment operator
        token = peekToken();
        if (!token || strcmp(token->type, "AssignmentOperator") != 0) {
            reportSyntaxError("Expected assignment operator in for-init assignment.");
            recoverFromError();
            freeParseTree(forInitNode);
            return NULL;
        }
        addChild(forInitNode, matchToken("AssignmentOperator", token->value));

        // Match expression
        ParseTreeNode* exprNode = parseExpression();
        if (!exprNode) {
            reportSyntaxError("Expected expression in for-init assignment.");
            recoverFromError();
            freeParseTree(forInitNode);
            return NULL;
        }
        addChild(forInitNode, exprNode);

        printf("[DEBUG] Successfully parsed assignment in for-init.\n");
        return forInitNode;
    }

    // No valid for-init found
    printf("[DEBUG] For Init is empty or invalid.\n");
    freeParseTree(forInitNode);
    return NULL;
}

ParseTreeNode* parseForUpdate() {
    printf("[DEBUG] Parsing For Update...\n");

    Token* token = peekToken();
    if (!token) {
        printf("[DEBUG] No tokens available for for-update.\n");
        return NULL;
    }

    ParseTreeNode* forUpdateNode = createParseTreeNode("ForUpdate", "");

    // Parse the first assignment statement
    ParseTreeNode* assignmentNode = parseUnaryExpr();
    if (!assignmentNode) {
        reportSyntaxError("Expected assignment statement in for-update.");
        recoverFromError();
        freeParseTree(forUpdateNode);
        return NULL;
    }
    addChild(forUpdateNode, assignmentNode);

    // Match and parse additional assignment statements separated by commas
    while (true) {
        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            addChild(forUpdateNode, matchToken("Delimiter", ",")); // Match and consume ','

            assignmentNode = parseAssignmentStatement();
            if (!assignmentNode) {
                reportSyntaxError("Expected assignment statement after ',' in for-update.");
                recoverFromError();
                freeParseTree(forUpdateNode);
                return NULL;
            }
            addChild(forUpdateNode, assignmentNode);
        } else {
            break; // No more updates
        }
    }

    printf("[DEBUG] Successfully parsed For Update.\n");
    return forUpdateNode;
}

// ---------------------------------------
// Expressions and Operators
// ---------------------------------------

ParseTreeNode* parseExpression() {
    printf("[DEBUG] Parsing Expression...\n");

    ParseTreeNode* expressionNode = createParseTreeNode("Expression", "");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing an Expression.");
        recoverFromError();
        freeParseTree(expressionNode);
        return NULL;
    }

    // Delegate to appropriate parsing function based on token type
    if (strcmp(token->type, "LogicalOperator") == 0 || strcmp(token->type, "RelationalOperator") == 0) {
        printf("[DEBUG] Delegating to parseBoolExpr...\n");
        ParseTreeNode* boolExprNode = parseBoolExpr();
        if (!boolExprNode) {
            reportSyntaxError("Failed to parse Boolean Expression.");
            recoverFromError();
            freeParseTree(expressionNode);
            return NULL;
        }
        addChild(expressionNode, boolExprNode);
    } else if (strcmp(token->type, "UnaryOperator") == 0) {
        printf("[DEBUG] Delegating to parseUnaryExpr...\n");
        ParseTreeNode* unaryExprNode = parseUnaryExpr();
        if (!unaryExprNode) {
            reportSyntaxError("Failed to parse Unary Expression.");
            recoverFromError();
            freeParseTree(expressionNode);
            return NULL;
        }
        addChild(expressionNode, unaryExprNode);
    } else {
        printf("[DEBUG] Delegating to parseArithmeticExpr...\n");
        ParseTreeNode* arithmeticExprNode = parseArithmeticExpr();
        if (!arithmeticExprNode) {
            reportSyntaxError("Failed to parse Arithmetic Expression.");
            recoverFromError();
            freeParseTree(expressionNode);
            return NULL;
        }
        addChild(expressionNode, arithmeticExprNode);
    }

    printf("[DEBUG] Successfully parsed Expression.\n");
    return expressionNode;
}


ParseTreeNode* parseBoolExpr() {
    printf("[DEBUG] Parsing Boolean Expression...\n");

    // Parse the left-hand side as a Boolean Term
    ParseTreeNode* leftOperand = parseBoolTerm();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of a Boolean Expression.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();
    // Continuously parse Logical OR (`||`) operations
    while (token && strcmp(token->type, "LogicalOperator") == 0 && strcmp(token->value, "||") == 0) {
        // Create a new node for the Logical OR expression
        ParseTreeNode* boolOrNode = createParseTreeNode("LogicalOrExpr", token->value);
        addChild(boolOrNode, leftOperand);

        // Match the `||` operator and consume it
        ParseTreeNode* operatorNode = matchToken("LogicalOperator", "||");
        if (!operatorNode) {
            reportSyntaxError("Failed to match Logical OR Operator (`||`).");
            recoverFromError();
            freeParseTree(boolOrNode);
            return NULL;
        }
        addChild(boolOrNode, operatorNode);

        // Parse the right-hand side as another Boolean Term
        ParseTreeNode* rightOperand = parseBoolTerm();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of a Logical OR Expression.");
            recoverFromError();
            freeParseTree(boolOrNode);
            return NULL;
        }
        addChild(boolOrNode, rightOperand);

        // Update the leftOperand for chaining OR operations
        leftOperand = boolOrNode;

        // Peek for further OR operators
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Boolean Expression.\n");
    return leftOperand;
}

ParseTreeNode* parseBoolTerm() {
    printf("[DEBUG] Parsing Boolean Term...\n");

    // Parse the left-hand side as a Boolean Factor
    ParseTreeNode* leftOperand = parseBoolFactor();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of a Boolean Term.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();
    // Continuously parse Logical AND (`&&`) operations
    while (token && strcmp(token->type, "LogicalOperator") == 0 && strcmp(token->value, "&&") == 0) {
        printf("[DEBUG] Detected Logical AND Operator '&&'.\n");

        // Create a new node for the Logical AND expression
        ParseTreeNode* boolAndNode = createParseTreeNode("LogicalAndExpr", token->value);
        addChild(boolAndNode, leftOperand);

        // Match the `&&` operator and consume it
        ParseTreeNode* operatorNode = matchToken("LogicalOperator", "&&");
        if (!operatorNode) {
            reportSyntaxError("Failed to match Logical AND Operator (`&&`).");
            recoverFromError();
            freeParseTree(boolAndNode);
            return NULL;
        }
        addChild(boolAndNode, operatorNode);

        // Parse the right-hand side as another Boolean Factor
        ParseTreeNode* rightOperand = parseBoolFactor();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of a Logical AND Expression.");
            recoverFromError();
            freeParseTree(boolAndNode);
            return NULL;
        }
        addChild(boolAndNode, rightOperand);

        // Update the leftOperand for chaining AND operations
        leftOperand = boolAndNode;

        // Peek for further AND operators
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Boolean Term.\n");
    return leftOperand;
}

ParseTreeNode* parseBoolFactor() {
    printf("[DEBUG] Parsing Boolean Factor...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing a Boolean Factor.");
        recoverFromError();
        return NULL;
    }

    // Handle Logical NOT (`!`) operator
    if (strcmp(token->type, "LogicalOperator") == 0 && strcmp(token->value, "!") == 0) {
        printf("[DEBUG] Detected Logical NOT Operator '!'.\n");

        // Create a node for the NOT operator
        ParseTreeNode* notNode = createParseTreeNode("LogicalNotExpr", "!");
        addChild(notNode, matchToken("LogicalOperator", "!")); // Match `!`

        // Parse the operand as another Boolean Factor
        ParseTreeNode* operand = parseBoolFactor();
        if (!operand) {
            reportSyntaxError("Failed to parse operand for Logical NOT.");
            recoverFromError();
            freeParseTree(notNode);
            return NULL;
        }
        addChild(notNode, operand);

        printf("[DEBUG] Successfully parsed Logical NOT Expression.\n");
        return notNode;
    }

    // Handle grouped boolean expressions `( <bool-expr> )`
    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
        printf("[DEBUG] Detected '(' indicating a grouped Boolean Expression.\n");

        // Create a node for the grouped expression
        ParseTreeNode* groupedExpr = createParseTreeNode("GroupedBoolExpr", "");
        addChild(groupedExpr, matchToken("Delimiter", "(")); // Match `(`

        // Parse the inner Boolean expression
        ParseTreeNode* innerExpr = parseBoolExpr();
        if (!innerExpr) {
            reportSyntaxError("Failed to parse inner Boolean Expression.");
            recoverFromError();
            freeParseTree(groupedExpr);
            return NULL;
        }
        addChild(groupedExpr, innerExpr);

        // Match `)`
        token = peekToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
            reportSyntaxError("Expected ')' to close grouped Boolean Expression.");
            recoverFromError();
            freeParseTree(groupedExpr);
            return NULL;
        }
        addChild(groupedExpr, matchToken("Delimiter", ")"));

        printf("[DEBUG] Successfully parsed grouped Boolean Expression.\n");
        return groupedExpr;
    }

    // Check for relational expressions
    ParseTreeNode* relationalExpr = parseRelationalExpr();
    if (relationalExpr) {
        printf("[DEBUG] Successfully parsed Relational Expression in Boolean Factor.\n");
        return relationalExpr;
    }

    // Check for boolean literals
    ParseTreeNode* boolLiteral = parseBoolLiteral();
    if (boolLiteral) {
        printf("[DEBUG] Successfully parsed Boolean Literal in Boolean Factor.\n");
        return boolLiteral;
    }

    // Handle arithmetic expressions (bool-expr can contain arithmetic comparisons)
    ParseTreeNode* arithmeticExpr = parseArithmeticExpr();
    if (arithmeticExpr) {
        printf("[DEBUG] Successfully parsed Arithmetic Expression in Boolean Factor.\n");
        return arithmeticExpr;
    }

    // Handle identifiers (boolean variables or function calls)
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        printf("[DEBUG] Detected Identifier: '%s'. Delegating to parseIdentifierExpr().\n", token->value);
        return parseIdentifierExpr();
    }

    reportSyntaxError("Expected a valid Boolean Factor (Logical NOT, Grouped Expression, Relational Expression, Literal, Identifier, or Arithmetic Expression).");
    recoverFromError();
    return NULL;
}


ParseTreeNode* parseArithmeticExpr() {
    printf("[DEBUG] Parsing Arithmetic Expression...\n");

    // Parse the left-hand side as a term
    ParseTreeNode* leftOperand = parseTerm(); // parseTerm() handles multiplication, division, modulo, etc.
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of an Arithmetic Expression.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();

    // Recursively handle addition and subtraction operators
    while (token && strcmp(token->type, "ArithmeticOperator") == 0 &&
           (strcmp(token->value, "+") == 0 || strcmp(token->value, "-") == 0)) {
        printf("[DEBUG] Detected addition/subtraction operator '%s'.\n", token->value);

        // Create a new node for the arithmetic expression
        ParseTreeNode* arithmeticNode = createParseTreeNode("ArithmeticExpr", "");

        // Add the left operand as a child
        addChild(arithmeticNode, leftOperand);

        // Match the operator and consume it
        addChild(arithmeticNode, matchToken("ArithmeticOperator", token->value));

        // Recursively parse the right-hand side
        ParseTreeNode* rightOperand = parseArithmeticExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of an Arithmetic Expression.");
            recoverFromError();
            freeParseTree(arithmeticNode);
            return NULL;
        }

        // Add the right operand as a child
        addChild(arithmeticNode, rightOperand);

        // Update the left operand for chaining operations
        leftOperand = arithmeticNode;

        // Peek at the next token to check for further addition/subtraction operators
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Arithmetic Expression.\n");
    return leftOperand;
}

ParseTreeNode* parseRelationalExpr() {
    printf("[DEBUG] Parsing Relational Expression...\n");

    // Parse the left-hand side as an arithmetic expression
    ParseTreeNode* leftOperand = parseArithmeticExpr();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of a Relational Expression.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();

    // Check and handle relational operators (==, !=, >, <, >=, <=)
    while (token && strcmp(token->type, "RelationalOperator") == 0 &&
           (strcmp(token->value, "==") == 0 || strcmp(token->value, "!=") == 0 ||
            strcmp(token->value, ">") == 0 || strcmp(token->value, "<") == 0 ||
            strcmp(token->value, ">=") == 0 || strcmp(token->value, "<=") == 0)) {
        printf("[DEBUG] Detected Relational Operator '%s'.\n", token->value);

        // Create a new node for the relational expression
        ParseTreeNode* relationalNode = createParseTreeNode("RelationalExpr", token->value);

        // Add the left operand as a child
        addChild(relationalNode, leftOperand);

        // Match and consume the relational operator
        if (!matchToken("RelationalOperator", token->value)) {
            reportSyntaxError("Failed to match Relational Operator.");
            recoverFromError();
            freeParseTree(relationalNode);
            return NULL;
        }

        // Parse the right-hand side as another arithmetic expression
        ParseTreeNode* rightOperand = parseArithmeticExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of a Relational Expression.");
            recoverFromError();
            freeParseTree(relationalNode);
            return NULL;
        }

        // Add the right operand as a child
        addChild(relationalNode, rightOperand);

        // Update leftOperand for potential chaining
        leftOperand = relationalNode;

        // Peek at the next token for further relational operators
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Relational Expression.\n");
    return leftOperand;
}

ParseTreeNode* parseUnaryExpr() {
    printf("[DEBUG] Parsing Unary Expression...\n");

    ParseTreeNode* unaryNode = createParseTreeNode("UnaryExpr", "");
    Token* token = peekToken();

    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing Unary Expression.");
        recoverFromError();
        freeParseTree(unaryNode);
        return NULL;
    }

    // Check for pre-increment or pre-decrement
    if (strcmp(token->type, "UnaryOperator") == 0 && 
        (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0)) {
        // Match the unary operator
        addChild(unaryNode, matchToken("UnaryOperator", token->value));

        // Match the identifier
        Token* nextToken = peekToken();
        if (!nextToken || strcmp(nextToken->type, "IDENTIFIER") != 0) {
            reportSyntaxError("Expected an identifier after Unary Operator.");
            recoverFromError();
            freeParseTree(unaryNode);
            return NULL;
        }
        addChild(unaryNode, matchToken("IDENTIFIER", nextToken->value));

        printf("[DEBUG] Successfully parsed Pre-Unary Expression.\n");
        return unaryNode;
    }

    // Check for post-increment or post-decrement
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Match the identifier
        addChild(unaryNode, matchToken("IDENTIFIER", token->value));

        // Match the unary operator
        Token* nextToken = peekToken();
        if (nextToken && strcmp(nextToken->type, "UnaryOperator") == 0 && 
            (strcmp(nextToken->value, "++") == 0 || strcmp(nextToken->value, "--") == 0)) {
            addChild(unaryNode, matchToken("UnaryOperator", nextToken->value));
            printf("[DEBUG] Successfully parsed Post-Unary Expression.\n");
            return unaryNode;
        }

        // If no unary operator follows, report syntax error
        reportSyntaxError("Expected Unary Operator (e.g., ++ or --) after Identifier.");
        recoverFromError();
        freeParseTree(unaryNode);
        return NULL;
    }

    // If no valid unary expression is found, report an error
    reportSyntaxError("Expected Unary Expression (e.g., ++identifier or identifier--).");
    recoverFromError();
    freeParseTree(unaryNode);
    return NULL;
}

ParseTreeNode* parseIdentifierExpr() {
    printf("[DEBUG] Parsing Identifier Expression...\n");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        reportSyntaxError("Expected an identifier in identifier expression.");
        recoverFromError();
        return NULL;
    }

    // Create the root node for the identifier expression
    ParseTreeNode* identifierExprNode = createParseTreeNode("IdentifierExpr", "");

    // Match the identifier and add it to the node
    addChild(identifierExprNode, matchToken("IDENTIFIER", token->value));

    printf("[DEBUG] Successfully parsed Identifier Expression.\n");
    return identifierExprNode;
}

ParseTreeNode* parseBase() {
    printf("[DEBUG] Parsing Base...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing Base.");
        recoverFromError();
        return NULL;
    }

    ParseTreeNode* baseNode = NULL;

    // Handle grouped expressions (parentheses)
    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
        printf("[DEBUG] Detected '(' indicating a grouped expression.\n");
        baseNode = createParseTreeNode("GroupedExpr", "");

        // Match '('
        addChild(baseNode, matchToken("Delimiter", "("));

        // Parse the inner arithmetic expression
        ParseTreeNode* innerExpr = parseArithmeticExpr();
        if (!innerExpr) {
            reportSyntaxError("Failed to parse inner arithmetic expression in grouped expression.");
            recoverFromError();
            freeParseTree(baseNode);
            return NULL;
        }
        addChild(baseNode, innerExpr);

        // Match ')'
        token = peekToken();
        if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
            reportSyntaxError("Expected ')' to close grouped expression.");
            recoverFromError();
            freeParseTree(baseNode);
            return NULL;
        }
        addChild(baseNode, matchToken("Delimiter", ")"));

        printf("[DEBUG] Successfully parsed grouped expression.\n");
        return baseNode;
    }

    // Handle literals (e.g., integers, floats, characters, strings)
    if (strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "FLOAT_LITERAL") == 0 ||
        strcmp(token->type, "CHAR_LITERAL") == 0 || strcmp(token->type, "STRING_LITERAL") == 0 ||
        (strcmp(token->type, "Keyword") == 0 && 
         (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0))) {
        printf("[DEBUG] Detected literal: Type='%s', Value='%s'\n", token->type, token->value);
        baseNode = parseLiteral();
        return baseNode;
    }

    // Handle identifiers (e.g., variable names)
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        printf("[DEBUG] Detected Identifier: '%s'\n", token->value);
        baseNode = createParseTreeNode("Identifier", token->value);
        addChild(baseNode, matchToken("IDENTIFIER", token->value));
        return baseNode;
    }

    // If none of the above cases match, report an error
    printf("[DEBUG] Token did not match any valid Base cases: Type='%s', Value='%s'\n", token->type, token->value);
    reportSyntaxError("Expected a valid Base (grouped expression, literal, or identifier).");
    recoverFromError();
    return NULL;
}

ParseTreeNode* parseFactor() {
    printf("[DEBUG] Parsing Factor...\n");

    // Parse the left-hand side as a base
    ParseTreeNode* baseNode = parseBase();
    if (!baseNode) {
        reportSyntaxError("Expected a base in factor.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();
    // Check for an exponentiation operator
    if (token && strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "^") == 0) {
        printf("[DEBUG] Detected Exponentiation Operator '%s'.\n", token->value);

        // Create a node for the factor (with exponentiation)
        ParseTreeNode* factorNode = createParseTreeNode("Factor", token->value);

        // Add the left base as a child
        addChild(factorNode, baseNode);

        // Match the exponentiation operator
        addChild(factorNode, matchToken("ArithmeticOperator", "^"));

        // Parse the right-hand side as another factor
        ParseTreeNode* rightFactor = parseFactor();
        if (!rightFactor) {
            reportSyntaxError("Failed to parse the right-hand side of the factor.");
            recoverFromError();
            freeParseTree(factorNode);
            return NULL;
        }
        addChild(factorNode, rightFactor);

        printf("[DEBUG] Successfully parsed Factor with Exponentiation.\n");
        return factorNode;
    }

    // If no exponentiation operator, return the base node
    printf("[DEBUG] Successfully parsed Factor without Exponentiation.\n");
    return baseNode;
}

ParseTreeNode* parseTerm() {
    printf("[DEBUG] Parsing Term...\n");

    // Parse the left-hand side as a factor
    ParseTreeNode* leftFactor = parseFactor();
    if (!leftFactor) {
        reportSyntaxError("Expected a factor in term.");
        recoverFromError();
        return NULL;
    }

    Token* token = peekToken();

    // Recursively handle multiplication, division, integer division, and modulo operators
    while (token && strcmp(token->type, "ArithmeticOperator") == 0 &&
           (strcmp(token->value, "*") == 0 || strcmp(token->value, "/") == 0 ||
            strcmp(token->value, "//") == 0 || strcmp(token->value, "%") == 0)) {
        printf("[DEBUG] Detected Term Operator '%s'.\n", token->value);

        // Create a node for the term operation
        ParseTreeNode* termNode = createParseTreeNode("Term", "");

        // Add the left factor as a child
        addChild(termNode, leftFactor);

        // Match the operator and consume it
        addChild(termNode, matchToken("ArithmeticOperator", token->value));

        // Recursively parse the right-hand side (ensuring proper precedence)
        ParseTreeNode* rightFactor = parseTerm();
        if (!rightFactor) {
            reportSyntaxError("Failed to parse the right-hand side of the term.");
            recoverFromError();
            freeParseTree(termNode);
            return NULL;
        }

        // Add the right operand as a child
        addChild(termNode, rightFactor);

        // Update the left operand for further chaining
        leftFactor = termNode;

        // Peek at the next token for further chaining
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Term.\n");
    return leftFactor;
}


ParseTreeNode* parseAssignExpr() {
    printf("[DEBUG] Parsing Assignment Expression...\n");

    // Create a parse tree node for the assignment expression
    ParseTreeNode* assignExprNode = createParseTreeNode("AssignExpr", "");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing assignment expression.");
        recoverFromError();
        freeParseTree(assignExprNode);
        return NULL;
    }

    ParseTreeNode* rhsNode = NULL;

    // Determine the type of the right-hand side and delegate parsing
    if (strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "FLOAT_LITERAL") == 0 ||
        strcmp(token->type, "CHAR_LITERAL") == 0 || strcmp(token->type, "STRING_LITERAL") == 0 ||
        (strcmp(token->type, "Keyword") == 0 && (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0))) {
        printf("[DEBUG] Parsing Literal as right-hand side of Assignment Expression.\n");
        rhsNode = parseLiteral();
    } else if (strcmp(token->type, "RelationalOperator") == 0 || strcmp(token->type, "LogicalOperator") == 0) {
        printf("[DEBUG] Parsing Boolean Expression as right-hand side of Assignment Expression.\n");
        rhsNode = parseBoolExpr();
    } else {
        printf("[DEBUG] Parsing Arithmetic Expression as right-hand side of Assignment Expression.\n");
        rhsNode = parseArithmeticExpr();
    }

    if (!rhsNode) {
        reportSyntaxError("Failed to parse right-hand side of assignment expression.");
        recoverFromError();
        freeParseTree(assignExprNode);
        return NULL;
    }
    addChild(assignExprNode, rhsNode);

    printf("[DEBUG] Successfully parsed Assignment Expression.\n");
    return assignExprNode;
}

// ---------------------------------------
// Literals and Identifiers                     
// ---------------------------------------

ParseTreeNode* parseBoolLiteral() {
    printf("[DEBUG] Parsing Boolean Literal...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing Boolean Literal.");
        recoverFromError();
        return NULL;
    }

    // Match "true" or "false"
    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        ParseTreeNode* boolLiteralNode = matchToken("Keyword", token->value);
        if (boolLiteralNode) {
            printf("[DEBUG] Successfully parsed Boolean Literal: '%s'.\n", token->value);
            return createParseTreeNode("BoolLiteral", boolLiteralNode->label);
        }
    }

    reportSyntaxError("Expected 'true' or 'false' as Boolean Literal.");
    recoverFromError();
    return NULL;
}

ParseTreeNode* parseLiteral() {
    printf("[DEBUG] Parsing Literal...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Expected a literal.");
        recoverFromError();
        return NULL;
    }

    if (strcmp(token->type, "INT_LITERAL") == 0 || strcmp(token->type, "FLOAT_LITERAL") == 0 ||
        strcmp(token->type, "CHAR_LITERAL") == 0 || strcmp(token->type, "STRING_LITERAL") == 0) {
        printf("[DEBUG] Matched numeric/character/string literal: %s\n", token->value);
        return createParseTreeNode("Literal", matchToken(token->type, token->value)->value);
    } else if (strcmp(token->type, "Keyword") == 0 &&
               (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        printf("[DEBUG] Detected boolean literal: %s\n", token->value);
        return parseBoolLiteral(); // Call `parseBoolLiteral` for "true" or "false"
    }

    reportSyntaxError("Invalid literal.");
    recoverFromError();
    return NULL;
}

// ---------------------------------------
// Comments
// ---------------------------------------

ParseTreeNode* parseComment() {
    printf("[DEBUG] Parsing Comment...\n");

    // Create a parse tree node for the comment
    ParseTreeNode* commentNode = createParseTreeNode("Comment", "");

    // Get the next token to determine the comment type
    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing comment.");
        freeParseTree(commentNode);
        return NULL;
    }

    if (strcmp(token->type, "Comment") == 0) {
        // Consume the comment token
        token = getNextToken();
        // Add the comment token value as a child node
        addChild(commentNode, createParseTreeNode("CommentContent", token->value));
        printf("[DEBUG] Parsed Comment: %s\n", token->value);
    } else {
        // Error for non-comment token
        char errorMessage[200];
        snprintf(errorMessage, sizeof(errorMessage),
                 "Expected a comment token but found Type='%s', Value='%s'.",
                 token->type, token->value);
        reportSyntaxError(errorMessage);
        freeParseTree(commentNode);
        return NULL;
    }

    return commentNode;
}

// ---------------------------------------
// Input and Output Specific Functions          
// ---------------------------------------

ParseTreeNode* parseFormatString() {
    printf("[DEBUG] Parsing Format String...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Expected format string but found end of input.");
        return NULL;
    }

    // Check if the token is a STRING_LITERAL
    if (strcmp(token->type, "STRING_LITERAL") == 0) {
        return createParseTreeNode("FormatString", matchToken("STRING_LITERAL", token->value)->value);
    }

    // Check for format specifier ("%d", "%f", etc.)
    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "%") == 0) {
        // Match '%'
        matchToken("Delimiter", "%");

        // Expect a valid format specifier (e.g., d, f, c, s)
        Token* specifierToken = peekToken();
        if (specifierToken && strcmp(specifierToken->type, "Keyword") == 0 &&
            (strcmp(specifierToken->value, "d") == 0 ||
             strcmp(specifierToken->value, "f") == 0 ||
             strcmp(specifierToken->value, "c") == 0 ||
             strcmp(specifierToken->value, "s") == 0)) {
            return createParseTreeNode("FormatString", matchToken("Keyword", specifierToken->value)->value);
        } else {
            reportSyntaxError("Invalid format specifier after '%'.");
            recoverFromError();
            return NULL;
        }
    }

    // If no valid format string, report an error
    reportSyntaxError("Invalid format string.");
    return NULL;
}

ParseTreeNode* parseOutputList() {
    printf("[DEBUG] Parsing Output List...\n");

    ParseTreeNode* outputListNode = createParseTreeNode("OutputList", "");

    // Parse the format string (optional)
    ParseTreeNode* formatStringNode = parseExpression();
    if (!formatStringNode) {
        reportSyntaxError("Invalid format string in output list.");
        recoverFromError();
        freeParseTree(outputListNode);
        return NULL;
    }
    addChild(outputListNode, formatStringNode);

    // Match optional ',' and parse the expression list
    Token* token = peekToken();
    while (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        addChild(outputListNode, matchToken("Delimiter", ",")); // Consume ','

        // Parse another expression
        ParseTreeNode* expressionNode = parseExpression();
        if (!expressionNode) {
            reportSyntaxError("Invalid expression in output list.");
            recoverFromError();
            freeParseTree(outputListNode);
            return NULL;
        }
        addChild(outputListNode, expressionNode);

        // Peek ahead for further expressions
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Output List.\n");
    return outputListNode;
}