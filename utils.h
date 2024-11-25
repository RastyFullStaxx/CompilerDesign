#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// Utility function prototypes
void writeHorizontalBar(FILE *symbolTable);
void trimWhitespace(char *str);
int isDelimiter(char c);
int validFiletype(const char *fileName);

#endif // UTILS_H
