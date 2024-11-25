#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stdio.h>
#include "token.h"

// Function prototypes
Token* keywords(char *lexeme, int lineNumber); // Check if a lexeme is a keyword
int isReservedWord(const char *token);        // Check if a token is a reserved word
int isNoiseWord(const char *token);           // Check if a token is a noise word

#endif // KEYWORDS_H
