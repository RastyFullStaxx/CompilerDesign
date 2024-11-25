#ifndef FILE_SELECTOR_H
#define FILE_SELECTOR_H

#include <stdio.h>

// Struct to hold opened file pointers
typedef struct {
    FILE *sourceFile;
    FILE *symbolTable;
    char *fileName; // Stores the name of the source file
} FileHandles;

// Function to find and open the `.prsm` file and the symbol table
FileHandles* initializeFiles();
void closeFiles(FileHandles *handles);

#endif // FILE_SELECTOR_H
