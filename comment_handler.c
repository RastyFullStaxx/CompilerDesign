#include "state_machine.h"
#include "comment_handler.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

// Function to handle comments (both single-line and multi-line)
int handleComments(char *line, int *inComment, int lineNumber, FILE *symbolTable) {
    int i = 0;

    //printf("[Debug] handleComments called. Line %d: %s\n", lineNumber, line);

    while (line[i] != '\0') {
        if (*inComment) {
            // Handle multi-line comment
            if (line[i] == '/' && line[i + 1] == '~') {
                // Multi-line comment ends
                //printf("[Debug] Multi-line Comment End found at line %d\n", lineNumber);
                writeToken(symbolTable, "Multi-line Comment End", "/~", lineNumber);
                *inComment = 0;
                return 1; // Skip processing further
            } else {
                // Continue multi-line comment content
                char content[256];
                int j = 0;
                while (line[i] != '\0' && !(line[i] == '/' && line[i + 1] == '~')) {
                    content[j++] = line[i++];
                }
                content[j] = '\0'; // Null-terminate
                //printf("[Debug] Multi-line Comment Content found: %s\n", content);
                writeToken(symbolTable, "Multi-line Comment Content", content, lineNumber);
                return 1; // Still inside a comment
            }
        } else if (line[i] == '~' && line[i + 1] == '~') {
            // Single-line comment detected
            char content[256];
            int j = 0;
            //printf("[Debug] Single-line Comment Start found at line %d\n", lineNumber);
            while (line[i] != '\0') {
                content[j++] = line[i++];
            }
            content[j] = '\0'; // Null-terminate
            //printf("[Debug] Single-line Comment Content: %s\n", content);
            writeToken(symbolTable, "Single-line Comment", content, lineNumber);
            return 1; // Skip processing further
        } else if (line[i] == '~' && line[i + 1] == '/') {
            // Multi-line comment start detected
            //printf("[Debug] Multi-line Comment Start found at line %d\n", lineNumber);
            writeToken(symbolTable, "Multi-line Comment Start", "~/", lineNumber);
            *inComment = 1;
            return 1; // Skip processing further
        } else {
            break; // No comment found, exit
        }
    }

    //printf("[Debug] Exiting handleComments. inComment: %d\n", *inComment);
    return *inComment; // Return whether still inside a comment
}
