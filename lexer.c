#include "state_machine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_selector.h"
#include "token.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

int main() {

    // Initialize files
    FileHandles *handles = initializeFiles();
    if (!handles) {
        return 1; // Error already handled in `initializeFiles`
    }

    // Write header to the symbol table
    writeHorizontalBar(handles->symbolTable);
    fprintf(handles->symbolTable, "%-65s%-30s%-20s\n", "Token Type", "Value", "Line Number");
    writeHorizontalBar(handles->symbolTable);

    // Process the source file line by line
    char line[256];
    int lineNumber = 1;
    int inComment = 0; // Multi-line comment flag

    while (fgets(line, sizeof(line), handles->sourceFile)) {
        // Trim whitespace
        trimWhitespace(line);

        // Skip empty lines
        if (strlen(line) == 0) {
            lineNumber++;
            continue;
        }

        // Handle multi-line comments
        if (handleComments(line, &inComment, lineNumber, handles->symbolTable)) {
            lineNumber++;
            continue; // Skip the rest of the line if still inside a comment
        }

        // Process the current line using the state machine
        processLine(line, lineNumber, handles->symbolTable);
        lineNumber++;
    }

    // Close files
    closeFiles(handles);

    printf("Lexical analysis completed. \nTokens saved in symbol_table.prsm\n\n\n");
    return 0;

} // end of main function

