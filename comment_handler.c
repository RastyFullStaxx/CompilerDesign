#include <stdio.h>
#include <string.h>
#include "file_selector.h"
#include "token.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// Function to handle multi-line comments
int handleComments(char *line, int *inComment, int lineNumber, FILE *symbolTable) {
    int startCommentIndex, endCommentIndex;
    startCommentIndex = strstr(line, "~/") ? strstr(line, "~/") - line : -1;
    endCommentIndex = strstr(line, "/~") ? strstr(line, "/~") - line : -1;

    while (startCommentIndex != -1 || endCommentIndex != -1) {
        if (startCommentIndex != -1 && (endCommentIndex == -1 || startCommentIndex < endCommentIndex)) {
            // Found a multi-line comment start
            writeToken(symbolTable, "Multi-line Comment Start", "~/", lineNumber);
            *inComment = 1; // Set inComment flag
            startCommentIndex = strstr(line + startCommentIndex + 2, "~/") 
                                ? strstr(line + startCommentIndex + 2, "~/") - line 
                                : -1;
        } else if (endCommentIndex != -1) {
            // Found a multi-line comment end
            writeToken(symbolTable, "Multi-line Comment End", "/~", lineNumber);
            *inComment = 0; // Clear inComment flag
            endCommentIndex = strstr(line + endCommentIndex + 2, "/~") 
                              ? strstr(line + endCommentIndex + 2, "/~") - line 
                              : -1;
        }
    }

    return *inComment; // Return whether we're still inside a comment
}
