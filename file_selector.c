#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "file_selector.h"
#include "token.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// Function to find a `.prsm` file in the current directory
static char* findPrsmFile() {
    DIR *dir = opendir("."); // Open the current directory
    if (dir == NULL) {
        perror("Error opening directory");
        return NULL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        const char *extension = strrchr(entry->d_name, '.');
        if (extension && strcmp(extension, ".prsm") == 0) {
            closedir(dir);
            return strdup(entry->d_name); // Return a copy of the file name
        }
    }

    closedir(dir);
    return NULL; // No `.prsm` file found
}

// Function to initialize and open the required files
FileHandles* initializeFiles() {
    FileHandles *handles = (FileHandles*)malloc(sizeof(FileHandles));
    if (!handles) {
        fprintf(stderr, "Error: Memory allocation failed for file handles.\n");
        exit(EXIT_FAILURE);
    }

    // Find the source file
    handles->fileName = findPrsmFile();
    if (!handles->fileName) {
        fprintf(stderr, "Error: No .prsm file found in the current directory\n");
        free(handles);
        return NULL;
    }

    // Open the source file
    handles->sourceFile = fopen(handles->fileName, "r");
    if (!handles->sourceFile) {
        perror("Error opening source file");
        free(handles->fileName);
        free(handles);
        return NULL;
    }

    // Open the symbol table file
    handles->symbolTable = fopen("symbol_table.prsm", "w");
    if (!handles->symbolTable) {
        perror("Error opening symbol table file");
        fclose(handles->sourceFile);
        free(handles->fileName);
        free(handles);
        return NULL;
    }

    if (!handles->fileName) {
    fprintf(stderr, "Error: No .prsm file found.\n");
    return NULL;
    } else {
        printf("\n\n\nSource file detected: %s\n", handles->fileName);
    }

    if (!handles->sourceFile) {
        fprintf(stderr, "Error: Could not open source file: %s\n", handles->fileName);
        return NULL;
    }

    if (!handles->symbolTable) {
        fprintf(stderr, "Error: Could not create symbol table file.\n");
        return NULL;
    }


        return handles;
    }

// Function to close opened files and free resources
void closeFiles(FileHandles *handles) {

    if (handles) {
        if (handles->sourceFile) {
            fclose(handles->sourceFile);
            printf("[Debug] Source file closed.\n");
        }
        if (handles->symbolTable) {
            fclose(handles->symbolTable);
            printf("[Debug] Symbol table closed.\n");
        }
        if (handles->fileName) {
            free(handles->fileName);
        }
        free(handles);
    }

} // end of closeFiles function
