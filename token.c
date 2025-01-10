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
        fprintf(symbolTable, "%-65s%-30s\n", type, value);
        //printf("[Debug] Written Token: Type = %s, Value = %s\n", type, value);
    } else {
        //fprintf(stderr, "[Error] Failed to write token: Type = %s, Value = %s\n", type, value);
    }

} // end of writeToken function
