#include "state_machine.h"
#include "comment_handler.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

// Function to handle comments (single-line and multi-line)
int handleComments(char *line, int *inComment, int lineNumber, FILE *symbolTable) {
    int i = 0;
    char commentBuffer[256] = {0}; // Buffer for the entire comment content
    int bufferIndex = 0;

    while (line[i] != '\0') {
        if (*inComment) {
            // Handle multi-line comment continuation
            if (line[i] == '/' && line[i + 1] == '~') {
                // Multi-line comment ends
                if (bufferIndex < sizeof(commentBuffer) - 3) {
                    // Add the closing marker to the buffer
                    commentBuffer[bufferIndex++] = '/';
                    commentBuffer[bufferIndex++] = '~';
                }
                commentBuffer[bufferIndex] = '\0'; // Null-terminate
                writeToken(symbolTable, "Comment", commentBuffer, lineNumber);
                *inComment = 0; // Exit multi-line comment mode
                return 1; // Skip further processing
            } else {
                // Accumulate content within the multi-line comment
                if (bufferIndex < sizeof(commentBuffer) - 1) {
                    commentBuffer[bufferIndex++] = line[i];
                }
                i++;
                continue; // Continue accumulating content
            }
        } else if (line[i] == '~' && line[i + 1] == '~') {
            // Single-line comment detected
            if (bufferIndex < sizeof(commentBuffer) - 3) {
                // Add the opening marker to the buffer
                commentBuffer[bufferIndex++] = '~';
                commentBuffer[bufferIndex++] = '~';
            }
            i += 2; // Skip the comment marker
            while (line[i] != '\0') {
                if (bufferIndex < sizeof(commentBuffer) - 1) {
                    commentBuffer[bufferIndex++] = line[i++];
                } else {
                    break; // Prevent buffer overflow
                }
            }
            commentBuffer[bufferIndex] = '\0'; // Null-terminate
            writeToken(symbolTable, "Comment", commentBuffer, lineNumber);
            return 1; // Skip further processing
        } else if (line[i] == '~' && line[i + 1] == '/') {
            // Multi-line comment starts
            if (bufferIndex < sizeof(commentBuffer) - 3) {
                // Add the opening marker to the buffer
                commentBuffer[bufferIndex++] = '~';
                commentBuffer[bufferIndex++] = '/';
            }
            *inComment = 1; // Enter multi-line comment mode
            i += 2; // Skip the comment marker
            bufferIndex = bufferIndex; // Keep buffer index for accumulating content
        } else {
            break; // No comment found, exit
        }
    }

    // If still in a multi-line comment, accumulate the remaining line
    if (*inComment && bufferIndex > 0) {
        commentBuffer[bufferIndex] = '\0'; // Null-terminate
        writeToken(symbolTable, "Comment", commentBuffer, lineNumber);
    }

    return *inComment; // Return whether still inside a comment
}
