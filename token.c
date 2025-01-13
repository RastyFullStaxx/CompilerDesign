#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_selector.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// Function to create a new token
Token* makeToken(const char *type, const char *value, int lineNumber) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Error: Memory allocation failed for token.\n");
        exit(EXIT_FAILURE);
    }

    // Copy type and value into the token
    strncpy(token->type, type, sizeof(token->type) - 1);
    token->type[sizeof(token->type) - 1] = '\0';
    strncpy(token->value, value, sizeof(token->value) - 1);
    token->value[sizeof(token->value) - 1] = '\0';

    // Assign the line number
    token->lineNumber = lineNumber;

    return token;
} // end of makeToken function

// Function to write a token to the symbol table
void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber) {
    if (symbolTable != NULL && type != NULL && value != NULL) {
        // Write token with fixed-width columns
        // Column width: Type = 25, Value = 20, Line = 6
        fprintf(symbolTable, "%-25s %-20s %-6d\n", type, value, lineNumber);

        // Debugging log
        printf("[Debug] Written Token: Type = %-25s, Value = %-20s, Line = %-6d\n", type, value, lineNumber);
    } else {
        fprintf(stderr, "[Error] Failed to write token: Type = %s, Value = %s, Line = %d\n", 
                type ? type : "(null)", value ? value : "(null)", lineNumber);
    }
} // end of writeToken function
