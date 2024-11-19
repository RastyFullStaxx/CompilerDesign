#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Token structure
typedef struct {
    char type[50];
    char value[50];
    int lineNumber;
} Token;

// Function prototypes
void processLine(char *line, int lineNumber, FILE *symbolTable);
void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber);
void writeHorizantalBar(FILE *symbolTable);
void trimWhitespace(char *str);

// FSM state types
typedef enum {START, IDENTIFIER, INTEGER, FLOAT, OPERATOR, DELIMITER, ERROR} State;

int main() {
    FILE *sourceFile = fopen("SourceCode.prsm", "r");
    if (!sourceFile) {
        perror("Error opening SourceCode.prsm");
        return 1;
    }

    FILE *symbolTable = fopen("symbol_table.txt", "w");
    if (!symbolTable) {
        perror("Error opening symbol_table.txt");
        return 1;
    }

    // Write header
    writeHorizantalBar(symbolTable);
    fprintf(symbolTable, "%-30s%-65s%-20s\n", "Type", "Value", "LineNumber");
    writeHorizantalBar(symbolTable);

    char line[256];
    int lineNumber = 1;
    int inComment = 0;  // Multi-line comment flag

    while (fgets(line, sizeof(line), sourceFile)) {
        // Trim whitespace
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        trimWhitespace(line);

        // Skip empty lines
        if (strlen(line) == 0) {
            lineNumber++;
            continue;
        }

        // Handle multi-line comments
        if (inComment) {
            if (strstr(line, "/~")) {
                writeToken(symbolTable, "Multi-line Comment End", "/~", lineNumber);
                inComment = 0;  // End comment
            }
            lineNumber++;
            continue;  // Ignore content inside multi-line comments
        }

        if (strstr(line, "~/")) {
            writeToken(symbolTable, "Multi-line Comment Start", "~/", lineNumber);
            inComment = 1;  // Start comment
            lineNumber++;
            continue;
        }

        // Process regular lines
        processLine(line, lineNumber, symbolTable);
        lineNumber++;
    }

    fclose(sourceFile);
    fclose(symbolTable);

    printf("Lexical analysis completed. Tokens saved in symbol_table.txt\n");
    return 0;
}

void processLine(char *line, int lineNumber, FILE *symbolTable) {
    State state = START;
    char currentToken[50] = "";
    int i = 0;

    const char *reservedWords[] = {"true", "false", "null", "const"};
    int reservedWordCount = sizeof(reservedWords) / sizeof(reservedWords[0]);

    for (int j = 0; line[j] != '\0'; j++) {
        char c = line[j];

        switch (state) {
            case START:
                if (isalpha(c) || c == '_') {
                    state = IDENTIFIER;
                    currentToken[i++] = c;
                } else if (isdigit(c)) {
                    state = INTEGER;
                    currentToken[i++] = c;
                } else if (c == '.') {
                    state = FLOAT;
                    currentToken[i++] = c;
                } else if (strchr("+-*/=%^~", c)) {
                    state = OPERATOR;
                    currentToken[i++] = c;
                } else if (strchr("(){}[],;<>", c)) {
                    state = DELIMITER;
                    currentToken[i++] = c;
                } else if (c == '~' && line[j + 1] == '~') {
                    writeToken(symbolTable, "Single-line Comment", "~~", lineNumber);
                    return;  // Skip rest of the line
                } else if (isspace(c)) {
                    // Ignore whitespace
                } else {
                    state = ERROR;
                    currentToken[i++] = c;
                }
                break;

            case IDENTIFIER:
                if (isalnum(c) || c == '_') {
                    currentToken[i++] = c;
                } else {
                    currentToken[i] = '\0';

                     // Check if it's a reserved word
                    int isReserved = 0;
                    for (int k = 0; k < reservedWordCount; k++) {
                        const char *reserved = reservedWords[k];
                        int match = 1;

                        for (int r = 0; reserved[r] != '\0'; r++) {
                            if (currentToken[r] != reserved[r] || currentToken[r] == '\0') {
                                match = 0;
                                break;
                            }
                        }

                        // Ensure the token length matches the reserved word length
                        if (match && currentToken[strlen(reserved)] == '\0') {
                            isReserved = 1;
                            break;
                        }
                    }

                    if (isReserved) {
                        writeToken(symbolTable, "Reserved Word", currentToken, lineNumber);
                    } else {
                        writeToken(symbolTable, "Identifier", currentToken, lineNumber);
                    }

                    i = 0;
                    state = START;
                    j--;  // Reprocess this character
                }
                break;

            case INTEGER:
                if (isdigit(c)) {
                    currentToken[i++] = c;
                } else if (c == '.') {
                    state = FLOAT;
                    currentToken[i++] = c;
                } else {
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Integer Literal", currentToken, lineNumber);
                    i = 0;
                    state = START;
                    j--;  // Reprocess this character
                }
                break;

            case FLOAT:
                if (isdigit(c)) {
                    currentToken[i++] = c;
                } else {
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Float Literal", currentToken, lineNumber);
                    i = 0;
                    state = START;
                    j--;  // Reprocess this character
                }
                break;

            case OPERATOR:
    // Check for increment and decrement (++ and --)
    if (currentToken[0] == '+') {
        if (line[j] == '+') { 
            // Increment operator
            currentToken[i++] = line[j++]; // Append second '+'
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Unary Operator", currentToken, lineNumber);
            i = 0;
            state = START;
        } else {
            // Distinguish Unary Plus (+x) from Addition (x + y)
            currentToken[i] = '\0'; // Null-terminate
            if (isspace(line[j]) || isalpha(line[j]) || isdigit(line[j])) {
                // Space or operand after '+' => Arithmetic Operator
                writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
            } else {
                // No operand => Unary Operator
                writeToken(symbolTable, "Unary Operator", currentToken, lineNumber);
            }
            i = 0;
            state = START;
        }
    } else if (currentToken[0] == '-') {
        if (line[j] == '-') { 
            // Decrement operator
            currentToken[i++] = line[j++]; // Append second '-'
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Unary Operator", currentToken, lineNumber);
            i = 0;
            state = START;
        } else {
            // Distinguish Unary Minus (-x) from Subtraction (x - y)
            currentToken[i] = '\0'; // Null-terminate
            if (isspace(line[j]) || isalpha(line[j]) || isdigit(line[j])) {
                // Space or operand after '-' => Arithmetic Operator
                writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
            } else {
                // No operand => Unary Operator
                writeToken(symbolTable, "Unary Operator", currentToken, lineNumber);
            }
            i = 0;
            state = START;
        }
    } else if (currentToken[0] == '/') {
        if (line[j] == '/') {
            // Integer Division Operator (//)
            currentToken[i++] = line[j++]; // Append second '/'
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
            i = 0;
            state = START;
        } else {
            // Regular Division Operator (/)
            currentToken[i] = '\0';
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
            i = 0;
            state = START;
        }
    } else if (currentToken[0] == '^') {
        // Exponentiation Operator (^)
        currentToken[i] = '\0';
        writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        i = 0;
        state = START;
    } else if (strchr("*/%", currentToken[0])) {
        // Other arithmetic operators (*, /, %)
        currentToken[i] = '\0';
        writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        i = 0;
        state = START;
    } else {
        // Default case: Handle unknown operators
        currentToken[i] = '\0';
        writeToken(symbolTable, "Unknown", currentToken, lineNumber);
        i = 0;
        state = START;
    }
    break;

                


            case DELIMITER:
                currentToken[i] = '\0';
                writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
                i = 0;
                state = START;
                j--;  // Reprocess this character
                break;

            case ERROR:
                currentToken[i] = '\0';
                writeToken(symbolTable, "Unknown", currentToken, lineNumber);
                i = 0;
                state = START;
                j--;  // Reprocess this character
                break;

        }
    }

    // Handle leftover tokens
    if (i > 0) {
        currentToken[i] = '\0';
        if (state == IDENTIFIER) {
            // Check if it's a reserved word
            int isReserved = 0;
            for (int k = 0; k < reservedWordCount; k++) {
                const char *reserved = reservedWords[k];
                int match = 1;

                for (int r = 0; reserved[r] != '\0'; r++) {
                    if (currentToken[r] != reserved[r] || currentToken[r] == '\0') {
                        match = 0;
                        break;
                    }
                }

                // Ensure the token length matches the reserved word length
                if (match && currentToken[strlen(reserved)] == '\0') {
                    isReserved = 1;
                    break;
                }
            }

            if (isReserved) {
                writeToken(symbolTable, "Reserved Word", currentToken, lineNumber);
            } else {
                writeToken(symbolTable, "Identifier", currentToken, lineNumber);
            }
            
        } else if (state == INTEGER) {
            writeToken(symbolTable, "Integer Literal", currentToken, lineNumber);
        } else if (state == FLOAT) {
            writeToken(symbolTable, "Float Literal", currentToken, lineNumber);
        } else if (state == OPERATOR) {
            writeToken(symbolTable, "Operator", currentToken, lineNumber);
        } else if (state == DELIMITER) {
            writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
        } else {
            writeToken(symbolTable, "Unknown", currentToken, lineNumber);
        }
    }
}

void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber) {
    fprintf(symbolTable, "%-30s%-65s%-20d\n", type, value, lineNumber);
}

void writeHorizantalBar(FILE *symbolTable) {
    fprintf(symbolTable, "--------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void trimWhitespace(char *str) {
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
}
