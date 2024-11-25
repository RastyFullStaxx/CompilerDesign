#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "file_selector.h"
#include "token.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// Write horizontal bar in the symbol table
void writeHorizontalBar(FILE *symbolTable) {

    if (symbolTable != NULL) {
        fprintf(symbolTable, "--------------------------------------------------------------------------------------------------------------------------------------------\n");
    }

} // end of writeHorizontalBar function




// Trim whitespace from a string
void trimWhitespace(char *str) {

    if (str == NULL) {
        return;
    }

    int end = strlen(str) - 1;
    while (end >= 0 && isspace((unsigned char)str[end])) {
        str[end--] = '\0';
    }

    char *start = str;  
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

} // end of trimWhitespace function




// Check if a character is a delimiter
int isDelimiter(char c) {

    return (c == ',' || c == '.' || c == ';' || c == ':' || 
            c == '(' || c == ')' || c == '{' || c == '}' || 
            c == '[' || c == ']' || c == '"' || c == '\'');

} // end of checkDelimiter function




// Validate file type
int validFiletype(const char *fileName) {

    const char *extension = strrchr(fileName, '.');
    if(!extension){
        return 1;
    }

    const char *prisma = ".prsm";
    while(*extension && *prisma){
        if(*extension != *prisma){
            return 1;
        }
        extension++;
        prisma++;
    }

    if (*extension == '\0' && *prisma == '\0'){
        return 0;
    }
    return 1;

} // end of validFiletype function
