#ifndef COMMENT_HANDLER_H
#define COMMENT_HANDLER_H

#include <stdio.h>

// Function to handle multi-line comments
int handleComments(char *line, int *inComment, int lineNumber, FILE *symbolTable);

#endif // COMMENT_HANDLER_H
