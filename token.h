#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

// Token structure
typedef struct {
    char type[50];
    char value[50];
    int lineNumber;
} Token;

// Function prototypes
Token* makeToken(const char *type, const char *value, int lineNumber);
void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber);

#endif // TOKEN_H
