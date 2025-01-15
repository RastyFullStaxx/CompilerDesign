#include <stdio.h>     // Standard I/O operations
#include <stdlib.h>    // Standard library functions
#include <string.h>    // String operations
#include <ctype.h>     // Character classification and conversion
#include <wctype.h>    // Wide character functions
#include <dirent.h>    // Directory operations

#include "syntax_analyzer.h" // Custom syntax analyzer header
#include "token.h"           // Custom token header

// Global Variables
int currentTokenIndex = 0;        // Tracks the current token
int totalTokens = 0;              // Total tokens available
Token tokens[MAX_TOKENS];         // Token array
Token* tokenStream = tokens;      // Pointer to the token array

// Function prototypes specific to syntax_analyzer.c
//void reportSyntaxError(const char* message); // Report syntax errors
//void recoverFromError();                     // Recover from syntax errors
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



Token* getNextToken() {
    if (currentTokenIndex < totalTokens) {
        Token* token = &tokens[currentTokenIndex++];
        
        // Map token to handle any necessary updates (e.g., parentheses or other delimiter handling)
        mapToken(token);

        // Debug output
        printf("[DEBUG] getNextToken: Retrieved Token[%d]: Type='%s', Value='%s', Line=%d\n",
               currentTokenIndex - 1, token->type, token->value, token->lineNumber);
        return token;
    }

    printf("[DEBUG] getNextToken: End of token stream reached. Current Index=%d, Total Tokens=%d\n",
           currentTokenIndex, totalTokens);
    return NULL;
}

Token* peekToken() {
    if (currentTokenIndex < totalTokens) {
        // Map token to handle any necessary updates (e.g., parentheses or other delimiter handling)
        Token* token = &tokens[currentTokenIndex];
        mapToken(token);

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




ParseTreeNode* matchToken(const char* expectedType, const char* expectedValue) {
    printf("[DEBUG] Matching Token. Expected Type='%s', Value='%s'\n", expectedType, expectedValue);

    Token* token = peekToken(); // Peek the current token without advancing
    if (!token) {
        reportSyntaxError("No more tokens available when matching.");
        recoverFromError(); // Attempt recovery if no token is available
        return NULL;
    }

    // Apply token mapping before comparison
    mapToken(token);

    // Check if the token matches the expected type and value
    if (strcmp(token->type, expectedType) != 0 || strcmp(token->value, expectedValue) != 0) {
        char errorMessage[200];
        snprintf(errorMessage, sizeof(errorMessage),
                 "Syntax Error at line %d: Expected '%s' of type '%s' but found '%s' of type '%s'.",
                 token->lineNumber, expectedValue, expectedType, token->value, token->type);
        reportSyntaxError(errorMessage);
        recoverFromError(); // Attempt recovery if token doesn't match
        return NULL;
    }

    printf("[DEBUG] Matched Token. Type='%s', Value='%s', Line=%d\n", token->type, token->value, token->lineNumber);

    // Consume the token
    token = getNextToken();

    // Create a parse tree node for the matched token
    ParseTreeNode* node = createParseTreeNode(token->type, token->value);
    if (!node) {
        reportSyntaxError("Failed to create parse tree node.");
        recoverFromError(); // Attempt recovery if node creation fails
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

    const char* recoveryDelimiters[] = {";", "{", "}", NULL};
    const char* recoveryKeywords[] = {"if", "for", "while", "return", "input", "printf", "array", NULL};

    Token* token;
    while ((token = peekToken()) != NULL) {
        printf("DEBUG: Token during recovery: Type='%s', Value='%s', Line=%d\n",
               token->type, token->value, token->lineNumber);

        // Recover at delimiters
        if (strcmp(token->type, "Delimiter") == 0) {
            for (int i = 0; recoveryDelimiters[i] != NULL; i++) {
                if (strcmp(token->value, recoveryDelimiters[i]) == 0) {
                    printf("DEBUG: Recovered at delimiter: '%s' on line %d\n", token->value, token->lineNumber);
                    getNextToken(); // Consume the delimiter
                    return 1; // Recovery succeeded
                }
            }
        }

        // Recover at keywords
        if (strcmp(token->type, "Keyword") == 0) {
            for (int i = 0; recoveryKeywords[i] != NULL; i++) {
                if (strcmp(token->value, recoveryKeywords[i]) == 0) {
                    printf("DEBUG: Recovered at keyword: '%s' on line %d\n", token->value, token->lineNumber);
                    return 1; // Recovery succeeded
                }
            }
        }

        // Skip the current token
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
ParseTreeNode* parseProgram() {
    printf("[DEBUG] Starting parseProgram...\n");

    ParseTreeNode* root = createParseTreeNode("Program", NULL);

    // Track last processed token to prevent infinite loops
    Token* lastToken = NULL;
    int loopSafetyCounter = 0;

    while (peekToken()) {
        Token* currentToken = peekToken();

        // Debugging to track token repetition
        if (lastToken == currentToken) {
            loopSafetyCounter++;
            if (loopSafetyCounter > 100) { // Adjust the threshold as needed
                printf("[ERROR] Infinite loop detected at Token: Type='%s', Value='%s', Line=%d\n",
                       currentToken->type, currentToken->value, currentToken->lineNumber);
                break;
            }
        } else {
            loopSafetyCounter = 0; // Reset the counter if the token changes
        }
        lastToken = currentToken;

        ParseTreeNode* statementNode = parseStatement();
        if (!statementNode) {
            printf("[DEBUG] Failed to parse statement. Continuing to next token...\n");
            continue;
        }
        addChild(root, statementNode);
    }

    printf("[DEBUG] Completed parsing program.\n");
    return root;
}







ParseTreeNode* parseBlock() {
    printf("\n[DEBUG] Parsing Block...\n");

    // Create the block node
    ParseTreeNode* blockNode = createParseTreeNode("Block", "");

    // Parse statements inside the block
    Token* token;
    printf("[DEBUG] Parsing statements inside block...\n");
    while ((token = peekToken()) && 
           !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}"))) {
        printf("[DEBUG] Parsing statement. Current Token: Type='%s', Value='%s', Line=%d\n",
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

        // Handle declarations
        if (strcmp(token->type, "Keyword") == 0 &&
            (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
             strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
             strcmp(token->value, "string") == 0)) {
            ParseTreeNode* declarationNode = parseDeclarationStatement();
            if (declarationNode) {
                addChild(blockNode, declarationNode);
            } else {
                reportSyntaxError("Error parsing declaration statement inside block.");
                recoverFromError();
            }
        } 
        // Handle assignment or expression
        else if (strcmp(token->type, "IDENTIFIER") == 0) {
            // Handle assignment expression
            ParseTreeNode* assignmentNode = parseAssignmentStatement();
            if (assignmentNode) {
                addChild(blockNode, assignmentNode);
            } else {
                reportSyntaxError("Error parsing assignment statement inside block.");
                recoverFromError();
            }
        }
        // Handle other statements
        else {
            ParseTreeNode* statementNode = parseStatement();
            if (statementNode) {
                addChild(blockNode, statementNode);
            } else {
                reportSyntaxError("Error parsing statement inside block.");
                recoverFromError();
                // Skip the current token and continue parsing subsequent statements
                getNextToken(); 
            }
        }
    }

    // Match the closing curly brace '}'
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "}") == 0) {
        printf("[DEBUG] Matched closing '}'.\n");
        getNextToken(); // Consume '}'
        addChild(blockNode, createParseTreeNode("Delimiter", "}"));
    } else {
        reportSyntaxError("Expected '}' at the end of block.");
        recoverFromError();
        freeParseTree(blockNode);
        return NULL;
    }

    printf("[DEBUG] Completed parsing Block.\n");
    return blockNode;
}


ParseTreeNode* parseStatementList() {
    printf("[DEBUG] Parsing Statement List...\n");

    ParseTreeNode* statementListNode = createParseTreeNode("StatementList", "");

    while (peekToken()) {
        ParseTreeNode* statementNode = parseStatement();
        if (!statementNode) {
            // Handle syntax error in statement
            reportSyntaxError("Error parsing statement in statement list.");
            recoverFromError();
            continue; // Attempt to parse the next statement
        } 
        addChild(statementListNode, statementNode);
    }

    printf("[DEBUG] Successfully parsed Statement List.\n");
    return statementListNode;
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

    // List .prsm files
    char prsmFiles[100][256]; // Fixed buffer for simplicity
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

    writeParseTreeToFile(root, parseTreeFile);
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

ParseTreeNode* parseDeclarationStatement() {
    printf("[DEBUG] Parsing Declaration Statement...\n");

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
        reportSyntaxError("Expected a valid type-specifier (e.g., int, float, etc.) in declaration statement.");
        recoverFromError();
        freeParseTree(declarationNode); // Cleanup memory
        return NULL;
    }
    addChild(declarationNode, matchToken("Keyword", token->value));

    // Parse the declarator list (IDENTIFIER with optional initialization)
    do {
        // Match an identifier
        token = peekToken();
        if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
            reportSyntaxError("Expected an identifier in declaration statement.");
            recoverFromError();
            freeParseTree(declarationNode); // Cleanup memory
            return NULL;
        }
        ParseTreeNode* declaratorNode = createParseTreeNode("Declarator", token->value);
        addChild(declarationNode, matchToken("IDENTIFIER", token->value));

        // Check for optional initialization
        token = peekToken();
        if (token && strcmp(token->type, "AssignmentOperator") == 0) {
            addChild(declaratorNode, matchToken("AssignmentOperator", token->value));

            // Match the initializer (literal or expression)
            token = peekToken();
            if (!token || (strcmp(token->type, "INT_LITERAL") != 0 &&
                           strcmp(token->type, "FLOAT_LITERAL") != 0 &&
                           strcmp(token->type, "CHAR_LITERAL") != 0 &&
                           strcmp(token->type, "STRING_LITERAL") != 0 &&
                           strcmp(token->type, "IDENTIFIER") != 0)) {
                reportSyntaxError("Expected a valid initializer (literal or identifier).");
                recoverFromError();
                freeParseTree(declaratorNode); // Cleanup memory
                return NULL;
            }
            addChild(declaratorNode, matchToken(token->type, token->value));
        }

        addChild(declarationNode, declaratorNode);

        // Check for a comma to continue the declaration list
        token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            addChild(declarationNode, matchToken("Delimiter", ","));
        } else {
            break; // No more declarators
        }

    } while (1);

    // Match the semicolon (';') at the end
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' at the end of the declaration statement.");
        recoverFromError();
        freeParseTree(declarationNode); // Cleanup memory
        return NULL;
    }
    addChild(declarationNode, matchToken("Delimiter", ";"));

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

ParseTreeNode* parseStatement() {
    printf("\n\n[DEBUG] Parsing Statement...\n");
    Token* token = peekToken();
    if (!token) {
        printf("[DEBUG] No more tokens available for parsing.\n");
        return NULL;
    }

    printf("[DEBUG] Current Token in parseStatement: Type='%s', Value='%s', Line=%d\n",
           token->type, token->value, token->lineNumber);

    ParseTreeNode* statementNode = NULL;

    // Match specific statement types
    if (strcmp(token->type, "Keyword") == 0) {
        if (strcmp(token->value, "if") == 0) {
            statementNode = parseIfStatement();
        } else if (strcmp(token->value, "input") == 0) {
            statementNode = parseInputStatement();
        } else if (strcmp(token->value, "printf") == 0) {
            statementNode = parseOutputStatement();
        } else if (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
                   strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
                   strcmp(token->value, "string") == 0) {
            statementNode = parseDeclarationStatement();
        } else if (strcmp(token->type, "Keyword") == 0) {
            if (strcmp(token->value, "for") == 0 || strcmp(token->value, "while") == 0 || strcmp(token->value, "do") == 0) {
            statementNode = parseIterativeStatement();
            }
        }
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // Handle assignment or function calls
        statementNode = parseAssignmentStatement();
    } else {
        // Fallback for unexpected tokens
        reportSyntaxError("Unrecognized statement.");
        recoverFromError();
        return NULL;
    }

    if (!statementNode) {
        // Error was already reported in the specific parsing function
        return NULL;
    }

    // Check if the statement needs a semicolon
    Token* delimiter = peekToken();
    if (delimiter && strcmp(delimiter->type, "Delimiter") == 0 && strcmp(delimiter->value, ";") == 0) {
        getNextToken(); // Consume the semicolon
        printf("[DEBUG] Successfully matched semicolon after statement.\n");
    } else {
        // Only report an error if the statement type requires a semicolon
        if (strcmp(statementNode->label, "CompoundStatement") != 0) {
            reportSyntaxError("Expected ';' after statement.");
            recoverFromError();
        }
    }

    printf("[DEBUG] Successfully parsed a statement.\n");
    return statementNode;
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

    // Match input-list (optional)
    if (peekToken()) {
        ParseTreeNode* inputListNode = parseInputList();
        if (inputListNode) {
            addChild(inputNode, inputListNode);
        } else {
            reportSyntaxError("Invalid input list in input statement.");
            recoverFromError();
            freeParseTree(inputNode);
            return NULL;
        }
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

    while (1) {
        ParseTreeNode* expressionNode = parseExpression();
        if (!expressionNode) {
            freeParseTree(expressionListNode);
            return NULL;
        }
        addChild(expressionListNode, expressionNode);

        Token* token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            addChild(expressionListNode, matchToken("Delimiter", ","));
        } else {
            break;  // No more expressions
        }
    }

    printf("[DEBUG] Successfully parsed Expression List.\n");
    return expressionListNode;
}



ParseTreeNode* parseStringConcat() {
    printf("[DEBUG] Parsing String Concatenation...\n");

    ParseTreeNode* concatNode = createParseTreeNode("StringConcat", "");

    // Match the first string literal or expression
    ParseTreeNode* firstPart = parseExpression();
    if (!firstPart) {
        freeParseTree(concatNode);
        return NULL;
    }
    addChild(concatNode, firstPart);

    // Match '+' and subsequent parts
    Token* token = peekToken();
    while (token && strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "+") == 0) {
        addChild(concatNode, matchToken("ArithmeticOperator", "+"));

        ParseTreeNode* nextPart = parseExpression();
        if (!nextPart) {
            reportSyntaxError("Expected expression after '+' in string concatenation.");
            recoverFromError();
            freeParseTree(concatNode);
            return NULL;
        }
        addChild(concatNode, nextPart);

        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed String Concatenation.\n");
    return concatNode;
}



ParseTreeNode* parseInputList() {
    printf("[DEBUG] Parsing Input List...\n");

    // Create the node for the input list
    ParseTreeNode* inputListNode = createParseTreeNode("InputList", "");

    while (1) {
        // Parse a format-variable pair
        ParseTreeNode* pairNode = parseFormatVariablePair();
        if (!pairNode) {
            freeParseTree(inputListNode);
            return NULL; // Error in parsing a pair
        }
        addChild(inputListNode, pairNode);

        // Check for a comma (',') indicating more pairs
        Token* token = peekToken();
        if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
            addChild(inputListNode, matchToken("Delimiter", ","));
        } else {
            break; // No more pairs
        }
    }

    printf("[DEBUG] Successfully parsed Input List.\n");
    return inputListNode;
}



ParseTreeNode* parseAddressVariable() {
    printf("[DEBUG] Parsing Address Variable...\n");

    ParseTreeNode* addressNode = createParseTreeNode("AddressVariable", "");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "SpecifierIdentifier") != 0) {
        reportSyntaxError("Expected '&' followed by variable name.");
        recoverFromError();
        freeParseTree(addressNode);
        return NULL;
    }
    addChild(addressNode, matchToken("SpecifierIdentifier", token->value));

    printf("[DEBUG] Successfully parsed Address Variable.\n");
    return addressNode;
}



ParseTreeNode* parseFormatVariablePair() {
    printf("[DEBUG] Parsing Format-Variable Pair...\n");

    ParseTreeNode* pairNode = createParseTreeNode("FormatVariablePair", "");

    // Match format string
    Token* token = peekToken();
    if (!token || strcmp(token->type, "STRING_LITERAL") != 0) {
        reportSyntaxError("Expected format string in format-variable pair.");
        recoverFromError();
        freeParseTree(pairNode);
        return NULL;
    }
    addChild(pairNode, matchToken("STRING_LITERAL", token->value));

    // Match ','
    token = peekToken();
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
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "printf") != 0) {
        reportSyntaxError("Expected 'printf' in output statement.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, matchToken("Keyword", "printf"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'printf'.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, matchToken("Delimiter", "("));

    // Match STRING_LITERAL
    ParseTreeNode* literalNode = parseLiteral(); // Use parseLiteral to handle STRING_LITERAL or other literals
    if (!literalNode) {
        reportSyntaxError("Expected a format string or expression in output statement.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, literalNode);

    // Handle additional expressions or concatenation
    token = peekToken();
    while (token && ((strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) ||
                     (strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "+") == 0))) {
        addChild(outputNode, matchToken(token->type, token->value));

        ParseTreeNode* expressionNode = parseExpression();
        if (!expressionNode) {
            reportSyntaxError("Expected expression after ',' or '+' in output statement.");
            recoverFromError();
            freeParseTree(outputNode);
            return NULL;
        }
        addChild(outputNode, expressionNode);

        token = peekToken(); // Update token after parsing expression
    }

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' to close output statement.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, matchToken("Delimiter", ")"));

    // Match ';'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' after output statement.");
        recoverFromError();
        freeParseTree(outputNode);
        return NULL;
    }
    addChild(outputNode, matchToken("Delimiter", ";"));

    printf("[DEBUG] Successfully parsed Output Statement.\n");
    return outputNode;
}



ParseTreeNode* parseFunctionStatement() {
    printf("[DEBUG] Parsing Function Statement...\n");

    // Create a parse tree node for the function statement
    ParseTreeNode* functionStmtNode = createParseTreeNode("FunctionStatement", "");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing function statement.");
        freeParseTree(functionStmtNode); // Clean up allocated memory
        return NULL;
    }

    // Check if it's a function declaration (starts with a type specifier or "void")
    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0 || strcmp(token->value, "void") == 0)) {
        printf("[DEBUG] Detected Function Declaration...\n");
        ParseTreeNode* functionDeclNode = parseFunctionDeclaration();
        if (!functionDeclNode) {
            reportSyntaxError("Error parsing function declaration.");
            recoverFromError();
            freeParseTree(functionStmtNode);
            return NULL;
        }
        addChild(functionStmtNode, functionDeclNode);
    }
    // Otherwise, check if it's a function call (starts with an identifier)
    else if (strcmp(token->type, "IDENTIFIER") == 0) {
        printf("[DEBUG] Detected Function Call...\n");
        ParseTreeNode* functionCallNode = parseFunctionCall();
        if (!functionCallNode) {
            reportSyntaxError("Error parsing function call.");
            recoverFromError();
            freeParseTree(functionStmtNode);
            return NULL;
        }
        addChild(functionStmtNode, functionCallNode);
    } else {
        // Handle unexpected token
        char errorMessage[200];
        snprintf(errorMessage, sizeof(errorMessage),
                 "Expected a function declaration or call, but found Type='%s', Value='%s'.",
                 token->type, token->value);
        reportSyntaxError(errorMessage);
        recoverFromError();
        freeParseTree(functionStmtNode);
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Function Statement.\n");
    return functionStmtNode;
}



ParseTreeNode* parseExpressionStatement() {
    printf("[DEBUG] Parsing Expression Statement...\n");

    // Create a parse tree node for the expression statement
    ParseTreeNode* expressionStatementNode = createParseTreeNode("ExpressionStatement", "");

    // Parse the expression
    ParseTreeNode* expressionNode = parseExpression();
    if (!expressionNode) {
        reportSyntaxError("Failed to parse the expression in the expression statement.");
        recoverFromError();
        freeParseTree(expressionStatementNode);
        return NULL;
    }
    addChild(expressionStatementNode, expressionNode);

    // Expect a semicolon to terminate the expression statement
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' at the end of the expression statement.");
        recoverFromError();
        freeParseTree(expressionStatementNode);
        return NULL;
    }
    addChild(expressionStatementNode, matchToken("Delimiter", ";"));

    printf("[DEBUG] Successfully parsed Expression Statement.\n");
    return expressionStatementNode;
}



ParseTreeNode* parseAssignmentStatement() {
    printf("[DEBUG] Parsing Assignment Statement...\n");

    // Create a node for the assignment statement
    ParseTreeNode* assignmentNode = createParseTreeNode("AssignmentStatement", "");

    // Match an identifier (the left-hand side of the assignment)
    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        reportSyntaxError("Expected an identifier in assignment statement.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }
    addChild(assignmentNode, matchToken("IDENTIFIER", token->value));

    // Match an assignment operator (e.g., =, +=, -=, etc.)
    token = peekToken();
    if (!token || strcmp(token->type, "AssignmentOperator") != 0) {
        reportSyntaxError("Expected an assignment operator in assignment statement.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }
    addChild(assignmentNode, matchToken("AssignmentOperator", token->value));

    // Match an expression (the right-hand side of the assignment)
    ParseTreeNode* expressionNode = parseExpression();
    if (!expressionNode) {
        reportSyntaxError("Expected an expression in assignment statement.");
        recoverFromError();
        freeParseTree(assignmentNode);
        return NULL;
    }
    addChild(assignmentNode, expressionNode);

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

    // Match the expression
    ParseTreeNode* expressionNode = parseExpression();
    if (!expressionNode) {
        reportSyntaxError("Invalid expression in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, expressionNode);

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' after expression in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, matchToken("Delimiter", ")"));

    // Match the statement block
    ParseTreeNode* statementBlockNode = parseStatementBlock();
    if (!statementBlockNode) {
        reportSyntaxError("Invalid statement block in conditional statement.");
        recoverFromError();
        freeParseTree(conditionalNode);
        return NULL;
    }
    addChild(conditionalNode, statementBlockNode);

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

            // Match the expression
            expressionNode = parseExpression();
            if (!expressionNode) {
                reportSyntaxError("Invalid expression in 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, expressionNode);

            // Match ')'
            token = peekToken();
            if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
                reportSyntaxError("Expected ')' after expression in 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, matchToken("Delimiter", ")"));

            // Match the statement block
            statementBlockNode = parseStatementBlock();
            if (!statementBlockNode) {
                reportSyntaxError("Invalid statement block in 'else if'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, statementBlockNode);

        } else {
            // Match the statement block for "else"
            statementBlockNode = parseStatementBlock();
            if (!statementBlockNode) {
                reportSyntaxError("Invalid statement block in 'else'.");
                recoverFromError();
                freeParseTree(conditionalNode);
                return NULL;
            }
            addChild(conditionalNode, statementBlockNode);
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
        printf("[DEBUG] No more tokens available for parsing iterative statement.\n");
        return NULL;
    }

    ParseTreeNode* iterativeNode = NULL;

    if (strcmp(token->type, "Keyword") == 0) {
        if (strcmp(token->value, "for") == 0) {
            iterativeNode = parseForLoop();
        } else if (strcmp(token->value, "while") == 0) {
            iterativeNode = parseWhileLoop();
        } else if (strcmp(token->value, "do") == 0) {
            iterativeNode = parseDoWhileLoop();
        }
    }

    if (!iterativeNode) {
        reportSyntaxError("Invalid iterative statement.");
        recoverFromError();
    }

    return iterativeNode;
}




// ---------------------------------------
// Conditional                                  
// ---------------------------------------

ParseTreeNode* parseStatementBlock() {
    printf("[DEBUG] Parsing Statement Block...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Expected statement or compound statement.");
        recoverFromError();
        return NULL;
    }

    ParseTreeNode* blockNode = NULL;

    if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        blockNode = parseCompoundStatement();
    } else {
        blockNode = parseStatement();
    }

    if (!blockNode) {
        reportSyntaxError("Failed to parse statement block.");
        recoverFromError();
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Statement Block.\n");
    return blockNode;
}



ParseTreeNode* parseIfStatement() {
    printf("[DEBUG] Parsing If Statement...\n");

    // Create the node for the if-statement
    ParseTreeNode* ifNode = createParseTreeNode("IfStatement", "");

    // Match "if"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "if") != 0) {
        reportSyntaxError("Expected 'if' in if statement.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, matchToken("Keyword", "if"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'if'.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, matchToken("Delimiter", "("));

    // Match the expression
    ParseTreeNode* expressionNode = parseExpression();
    if (!expressionNode) {
        reportSyntaxError("Invalid expression in if statement.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, expressionNode);

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' to close if condition.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, matchToken("Delimiter", ")"));

    // Parse the statement block
    ParseTreeNode* blockNode = parseStatementBlock();
    if (!blockNode) {
        reportSyntaxError("Expected statement block after if condition.");
        recoverFromError();
        freeParseTree(ifNode);
        return NULL;
    }
    addChild(ifNode, blockNode);

    printf("[DEBUG] Successfully parsed If Statement.\n");
    return ifNode;
}



// ParseTreeNode* parseSwitchStatement() {
//     printf("[DEBUG] Parsing Switch Statement...\n");
// 
//     ParseTreeNode* switchNode = createParseTreeNode("SwitchStatement", "");
// 
//     // Match "switch"
//     Token* token = peekToken();
//     if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "switch") != 0) {
//         reportSyntaxError("Expected 'switch' keyword.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, matchToken("Keyword", "switch"));
// 
//     // Match '('
//     token = peekToken();
//     if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
//         reportSyntaxError("Expected '(' after 'switch'.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, matchToken("Delimiter", "("));
// 
//     // Match <expression>
//     ParseTreeNode* exprNode = parseExpression();
//     if (!exprNode) {
//         reportSyntaxError("Invalid expression in 'switch'.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, exprNode);
// 
//     // Match ')'
//     token = peekToken();
//     if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
//         reportSyntaxError("Expected ')' after 'switch' expression.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, matchToken("Delimiter", ")"));
// 
//     // Match '{'
//     token = peekToken();
//     if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "{") != 0) {
//         reportSyntaxError("Expected '{' to open switch block.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, matchToken("Delimiter", "{"));
// 
//     // Match <case-list>
//     ParseTreeNode* caseListNode = parseCaseList();
//     if (!caseListNode) {
//         reportSyntaxError("Invalid case list in switch statement.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, caseListNode);
// 
//     // Match '}'
//     token = peekToken();
//     if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "}") != 0) {
//         reportSyntaxError("Expected '}' to close switch block.");
//         recoverFromError();
//         freeParseTree(switchNode);
//         return NULL;
//     }
//     addChild(switchNode, matchToken("Delimiter", "}"));
// 
//     printf("[DEBUG] Successfully parsed Switch Statement.\n");
//     return switchNode;
// }


ParseTreeNode* parseCompoundStatement() {
    printf("[DEBUG] Parsing Compound Statement...\n");

    ParseTreeNode* compoundNode = createParseTreeNode("CompoundStatement", "");

    // Match '{'
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "{") != 0) {
        reportSyntaxError("Expected '{' at the beginning of compound statement.");
        recoverFromError();
        freeParseTree(compoundNode);
        return NULL;
    }
    addChild(compoundNode, matchToken("Delimiter", "{"));

    // Parse statement list
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") != 0 && strcmp(token->value, "}") != 0) {
        ParseTreeNode* statementNode = parseStatement();
        if (!statementNode) {
            reportSyntaxError("Invalid statement in compound statement.");
            recoverFromError();
            continue;
        }
        addChild(compoundNode, statementNode);
    }

    // Match '}'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "}") != 0) {
        reportSyntaxError("Expected '}' to close compound statement.");
        recoverFromError();
        freeParseTree(compoundNode);
        return NULL;
    }
    addChild(compoundNode, matchToken("Delimiter", "}"));

    printf("[DEBUG] Successfully parsed Compound Statement.\n");
    return compoundNode;
}



// ---------------------------------------
// Loop                                         
// ---------------------------------------

ParseTreeNode* parseForLoop() {
    printf("[DEBUG] Parsing For Loop...\n");

    ParseTreeNode* forNode = createParseTreeNode("ForLoop", "");

    // Match "for"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "for") != 0) {
        reportSyntaxError("Expected 'for' in for-loop.");
        recoverFromError();
        freeParseTree(forNode);
        return NULL;
    }
    addChild(forNode, matchToken("Keyword", "for"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'for'.");
        recoverFromError();
        freeParseTree(forNode);
        return NULL;
    }
    addChild(forNode, matchToken("Delimiter", "("));

    // Match for-init (optional)
    token = peekToken();
    if (token && strcmp(token->type, "Keyword") == 0) {
        ParseTreeNode* forInitNode = parseDeclarationStatement();
        if (forInitNode) {
            addChild(forNode, forInitNode);
        }
    }

    // Match ';'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' after for-init.");
        recoverFromError();
        freeParseTree(forNode);
        return NULL;
    }
    addChild(forNode, matchToken("Delimiter", ";"));

    // Match condition (optional)
    ParseTreeNode* conditionNode = parseExpression();
    if (conditionNode) {
        addChild(forNode, conditionNode);
    }

    // Match ';'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' after condition in for-loop.");
        recoverFromError();
        freeParseTree(forNode);
        return NULL;
    }
    addChild(forNode, matchToken("Delimiter", ";"));

    // Match for-update (optional)
    ParseTreeNode* updateNode = parseExpression();
    if (updateNode) {
        addChild(forNode, updateNode);
    }

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' to close for-loop.");
        recoverFromError();
        freeParseTree(forNode);
        return NULL;
    }
    addChild(forNode, matchToken("Delimiter", ")"));

    // Match statement-block
    ParseTreeNode* blockNode = parseStatementBlock();
    if (!blockNode) {
        reportSyntaxError("Invalid statement block in for-loop.");
        recoverFromError();
        freeParseTree(forNode);
        return NULL;
    }
    addChild(forNode, blockNode);

    printf("[DEBUG] Successfully parsed For Loop.\n");
    return forNode;
}




ParseTreeNode* parseWhileLoop() {
    printf("[DEBUG] Parsing While Loop...\n");

    ParseTreeNode* whileNode = createParseTreeNode("WhileLoop", "");

    // Match "while"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "while") != 0) {
        reportSyntaxError("Expected 'while' in while-loop.");
        recoverFromError();
        freeParseTree(whileNode);
        return NULL;
    }
    addChild(whileNode, matchToken("Keyword", "while"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'while'.");
        recoverFromError();
        freeParseTree(whileNode);
        return NULL;
    }
    addChild(whileNode, matchToken("Delimiter", "("));

    // Match condition
    ParseTreeNode* conditionNode = parseExpression();
    if (!conditionNode) {
        reportSyntaxError("Expected condition in while-loop.");
        recoverFromError();
        freeParseTree(whileNode);
        return NULL;
    }
    addChild(whileNode, conditionNode);

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' to close while-loop.");
        recoverFromError();
        freeParseTree(whileNode);
        return NULL;
    }
    addChild(whileNode, matchToken("Delimiter", ")"));

    // Match statement-block
    ParseTreeNode* blockNode = parseStatementBlock();
    if (!blockNode) {
        reportSyntaxError("Invalid statement block in while-loop.");
        recoverFromError();
        freeParseTree(whileNode);
        return NULL;
    }
    addChild(whileNode, blockNode);

    printf("[DEBUG] Successfully parsed While Loop.\n");
    return whileNode;
}



ParseTreeNode* parseForInit() {
    printf("[DEBUG] Parsing For Init...\n");

    Token* token = peekToken();
    if (!token) {
        printf("[DEBUG] No tokens available for for-init.\n");
        return NULL;
    }

    ParseTreeNode* forInitNode = createParseTreeNode("ForInit", "");

    // Check for a declaration statement
    if (strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        ParseTreeNode* declNode = parseDeclarationStatement();
        if (declNode) {
            addChild(forInitNode, declNode);
            return forInitNode;
        }
    }

    // Otherwise, attempt to parse an assignment statement
    ParseTreeNode* assignmentNode = parseAssignmentStatement();
    if (assignmentNode) {
        addChild(forInitNode, assignmentNode);
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

    // Check for increment/decrement
    if (strcmp(token->type, "IDENTIFIER") == 0) {
        ParseTreeNode* incDecNode = parseIncDec();
        if (incDecNode) {
            addChild(forUpdateNode, incDecNode);
            return forUpdateNode;
        }
    }

    // Otherwise, attempt to parse an assignment statement
    ParseTreeNode* assignmentNode = parseAssignmentStatement();
    if (assignmentNode) {
        addChild(forUpdateNode, assignmentNode);
        return forUpdateNode;
    }

    // No valid for-update found
    printf("[DEBUG] For Update is empty or invalid.\n");
    freeParseTree(forUpdateNode);
    return NULL;
}


ParseTreeNode* parseIncDec() {
    printf("[DEBUG] Parsing Increment/Decrement...\n");

    ParseTreeNode* incDecNode = createParseTreeNode("IncDec", "");

    // Match identifier
    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        reportSyntaxError("Expected an identifier in increment/decrement.");
        recoverFromError();
        freeParseTree(incDecNode);
        return NULL;
    }
    addChild(incDecNode, matchToken("IDENTIFIER", token->value));

    // Match increment or decrement operator
    token = peekToken();
    if (!token || strcmp(token->type, "UnaryOperator") != 0 ||
        (strcmp(token->value, "++") != 0 && strcmp(token->value, "--") != 0)) {
        reportSyntaxError("Expected '++' or '--' in increment/decrement.");
        recoverFromError();
        freeParseTree(incDecNode);
        return NULL;
    }
    addChild(incDecNode, matchToken("UnaryOperator", token->value));

    printf("[DEBUG] Successfully parsed Increment/Decrement.\n");
    return incDecNode;
}


ParseTreeNode* parseDoWhileLoop() {
    printf("[DEBUG] Parsing Do-While Loop...\n");

    // Create the node for the do-while loop
    ParseTreeNode* doWhileNode = createParseTreeNode("DoWhileLoop", "");

    // Match "do"
    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "do") != 0) {
        reportSyntaxError("Expected 'do' in do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, matchToken("Keyword", "do"));

    // Match the statement block
    ParseTreeNode* statementBlockNode = parseStatementBlock();
    if (!statementBlockNode) {
        reportSyntaxError("Invalid statement block in do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, statementBlockNode);

    // Match "while"
    token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 || strcmp(token->value, "while") != 0) {
        reportSyntaxError("Expected 'while' in do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, matchToken("Keyword", "while"));

    // Match '('
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, "(") != 0) {
        reportSyntaxError("Expected '(' after 'while' in do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, matchToken("Delimiter", "("));

    // Match the condition expression
    ParseTreeNode* conditionNode = parseExpression();
    if (!conditionNode) {
        reportSyntaxError("Invalid condition in do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, conditionNode);

    // Match ')'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ")") != 0) {
        reportSyntaxError("Expected ')' after condition in do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, matchToken("Delimiter", ")"));

    // Match ';'
    token = peekToken();
    if (!token || strcmp(token->type, "Delimiter") != 0 || strcmp(token->value, ";") != 0) {
        reportSyntaxError("Expected ';' after do-while loop.");
        recoverFromError();
        freeParseTree(doWhileNode);
        return NULL;
    }
    addChild(doWhileNode, matchToken("Delimiter", ";"));

    printf("[DEBUG] Successfully parsed Do-While Loop.\n");
    return doWhileNode;
}



// ---------------------------------------
// Expressions and Operators
// ---------------------------------------

ParseTreeNode* parseExpression() {
    printf("[DEBUG] Parsing Expression...\n");

    // Create a node for the expression
    ParseTreeNode* exprNode = createParseTreeNode("Expression", "");

    // Parse logical OR expression (highest precedence)
    ParseTreeNode* logicalOrExprNode = parseLogicalOrExpr();
    if (!logicalOrExprNode) {
        reportSyntaxError("Failed to parse logical OR expression.");
        recoverFromError();
        freeParseTree(exprNode);
        return NULL;
    }
    addChild(exprNode, logicalOrExprNode);

    // Parse logical AND expression (second highest precedence)
    ParseTreeNode* logicalAndExprNode = parseLogicalAndExpr();
    if (logicalAndExprNode) {
        addChild(exprNode, logicalAndExprNode);
    }

    // Parse equality expression (third precedence)
    ParseTreeNode* equalityExprNode = parseEqualityExpr();
    if (equalityExprNode) {
        addChild(exprNode, equalityExprNode);
    }

    // Parse relational expression (fourth precedence)
    ParseTreeNode* relationalExprNode = parseRelationalExpr();
    if (relationalExprNode) {
        addChild(exprNode, relationalExprNode);
    }

    // Parse additive expression (fifth precedence)
    ParseTreeNode* additiveExprNode = parseAdditiveExpr();
    if (additiveExprNode) {
        addChild(exprNode, additiveExprNode);
    }

    // Parse multiplicative expression (sixth precedence)
    ParseTreeNode* multiplicativeExprNode = parseMultiplicativeExpr();
    if (multiplicativeExprNode) {
        addChild(exprNode, multiplicativeExprNode);
    }

    // Parse exponential expression (seventh precedence)
    ParseTreeNode* exponentialExprNode = parseExponentialExpr();
    if (exponentialExprNode) {
        addChild(exprNode, exponentialExprNode);
    }

    // Parse unary expression (lowest precedence among operators)
    ParseTreeNode* unaryExprNode = parseUnaryExpr();
    if (unaryExprNode) {
        addChild(exprNode, unaryExprNode);
    }

    // Finally, parse the primary expression (literals and identifiers)
    ParseTreeNode* primaryExprNode = parsePrimaryExpr();
    if (!primaryExprNode) {
        reportSyntaxError("Failed to parse primary expression.");
        recoverFromError();
        freeParseTree(exprNode);
        return NULL;
    }
    addChild(exprNode, primaryExprNode);

    printf("[DEBUG] Successfully parsed Expression.\n");
    return exprNode;
}





ParseTreeNode* parseLogicalOrExpr() {
    printf("[DEBUG] Parsing Logical OR Expression...\n");

    // Create a parse tree node for the Logical OR expression
    ParseTreeNode* logicalOrNode = createParseTreeNode("LogicalOrExpr", "");

    // Parse the left-hand side as a Logical AND expression
    ParseTreeNode* leftHandSide = parseLogicalAndExpr();
    if (!leftHandSide) {
        reportSyntaxError("Failed to parse the left-hand side of a Logical OR expression.");
        recoverFromError();
        freeParseTree(logicalOrNode);
        return NULL;
    }
    addChild(logicalOrNode, leftHandSide);

    // Check for multiple '||' operators
    while (peekToken() && strcmp(peekToken()->type, "LogicalOperator") == 0 && strcmp(peekToken()->value, "||") == 0) {
        // Match the '||' operator and add it to the tree
        ParseTreeNode* operatorNode = matchToken("LogicalOperator", "||");
        if (!operatorNode) {
            reportSyntaxError("Failed to match '||' operator in Logical OR expression.");
            recoverFromError();
            freeParseTree(logicalOrNode);
            return NULL;
        }
        addChild(logicalOrNode, operatorNode);

        // Parse the right-hand side as another Logical AND expression
        ParseTreeNode* rightHandSide = parseLogicalAndExpr();
        if (!rightHandSide) {
            reportSyntaxError("Failed to parse the right-hand side of a Logical OR expression.");
            recoverFromError();
            freeParseTree(logicalOrNode);
            return NULL;
        }
        addChild(logicalOrNode, rightHandSide);
    }

    printf("[DEBUG] Successfully parsed Logical OR Expression.\n");
    return logicalOrNode;
}



ParseTreeNode* parseLogicalAndExpr() {
    printf("[DEBUG] Parsing Logical AND Expression...\n");

    // Create a parse tree node for the Logical AND expression
    ParseTreeNode* logicalAndNode = createParseTreeNode("LogicalAndExpr", "");

    // Parse the left-hand side as an Equality expression
    ParseTreeNode* leftHandSide = parseEqualityExpr();
    if (!leftHandSide) {
        reportSyntaxError("Failed to parse the left-hand side of a Logical AND expression.");
        recoverFromError();
        freeParseTree(logicalAndNode);
        return NULL;
    }
    addChild(logicalAndNode, leftHandSide);

    // Continuously check for '&&' operators and parse accordingly
    while (peekToken() && strcmp(peekToken()->type, "LogicalOperator") == 0 && strcmp(peekToken()->value, "&&") == 0) {
        // Match and add the '&&' operator as a child node
        ParseTreeNode* operatorNode = matchToken("LogicalOperator", "&&");
        if (!operatorNode) {
            reportSyntaxError("Failed to match '&&' operator in Logical AND expression.");
            recoverFromError();
            freeParseTree(logicalAndNode);
            return NULL;
        }
        addChild(logicalAndNode, operatorNode);

        // Parse the right-hand side as another Equality expression
        ParseTreeNode* rightHandSide = parseEqualityExpr();
        if (!rightHandSide) {
            reportSyntaxError("Failed to parse the right-hand side of a Logical AND expression.");
            recoverFromError();
            freeParseTree(logicalAndNode);
            return NULL;
        }
        addChild(logicalAndNode, rightHandSide);
    }

    printf("[DEBUG] Successfully parsed Logical AND Expression.\n");
    return logicalAndNode;
}



ParseTreeNode* parseEqualityExpr() {
    printf("[DEBUG] Parsing Equality Expression...\n");

    // Create a parse tree node for the Equality expression
    ParseTreeNode* equalityNode = createParseTreeNode("EqualityExpr", "");

    // Parse the first operand (Relational expression)
    ParseTreeNode* leftOperand = parseRelationalExpr();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of an Equality expression.");
        recoverFromError();
        freeParseTree(equalityNode);
        return NULL;
    }
    addChild(equalityNode, leftOperand);

    // Check for equality or relational operators
    while (peekToken() && strcmp(peekToken()->type, "RelationalOperator") == 0) {
        // Match the operator
        ParseTreeNode* operatorNode = matchToken("RelationalOperator", peekToken()->value);
        if (!operatorNode) {
            reportSyntaxError("Failed to match a Relational Operator in Equality expression.");
            recoverFromError();
            freeParseTree(equalityNode);
            return NULL;
        }
        printf("[DEBUG] Matched Relational Operator: %s\n", operatorNode->value);

        // Add the operator node as a child
        addChild(equalityNode, operatorNode);

        // Parse the next operand (Relational expression)
        ParseTreeNode* rightOperand = parseRelationalExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of an Equality expression.");
            recoverFromError();
            freeParseTree(equalityNode);
            return NULL;
        }
        addChild(equalityNode, rightOperand);
    }

    printf("[DEBUG] Successfully parsed Equality Expression.\n");
    return equalityNode;
}



ParseTreeNode* parseRelationalExpr() {
    printf("[DEBUG] Parsing Relational Expression...\n");

    // Create a parse tree node for the Relational expression
    ParseTreeNode* relationalNode = createParseTreeNode("RelationalExpr", "");

    // Parse the first operand (Additive expression)
    ParseTreeNode* leftOperand = parseAdditiveExpr();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of a Relational expression.");
        recoverFromError();
        freeParseTree(relationalNode);
        return NULL;
    }
    addChild(relationalNode, leftOperand);

    // Check for relational operators (<, >, <=, >=, ==, !=)
    while (peekToken() && strcmp(peekToken()->type, "RelationalOperator") == 0 &&
           (strcmp(peekToken()->value, "<") == 0 || strcmp(peekToken()->value, ">") == 0 ||
            strcmp(peekToken()->value, "<=") == 0 || strcmp(peekToken()->value, ">=") == 0 ||
            strcmp(peekToken()->value, "==") == 0 || strcmp(peekToken()->value, "!=") == 0)) {
        // Consume the operator and add it to the parse tree
        addChild(relationalNode, matchToken("RelationalOperator", peekToken()->value));

        // Parse the next operand (Additive expression)
        ParseTreeNode* rightOperand = parseAdditiveExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of a Relational expression.");
            recoverFromError();
            freeParseTree(relationalNode);
            return NULL;
        }
        addChild(relationalNode, rightOperand);
    }

    printf("[DEBUG] Successfully parsed Relational Expression.\n");
    return relationalNode;
}



ParseTreeNode* parseAdditiveExpr() {
    printf("[DEBUG] Parsing Additive Expression...\n");

    // Create a parse tree node for the Additive expression
    ParseTreeNode* additiveNode = createParseTreeNode("AdditiveExpr", "");

    // Parse the first operand (Multiplicative expression)
    ParseTreeNode* leftOperand = parseMultiplicativeExpr();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of an Additive expression.");
        recoverFromError();
        freeParseTree(additiveNode);
        return NULL;
    }
    addChild(additiveNode, leftOperand);

    // Check for additive operators ('+', '-')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           (strcmp(peekToken()->value, "+") == 0 || strcmp(peekToken()->value, "-") == 0)) {
        // Match the operator and create a node for it
        ParseTreeNode* operatorNode = matchToken("ArithmeticOperator", peekToken()->value);
        if (!operatorNode) {
            reportSyntaxError("Failed to match Additive Operator in Additive expression.");
            recoverFromError();
            freeParseTree(additiveNode);
            return NULL;
        }
        printf("[DEBUG] Matched Additive Operator: %s\n", operatorNode->value);

        // Add the operator as a child node
        addChild(additiveNode, operatorNode);

        // Parse the next operand (Multiplicative expression)
        ParseTreeNode* rightOperand = parseMultiplicativeExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of an Additive expression.");
            recoverFromError();
            freeParseTree(additiveNode);
            return NULL;
        }
        addChild(additiveNode, rightOperand);
    }

    printf("[DEBUG] Successfully parsed Additive Expression.\n");
    return additiveNode;
}



ParseTreeNode* parseMultiplicativeExpr() {
    printf("[DEBUG] Parsing Multiplicative Expression...\n");

    // Create a parse tree node for the Multiplicative expression
    ParseTreeNode* multiplicativeNode = createParseTreeNode("MultiplicativeExpr", "");

    // Parse the first operand (Exponential expression)
    ParseTreeNode* leftOperand = parseExponentialExpr();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of a Multiplicative expression.");
        recoverFromError();
        freeParseTree(multiplicativeNode);
        return NULL;
    }
    addChild(multiplicativeNode, leftOperand);

    // Check for multiplicative operators ('*', '/', '%', '//')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           (strcmp(peekToken()->value, "*") == 0 || 
            strcmp(peekToken()->value, "/") == 0 || 
            strcmp(peekToken()->value, "%") == 0 || 
            strcmp(peekToken()->value, "//") == 0)) {
        // Match and add the operator as a child node
        ParseTreeNode* operatorNode = matchToken("ArithmeticOperator", peekToken()->value);
        if (!operatorNode) {
            reportSyntaxError("Failed to match Multiplicative Operator in Multiplicative expression.");
            recoverFromError();
            freeParseTree(multiplicativeNode);
            return NULL;
        }
        printf("[DEBUG] Matched Multiplicative Operator: %s\n", operatorNode->value);
        addChild(multiplicativeNode, operatorNode);

        // Parse the next operand (Exponential expression)
        ParseTreeNode* rightOperand = parseExponentialExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of a Multiplicative expression.");
            recoverFromError();
            freeParseTree(multiplicativeNode);
            return NULL;
        }
        addChild(multiplicativeNode, rightOperand);
    }

    printf("[DEBUG] Successfully parsed Multiplicative Expression.\n");
    return multiplicativeNode;
}



ParseTreeNode* parseExponentialExpr() {
    printf("[DEBUG] Parsing Exponential Expression...\n");

    // Create a parse tree node for the Exponential expression
    ParseTreeNode* exponentialNode = createParseTreeNode("ExponentialExpr", "");

    // Parse the first operand (Unary expression)
    ParseTreeNode* leftOperand = parseUnaryExpr();
    if (!leftOperand) {
        reportSyntaxError("Failed to parse the left-hand side of an Exponential expression.");
        recoverFromError();
        freeParseTree(exponentialNode);
        return NULL;
    }
    addChild(exponentialNode, leftOperand);

    // Check for the exponential operator ('^')
    while (peekToken() && strcmp(peekToken()->type, "ArithmeticOperator") == 0 &&
           strcmp(peekToken()->value, "^") == 0) {
        // Consume the operator and create a node for it
        ParseTreeNode* operatorNode = matchToken("ArithmeticOperator", "^");
        if (!operatorNode) {
            reportSyntaxError("Failed to match the Exponential operator '^' in Exponential expression.");
            recoverFromError();
            freeParseTree(exponentialNode);
            return NULL;
        }
        printf("[DEBUG] Matched Exponential Operator: %s\n", "^");

        // Add the operator as a child node
        addChild(exponentialNode, operatorNode);

        // Parse the next operand (Unary expression)
        ParseTreeNode* rightOperand = parseUnaryExpr();
        if (!rightOperand) {
            reportSyntaxError("Failed to parse the right-hand side of an Exponential expression.");
            recoverFromError();
            freeParseTree(exponentialNode);
            return NULL;
        }
        addChild(exponentialNode, rightOperand);
    }

    printf("[DEBUG] Successfully parsed Exponential Expression.\n");
    return exponentialNode;
}




ParseTreeNode* parseUnaryExpr() {
    printf("[DEBUG] Parsing Unary Expression...\n");

    // Create a parse tree node for the Unary Expression
    ParseTreeNode* unaryNode = createParseTreeNode("UnaryExpr", "");

    Token* token = peekToken();

    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing Unary Expression.");
        recoverFromError();
        freeParseTree(unaryNode);
        return NULL;
    }

    // Check for unary operators (Logical NOT `!`, Subtraction `-`, Increment `++`, Decrement `--`)
    if ((strcmp(token->type, "LogicalOperator") == 0 && strcmp(token->value, "!") == 0) ||
        (strcmp(token->type, "ArithmeticOperator") == 0 && strcmp(token->value, "-") == 0) ||
        (strcmp(token->type, "UnaryOperator") == 0 && 
         (strcmp(token->value, "++") == 0 || strcmp(token->value, "--") == 0))) {

        // Match the unary operator and add it as a child node
        ParseTreeNode* operatorNode = matchToken(token->type, token->value);
        if (!operatorNode) {
            reportSyntaxError("Failed to match Unary Operator in Unary Expression.");
            recoverFromError();
            freeParseTree(unaryNode);
            return NULL;
        }
        printf("[DEBUG] Matched Unary Operator: %s\n", token->value);
        addChild(unaryNode, operatorNode);

        // Parse the operand (recursively handle unary expressions)
        ParseTreeNode* operand = parseUnaryExpr();
        if (!operand) {
            reportSyntaxError("Failed to parse operand in Unary Expression.");
            recoverFromError();
            freeParseTree(unaryNode);
            return NULL;
        }
        addChild(unaryNode, operand);
    } else if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        // Handle SpecifierIdentifier (e.g., &number)
        ParseTreeNode* specifierNode = matchToken("SpecifierIdentifier", token->value);
        if (!specifierNode) {
            reportSyntaxError("Failed to match SpecifierIdentifier in Unary Expression.");
            recoverFromError();
            freeParseTree(unaryNode);
            return NULL;
        }
        printf("[DEBUG] Matched SpecifierIdentifier: %s\n", token->value);
        addChild(unaryNode, specifierNode);
    } else {
        // If no unary operator, parse as a postfix expression
        ParseTreeNode* postfixExprNode = parsePostfixExpr();
        if (!postfixExprNode) {
            reportSyntaxError("Failed to parse Postfix Expression in Unary Expression.");
            recoverFromError();
            freeParseTree(unaryNode);
            return NULL;
        }
        addChild(unaryNode, postfixExprNode);
    }

    printf("[DEBUG] Successfully parsed Unary Expression.\n");
    return unaryNode;
}



ParseTreeNode* parsePostfixExpr() {
    printf("[DEBUG] Parsing Postfix Expression...\n");

    // Create a parse tree node for the Postfix Expression
    ParseTreeNode* postfixNode = createParseTreeNode("PostfixExpr", "");

    // Parse the primary expression and add it as a child
    ParseTreeNode* primaryNode = parsePrimaryExpr();
    if (!primaryNode) {
        reportSyntaxError("Failed to parse Primary Expression in Postfix Expression.");
        recoverFromError();
        freeParseTree(postfixNode);
        return NULL;
    }
    addChild(postfixNode, primaryNode);

    // Handle optional postfix operations: function calls, unary operators, or array access
    Token* token = peekToken();
    while (token &&
           ((strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) ||
            strcmp(token->type, "UnaryOperator") == 0 ||
            (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[")))) {

        if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
            printf("[DEBUG] Parsing Function Call in Postfix Expression...\n");

            // Create a node for the function call
            ParseTreeNode* funcCallNode = createParseTreeNode("FunctionCall", "");

            // Match '('
            ParseTreeNode* leftParenNode = matchToken("Delimiter", "(");
            if (!leftParenNode) {
                reportSyntaxError("Expected '(' to start function arguments.");
                recoverFromError();
                freeParseTree(postfixNode);
                return NULL;
            }
            addChild(funcCallNode, leftParenNode);

            // Parse the argument list
            ParseTreeNode* argumentListNode = parseArgumentList();
            if (!argumentListNode) {
                reportSyntaxError("Failed to parse arguments in function call.");
                recoverFromError();
                freeParseTree(postfixNode);
                return NULL;
            }
            addChild(funcCallNode, argumentListNode);

            // Match ')'
            ParseTreeNode* rightParenNode = matchToken("Delimiter", ")");
            if (!rightParenNode) {
                reportSyntaxError("Expected ')' after function arguments.");
                recoverFromError();
                freeParseTree(postfixNode);
                return NULL;
            }
            addChild(funcCallNode, rightParenNode);

            // Add the function call node as a child of the postfix node
            addChild(postfixNode, funcCallNode);

        } else if (strcmp(token->type, "UnaryOperator") == 0) {
            printf("[DEBUG] Parsing Unary Operator in Postfix Expression...\n");

            // Match the unary operator
            ParseTreeNode* unaryOpNode = matchToken("UnaryOperator", token->value);
            if (!unaryOpNode) {
                reportSyntaxError("Failed to parse Unary Operator in Postfix Expression.");
                recoverFromError();
                freeParseTree(postfixNode);
                return NULL;
            }

            // Add the unary operator node as a child of the postfix node
            addChild(postfixNode, unaryOpNode);

        } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "[") == 0) {
            printf("[DEBUG] Parsing Array Access in Postfix Expression...\n");

            // Delegate to parseArrayAccess
            ParseTreeNode* arrayAccessNode = parseArrayAccess();
            if (!arrayAccessNode) {
                reportSyntaxError("Failed to parse array access in Postfix Expression.");
                recoverFromError();
                freeParseTree(postfixNode);
                return NULL;
            }
            addChild(postfixNode, arrayAccessNode);
        }

        // Check for more postfix operations
        token = peekToken();
    }

    printf("[DEBUG] Successfully parsed Postfix Expression.\n");
    return postfixNode;
}



// ---------------------------------------
// Literals and Identifiers                     
// ---------------------------------------

ParseTreeNode* parsePrimaryExpr() {
    printf("[DEBUG] Parsing Primary Expression...\n");

    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing a Primary Expression.");
        recoverFromError();
        return NULL;
    }

    // Create a node for the Primary Expression
    ParseTreeNode* primaryNode = createParseTreeNode("PrimaryExpr", "");

    // Check the type of the token and handle accordingly
    if (strcmp(token->type, "INT_LITERAL") == 0 || 
        strcmp(token->type, "FLOAT_LITERAL") == 0 || 
        strcmp(token->type, "CHAR_LITERAL") == 0 || 
        strcmp(token->type, "STRING_LITERAL") == 0 || 
        strcmp(token->type, "BOOLEAN_LITERAL") == 0) {
        // It's a literal
        printf("[DEBUG] Literal detected: Type='%s', Value='%s'\n", token->type, token->value);
        addChild(primaryNode, matchToken(token->type, token->value)); // Match and add the literal
    } else if (strcmp(token->type, "IDENTIFIER") == 0) {
        // It's an identifier
        printf("[DEBUG] Identifier detected: %s\n", token->value);
        addChild(primaryNode, matchToken("IDENTIFIER", token->value)); // Match and add the identifier
    } else if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        // It's a specifier identifier (e.g., &variable)
        printf("[DEBUG] SpecifierIdentifier detected: %s\n", token->value);
        addChild(primaryNode, matchToken("SpecifierIdentifier", token->value)); // Match and add the specifier identifier
    } else if (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "(") == 0) {
        // It's a grouped expression
        printf("[DEBUG] Detected '(' indicating a grouped expression.\n");
        addChild(primaryNode, matchToken("Delimiter", "(")); // Match and add '('

        // Parse the inner expression
        ParseTreeNode* innerExpr = parseExpression();
        if (!innerExpr) {
            reportSyntaxError("Failed to parse inner expression in grouped expression.");
            recoverFromError();
            freeParseTree(primaryNode);
            return NULL;
        }
        addChild(primaryNode, innerExpr);

        // Match the closing ')'
        if (!matchToken("Delimiter", ")")) {
            reportSyntaxError("Expected ')' to close the grouped expression.");
            recoverFromError();
            freeParseTree(primaryNode);
            return NULL;
        }
        addChild(primaryNode, createParseTreeNode("Delimiter", ")"));
        printf("[DEBUG] Detected closing ')' for grouped expression.\n");
    } else {
        // Unexpected token type
        reportSyntaxError("Expected a literal, identifier, specifier identifier, or grouped expression.");
        recoverFromError();
        freeParseTree(primaryNode);
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Primary Expression.\n");
    return primaryNode;
}



ParseTreeNode* parseBoolLiteral() {
    printf("[DEBUG] Parsing Boolean Literal...\n");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        reportSyntaxError("Expected a Boolean literal (true or false).");
        recoverFromError();
        return NULL;
    }

    return createParseTreeNode("BoolLiteral", matchToken("Keyword", token->value)->value);
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
        return createParseTreeNode("Literal", matchToken(token->type, token->value)->value);
    } else if (strcmp(token->type, "Keyword") == 0 &&
               (strcmp(token->value, "true") == 0 || strcmp(token->value, "false") == 0)) {
        return parseBoolLiteral();
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
        getNextToken();
        // Add the comment token value as a child node
        addChild(commentNode, createParseTreeNode("Comment", token->value));
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

ParseTreeNode* parseOutputList() {
    printf("[DEBUG] Parsing Output List...\n");

    // Create a parse tree node for the output list
    ParseTreeNode* outputListNode = createParseTreeNode("OutputList", "");

    Token* token = peekToken();

    // Parse the first argument (STRING_LITERAL or expression)
    if (token && strcmp(token->type, "STRING_LITERAL") == 0) {
        // Parse as a string literal
        ParseTreeNode* stringNode = matchToken("STRING_LITERAL", token->value);
        if (!stringNode) {
            reportSyntaxError("Expected a STRING_LITERAL in output list.");
            recoverFromError();
            freeParseTree(outputListNode);
            return NULL;
        }
        addChild(outputListNode, stringNode);
    } else if (token &&
               (strcmp(token->type, "IDENTIFIER") == 0 ||
                strcmp(token->type, "INT_LITERAL") == 0 ||
                strcmp(token->type, "FLOAT_LITERAL") == 0 ||
                strcmp(token->type, "CHAR_LITERAL") == 0 ||
                strcmp(token->type, "SpecifierIdentifier") == 0)) {
        // Parse as an expression
        ParseTreeNode* expressionNode = parseExpression();
        if (!expressionNode) {
            reportSyntaxError("Failed to parse expression in output list.");
            recoverFromError();
            freeParseTree(outputListNode);
            return NULL;
        }
        addChild(outputListNode, expressionNode);
    } else {
        reportSyntaxError("Expected STRING_LITERAL or expression as the first argument in output list.");
        recoverFromError();
        freeParseTree(outputListNode);
        return NULL;
    }

    // Parse additional arguments separated by commas
    while (peekToken() && strcmp(peekToken()->type, "Delimiter") == 0 && strcmp(peekToken()->value, ",") == 0) {
        // Consume the comma
        ParseTreeNode* commaNode = matchToken("Delimiter", ",");
        if (!commaNode) {
            reportSyntaxError("Expected ',' in output list.");
            recoverFromError();
            freeParseTree(outputListNode);
            return NULL;
        }
        addChild(outputListNode, commaNode);

        // Parse the next argument (expression)
        ParseTreeNode* nextArgumentNode = parseExpression();
        if (!nextArgumentNode) {
            reportSyntaxError("Expected a valid expression after ',' in output list.");
            recoverFromError();
            freeParseTree(outputListNode);
            return NULL;
        }
        addChild(outputListNode, nextArgumentNode);
    }

    printf("[DEBUG] Successfully parsed Output List.\n");
    return outputListNode;
}



// ---------------------------------------
// Array Handling                               
// ---------------------------------------

ParseTreeNode* parseArrayAccess() {
    printf("[DEBUG] Parsing Array Access...\n");

    // Create a parse tree node for array access
    ParseTreeNode* arrayAccessNode = createParseTreeNode("ArrayAccess", "");

    // Match and add the identifier for the array name
    ParseTreeNode* arrayNameNode = matchToken("IDENTIFIER", NULL); // Match any identifier
    if (!arrayNameNode) {
        reportSyntaxError("Expected an identifier for array access.");
        recoverFromError();
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, arrayNameNode);

    // Match and add the left bracket '['
    ParseTreeNode* leftBracketNode = matchToken("Delimiter", "[");
    if (!leftBracketNode) {
        reportSyntaxError("Expected '[' to start array access.");
        recoverFromError();
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, leftBracketNode);

    // Parse the index expression
    ParseTreeNode* indexExprNode = parseExpression();
    if (!indexExprNode) {
        reportSyntaxError("Expected a valid expression for array index.");
        recoverFromError();
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, indexExprNode);

    // Match and add the right bracket ']'
    ParseTreeNode* rightBracketNode = matchToken("Delimiter", "]");
    if (!rightBracketNode) {
        reportSyntaxError("Expected ']' to close array access.");
        recoverFromError();
        freeParseTree(arrayAccessNode);
        return NULL;
    }
    addChild(arrayAccessNode, rightBracketNode);

    printf("[DEBUG] Successfully parsed Array Access.\n");
    return arrayAccessNode;
}



// ---------------------------------------
// Functions                                    
// ---------------------------------------

ParseTreeNode* parseFunctionCall() {
    printf("[DEBUG] Parsing Function Call...\n");

    // Create a parse tree node for the function call
    ParseTreeNode* functionCallNode = createParseTreeNode("FunctionCall", "");

    // Expect an identifier for the function name
    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        reportSyntaxError("Expected an identifier for function name.");
        recoverFromError();
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, matchToken("IDENTIFIER", token->value));

    // Expect '(' to start the argument list
    if (!matchToken("Delimiter", "(")) {
        reportSyntaxError("Expected '(' after function name.");
        recoverFromError();
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("LeftParenthesis", "("));

    // Parse the argument list if present
    token = peekToken();
    if (token && !(strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        ParseTreeNode* argumentListNode = parseArgumentList();
        if (!argumentListNode) {
            reportSyntaxError("Error parsing argument list.");
            recoverFromError();
            freeParseTree(functionCallNode);
            return NULL;
        }
        addChild(functionCallNode, argumentListNode);
    }

    // Expect ')' to end the argument list
    if (!matchToken("Delimiter", ")")) {
        reportSyntaxError("Expected ')' after argument list.");
        recoverFromError();
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("RightParenthesis", ")"));

    // Expect a semicolon to end the function call
    if (!matchToken("Delimiter", ";")) {
        reportSyntaxError("Expected ';' after function call.");
        recoverFromError();
        freeParseTree(functionCallNode);
        return NULL;
    }
    addChild(functionCallNode, createParseTreeNode("Semicolon", ";"));

    printf("[DEBUG] Successfully parsed Function Call.\n");
    return functionCallNode;
}



ParseTreeNode* parseArgumentList() {
    printf("[DEBUG] Parsing Argument List...\n");

    // Create a parse tree node for the argument list
    ParseTreeNode* argumentListNode = createParseTreeNode("ArgumentList", "");

    // Parse the first argument
    Token* token = peekToken();
    if (!token) {
        reportSyntaxError("Unexpected end of input while parsing argument list.");
        recoverFromError();
        freeParseTree(argumentListNode);
        return NULL;
    }

    if (strcmp(token->type, "SpecifierIdentifier") == 0) {
        // Handle specifier identifier (e.g., `&var`)
        addChild(argumentListNode, matchToken("SpecifierIdentifier", token->value));
    } else {
        // Handle an expression as an argument
        ParseTreeNode* firstArgumentNode = parseExpression();
        if (!firstArgumentNode) {
            reportSyntaxError("Expected a valid expression as the first argument.");
            recoverFromError();
            freeParseTree(argumentListNode);
            return NULL;
        }
        addChild(argumentListNode, firstArgumentNode);
    }

    // Parse additional arguments separated by commas
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        addChild(argumentListNode, matchToken("Delimiter", ",")); // Add the comma to the tree

        token = peekToken();
        if (!token) {
            reportSyntaxError("Unexpected end of input while parsing additional arguments.");
            recoverFromError();
            freeParseTree(argumentListNode);
            return NULL;
        }

        if (strcmp(token->type, "SpecifierIdentifier") == 0) {
            addChild(argumentListNode, matchToken("SpecifierIdentifier", token->value));
        } else {
            ParseTreeNode* nextArgumentNode = parseExpression(); // Parse as an expression if not a SpecifierIdentifier
            if (!nextArgumentNode) {
                reportSyntaxError("Expected a valid expression in the argument list.");
                recoverFromError();
                freeParseTree(argumentListNode);
                return NULL;
            }
            addChild(argumentListNode, nextArgumentNode);
        }
    }

    printf("[DEBUG] Successfully parsed Argument List.\n");
    return argumentListNode;
}



ParseTreeNode* parseFunctionDeclaration() {
    printf("[DEBUG] Parsing Function Declaration...\n");

    // Create a parse tree node for the function declaration
    ParseTreeNode* functionDeclNode = createParseTreeNode("FunctionDeclaration", "");

    // Parse the return type
    ParseTreeNode* returnTypeNode = parseTypeSpecifier();
    if (!returnTypeNode) {
        reportSyntaxError("Expected a return type in function declaration.");
        recoverFromError();
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, returnTypeNode);

    // Expect an identifier for the function name
    Token* token = peekToken();
    if (!token || strcmp(token->type, "IDENTIFIER") != 0) {
        reportSyntaxError("Expected an identifier for the function name.");
        recoverFromError();
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, matchToken("IDENTIFIER", token->value));

    // Expect '(' to start the parameter list
    if (!matchToken("Delimiter", "(")) {
        reportSyntaxError("Expected '(' after the function name.");
        recoverFromError();
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, createParseTreeNode("Delimiter", "("));

    // Parse the parameter list if present
    ParseTreeNode* parameterListNode = parseParameterList();
    if (parameterListNode) {
        addChild(functionDeclNode, parameterListNode);
    }

    // Expect ')' to end the parameter list
    if (!matchToken("Delimiter", ")")) {
        reportSyntaxError("Expected ')' after the parameter list.");
        recoverFromError();
        freeParseTree(functionDeclNode);
        return NULL;
    }
    addChild(functionDeclNode, createParseTreeNode("Delimiter", ")"));

    // Check for either a semicolon (declaration-only) or a function body (compound statement)
    token = peekToken();
    if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ";") == 0) {
        // Declaration-only: consume the semicolon
        addChild(functionDeclNode, matchToken("Delimiter", ";"));
    } else if (token && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, "{") == 0) {
        // Function body: delegate to parseBlock()
        ParseTreeNode* blockNode = parseBlock();
        if (!blockNode) {
            reportSyntaxError("Error parsing function body.");
            recoverFromError();
            freeParseTree(functionDeclNode);
            return NULL;
        }
        addChild(functionDeclNode, blockNode);
    } else {
        reportSyntaxError("Expected ';' or a function body in function declaration.");
        recoverFromError();
        freeParseTree(functionDeclNode);
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Function Declaration.\n");
    return functionDeclNode;
}



ParseTreeNode* parseParameterList() {
    printf("[DEBUG] Parsing Parameter List...\n");

    // Create a node for the parameter list
    ParseTreeNode* parameterListNode = createParseTreeNode("ParameterList", "");

    // Peek at the next token to check if parameters are present
    Token* token = peekToken();
    if (!token || (strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ")") == 0)) {
        // No parameters; return an empty ParameterList node
        printf("[DEBUG] No parameters in the parameter list.\n");
        return parameterListNode;
    }

    // Parse the first parameter
    ParseTreeNode* parameterNode = parseParameter();
    if (!parameterNode) {
        reportSyntaxError("Expected a valid parameter in parameter list.");
        recoverFromError();
        freeParseTree(parameterListNode);
        return NULL;
    }
    addChild(parameterListNode, parameterNode);

    // Parse additional parameters separated by commas
    while ((token = peekToken()) && strcmp(token->type, "Delimiter") == 0 && strcmp(token->value, ",") == 0) {
        // Consume the comma
        addChild(parameterListNode, matchToken("Delimiter", ","));

        // Parse the next parameter
        parameterNode = parseParameter();
        if (!parameterNode) {
            reportSyntaxError("Expected a valid parameter after ','.");
            recoverFromError();
            freeParseTree(parameterListNode);
            return NULL;
        }
        addChild(parameterListNode, parameterNode);
    }

    printf("[DEBUG] Successfully parsed Parameter List.\n");
    return parameterListNode;
}



ParseTreeNode* parseParameter() {
    printf("[DEBUG] Parsing Parameter...\n");

    // Create a parse tree node for the parameter
    ParseTreeNode* parameterNode = createParseTreeNode("Parameter", "");

    // Parse the type specifier (e.g., "int", "float", etc.)
    Token* token = peekToken();
    if (token && strcmp(token->type, "Keyword") == 0 &&
        (strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
         strcmp(token->value, "char") == 0 || strcmp(token->value, "bool") == 0 ||
         strcmp(token->value, "string") == 0)) {
        addChild(parameterNode, matchToken("Keyword", token->value)); // Match and add type specifier
    } else {
        reportSyntaxError("Expected type specifier for parameter.");
        recoverFromError();
        freeParseTree(parameterNode);
        return NULL;
    }

    // Match and add the parameter name (IDENTIFIER or SpecifierIdentifier)
    token = peekToken();
    if (token && (strcmp(token->type, "SpecifierIdentifier") == 0 || strcmp(token->type, "IDENTIFIER") == 0)) {
        addChild(parameterNode, matchToken(token->type, token->value)); // Match and add parameter name
    } else {
        reportSyntaxError("Expected parameter name (identifier or specifier identifier).");
        recoverFromError();
        freeParseTree(parameterNode);
        return NULL;
    }

    printf("[DEBUG] Successfully parsed Parameter.\n");
    return parameterNode;
}


ParseTreeNode* parseReturnType() {
    printf("[DEBUG] Parsing Return Type...\n");

    Token* token = peekToken();
    if (!token || strcmp(token->type, "Keyword") != 0 ||
        !(strcmp(token->value, "int") == 0 || strcmp(token->value, "float") == 0 ||
          strcmp(token->value, "bool") == 0 || strcmp(token->value, "char") == 0 ||
          strcmp(token->value, "string") == 0 || strcmp(token->value, "void") == 0)) {
        reportSyntaxError("Expected a valid return type (int, float, bool, char, string, or void).");
        recoverFromError();
        return NULL;
    }

    return createParseTreeNode("ReturnType", matchToken("Keyword", token->value)->value);
}
