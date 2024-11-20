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

// FSM state types
typedef enum {
    START,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    ARITHMETIC_OPERATOR,
    ASSIGNMENT_OPERATOR,
    RELATIONAL_OPERATOR,
    LOGICAL_OPERATOR,
    UNARY_OPERATOR,
    DELIMITER,
    ERROR
} State;


// Function prototypes
void processLine(char *line, int lineNumber, FILE *symbolTable);
void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber);
void writeHorizantalBar(FILE *symbolTable);
void trimWhitespace(char *str);


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
    fprintf(symbolTable, "%-65s%-30s%-20s\n", "Type", "Value", "LineNumber");
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

    // Reserved words declaration
    const char *reservedWords[] = {"true", "false", "null", "const"};
    int reservedWordCount = sizeof(reservedWords) / sizeof(reservedWords[0]);


    for (int j = 0; line[j] != '\0'; j++) {
        char c = line[j];

        switch (state) {

// START case for recursive use ------------------------------------------------------------------------------------------------
            case START:
    if (c == '~' && line[j + 1] == '~') {
        // Single-line comment indicator (~~)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume the second '~'
        currentToken[i] = '\0';
        writeToken(symbolTable, "Single-line Comment", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
        return; // Skip the rest of the line
    } else if (c == '=' && line[j + 1] == '=') {
        // Equal to operator (==)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume the second '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Relational Operator (Equal To)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '!' && line[j + 1] == '=') {
        // Not equal to operator (!=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Relational Operator (Not Equal To)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '>' && line[j + 1] == '=') {
        // Greater than or equal to operator (>=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Relational Operator (Greater Than or Equal To)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '<' && line[j + 1] == '=') {
        // Less than or equal to operator (<=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Relational Operator (Less Than or Equal To)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '>') {
        // Greater than operator (>)
        currentToken[i++] = c;
        currentToken[i] = '\0';
        writeToken(symbolTable, "Relational Operator (Greater Than)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '<') {
        // Less than operator (<)
        currentToken[i++] = c;
        currentToken[i] = '\0';
        writeToken(symbolTable, "Relational Operator (Less Than)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '!' && line[j + 1] != '=') {
        // Logical NOT operator (!)
        currentToken[i++] = c;
        currentToken[i] = '\0';
        writeToken(symbolTable, "Logical Operator (NOT)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '|' && line[j + 1] == '|') {
        // Logical OR operator (||)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume the second '|'
        currentToken[i] = '\0';
        writeToken(symbolTable, "Logical Operator (OR)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '&' && line[j + 1] == '&') {
        // Logical AND operator (&&)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume the second '&'
        currentToken[i] = '\0';
        writeToken(symbolTable, "Logical Operator (AND)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '=' && line[j + 1] != '=') {
        // Assignment operator
        currentToken[i++] = c;
        currentToken[i] = '\0';
        writeToken(symbolTable, "Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '+' && line[j + 1] == '=') {
        // Addition assignment operator (+=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Addition Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '-' && line[j + 1] == '=') {
        // Subtraction assignment operator (-=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Subtraction Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '*' && line[j + 1] == '=') {
        // Multiplication assignment operator (*=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Multiplication Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '/' && line[j + 1] == '=') {
        // Division assignment operator (/=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Division Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '/' && line[j + 1] == '/' && line[j + 2] == '=') {
        // Integer division assignment operator (//=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '/'
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Integer Division Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '%' && line[j + 1] == '=') {
        // Modulo assignment operator (%=)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume '='
        currentToken[i] = '\0';
        writeToken(symbolTable, "Modulo Assignment Operator", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '/' && line[j + 1] == '/') {
        // Integer division operator (//)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume the second '/'
        currentToken[i] = '\0';
        writeToken(symbolTable, "Arithmetic Operator (Integer Division)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '/') {
        // Division operator
        currentToken[i++] = c;
        currentToken[i] = '\0';
        writeToken(symbolTable, "Arithmetic Operator (Division)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '+' && line[j + 1] == '+') {
        // Increment operator (++)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume second '+'
        currentToken[i] = '\0';
        writeToken(symbolTable, "Unary Operator (Increment)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if (c == '-' && line[j + 1] == '-') {
        // Decrement operator (--)
        currentToken[i++] = c;
        currentToken[i++] = line[++j]; // Consume second '-'
        currentToken[i] = '\0';
        writeToken(symbolTable, "Unary Operator (Decrement)", currentToken, lineNumber);
        i = 0;
        state = START; // Reset state
    } else if ((c == '+' || c == '-') && (isdigit(line[j + 1]) || isalpha(line[j + 1]) || line[j + 1] == '_')) {
        // Unary + or -
        state = UNARY_OPERATOR;
        currentToken[i++] = c;
    } else if (c == '+' || c == '-') {
        // Standalone + or - treated as arithmetic operators
        currentToken[i++] = c;
        currentToken[i] = '\0';
        if (c == '+') {
            writeToken(symbolTable, "Arithmetic Operator (Addition)", currentToken, lineNumber);
        } else {
            writeToken(symbolTable, "Arithmetic Operator (Subtraction)", currentToken, lineNumber);
        }
        i = 0;
        state = START; // Reset state
    } else if (isalpha(c) || c == '_') {
        // Identifier or reserved word
        state = IDENTIFIER;
        currentToken[i++] = c;
    } else if (isdigit(c)) {
        // Integer literal
        state = INTEGER;
        currentToken[i++] = c;
    } else if (c == '*' || c == '%' || c == '^') {
        // Arithmetic operators (excluding / already handled)
        currentToken[i++] = c;
        currentToken[i] = '\0';
        if (c == '*') {
            writeToken(symbolTable, "Arithmetic Operator (Multiplication)", currentToken, lineNumber);
        } else if (c == '%') {
            writeToken(symbolTable, "Arithmetic Operator (Modulo)", currentToken, lineNumber);
        } else if (c == '^') {
            writeToken(symbolTable, "Arithmetic Operator (Exponentiation)", currentToken, lineNumber);
        }
        i = 0;
        state = START; // Reset state
    } else if (c == '=' || c == '>' || c == '<' || c == '!') {
        // Relational or assignment operators
        state = RELATIONAL_OPERATOR;
        currentToken[i++] = c;
    } else if (c == '&') {
        // Logical AND candidate
        state = LOGICAL_OPERATOR;
        currentToken[i++] = c;
    } else if (c == '|') {
        // Logical OR candidate
        state = LOGICAL_OPERATOR;
        currentToken[i++] = c;
    } else if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ';') {
        // Delimiters
        currentToken[i++] = c;
        currentToken[i] = '\0';
        writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
        i = 0;
        state = START;
    } else if (isspace(c)) {
        // Ignore whitespace
    } else {
        // Unknown character
        state = ERROR;
        currentToken[i++] = c;
    }
    break;









            case IDENTIFIER:
                if (isalnum(c) || c == '_') {
                    // Continue building the identifier
                    currentToken[i++] = c;
                } else {
                    // Identifier complete
                    currentToken[i] = '\0';

                    // Check if the identifier is a reserved word
                    int isReserved = 0;
                    for (int k = 0; k < reservedWordCount; k++) {
                        int match = 1;
                        const char *reserved = reservedWords[k];
                        for (int r = 0; reserved[r] != '\0'; r++) {
                            if (currentToken[r] != reserved[r] || currentToken[r] == '\0') {
                                match = 0;
                                break;
                            }
                        }

                        // Match found only if lengths match too
                        if (match && currentToken[strlen(reserved)] == '\0') {
                            isReserved = 1;
                            break;
                        }
                    }

                    // Token classification
                    if (isReserved) {
                        writeToken(symbolTable, "Reserved Word", currentToken, lineNumber);
                    } else {
                        writeToken(symbolTable, "Identifier", currentToken, lineNumber);
                    }

                    // Reset state and reprocess current character
                    i = 0;
                    state = START;
                    j--; // Reprocess this character
                }
                break;




            case INTEGER:
                if (isdigit(c)) {
                    // Continue building the integer literal
                    currentToken[i++] = c;
                } else if (c == '.') {
                    // Transition to FLOAT state
                    state = FLOAT;
                    currentToken[i++] = c;
                } else {
                    // Integer literal complete
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Integer Literal", currentToken, lineNumber);

                    // Reset state and reprocess current character
                    i = 0;
                    state = START;
                    j--; // Reprocess this character
                }
                break;




            case FLOAT:
                if (isdigit(c)) {
                    // Continue building the float literal
                    currentToken[i++] = c;
                } else {
                    // Float literal complete
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Float Literal", currentToken, lineNumber);

                    // Reset state and reprocess current character
                    i = 0;
                    state = START;
                    j--; // Reprocess this character
                }
                break;






            case ARITHMETIC_OPERATOR:
                if (currentToken[0] == '+') {
                    // Addition operator
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Addition)", currentToken, lineNumber);
                } else if (currentToken[0] == '-') {
                    // Subtraction operator
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Subtraction)", currentToken, lineNumber);
                } else if (currentToken[0] == '*') {
                    // Multiplication operator
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Multiplication)", currentToken, lineNumber);
                } else if (currentToken[0] == '/') {
                    if (line[j] == '/') {
                        // Integer division operator (//)
                        currentToken[i++] = line[j++]; // Append the second '/'
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Arithmetic Operator (Integer Division)", currentToken, lineNumber);
                    } else {
                        // Division operator
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Arithmetic Operator (Division)", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '%') {
                    // Modulo operator
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Modulo)", currentToken, lineNumber);
                } else if (currentToken[0] == '^') {
                    // Exponentiation operator
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Exponentiation)", currentToken, lineNumber);
                }

                // Reset state for next token
                i = 0;
                state = START;
                break;






            case ASSIGNMENT_OPERATOR:
                if (currentToken[0] == '=') {
                    if (line[j] == '=') {
                        // Relational Operator (==)
                        currentToken[i++] = line[j++]; // Append second '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Relational Operator (Equal To)", currentToken, lineNumber);
                    } else {
                        // Assignment Operator (=)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Assignment Operator", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '+') {
                    if (line[j] == '=') {
                        // Addition Assignment Operator (+=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Addition Assignment Operator", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '-') {
                    if (line[j] == '=') {
                        // Subtraction Assignment Operator (-=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Subtraction Assignment Operator", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '*') {
                    if (line[j] == '=') {
                        // Multiplication Assignment Operator (*=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Multiplication Assignment Operator", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '/') {
                    if (line[j] == '=') {
                        // Division Assignment Operator (/=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Division Assignment Operator", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '%') {
                    if (line[j] == '=') {
                        // Modulo Assignment Operator (%=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Modulo Assignment Operator", currentToken, lineNumber);
                    }
                }
                i = 0;
                state = START;
                break;







            case RELATIONAL_OPERATOR:
                if (currentToken[0] == '=' && line[j] == '=') {
                    // Equal to operator (==)
                    currentToken[i++] = line[j++]; // Append second '='
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Relational Operator (Equal To)", currentToken, lineNumber);
                } else if (currentToken[0] == '!' && line[j] == '=') {
                    // Not equal to operator (!=)
                    currentToken[i++] = line[j++]; // Append second '='
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Relational Operator (Not Equal To)", currentToken, lineNumber);
                } else if (currentToken[0] == '<') {
                    if (line[j] == '=') {
                        // Less than or equal to operator (<=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Relational Operator (Less Than or Equal To)", currentToken, lineNumber);
                    } else {
                        // Less than operator (<)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Relational Operator (Less Than)", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '>') {
                    if (line[j] == '=') {
                        // Greater than or equal to operator (>=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Relational Operator (Greater Than or Equal To)", currentToken, lineNumber);
                    } else {
                        // Greater than operator (>)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Relational Operator (Greater Than)", currentToken, lineNumber);
                    }
                }
                i = 0;
                state = START;
                break;






            case LOGICAL_OPERATOR:
                if (currentToken[0] == '!') {
                    if (line[j] == '=') {
                        // Relational Operator (!=)
                        currentToken[i++] = line[j++]; // Append '='
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Relational Operator (Not Equal To)", currentToken, lineNumber);
                    } else {
                        // Logical Operator (!)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Logical Operator (NOT)", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '&') {
                    if (line[j] == '&') {
                        // Logical AND Operator (&&)
                        currentToken[i++] = line[j++]; // Append second '&'
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Logical Operator (AND)", currentToken, lineNumber);
                    } else {
                        // Single & (fallback to unknown or further usage)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Unknown Operator", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '|') {
                    if (line[j] == '|') {
                        // Logical OR Operator (||)
                        currentToken[i++] = line[j++]; // Append second '|'
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Logical Operator (OR)", currentToken, lineNumber);
                    } else {
                        // Single | (fallback to unknown or further usage)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Unknown Operator", currentToken, lineNumber);
                    }
                }
                i = 0;
                state = START;
                break;





            case UNARY_OPERATOR:
                if (currentToken[0] == '+') {
                    if (line[j] == '+') {
                        // Increment Operator (++)
                        currentToken[i++] = line[j++]; // Append second '+'
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Unary Operator (Increment)", currentToken, lineNumber);
                    } else {
                        // Unary Plus (+i)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Unary Operator (Positive)", currentToken, lineNumber);
                    }
                } else if (currentToken[0] == '-') {
                    if (line[j] == '-') {
                        // Decrement Operator (--)
                        currentToken[i++] = line[j++]; // Append second '-'
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Unary Operator (Decrement)", currentToken, lineNumber);
                    } else {
                        // Unary Minus (-x)
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "Unary Operator (Negative)", currentToken, lineNumber);
                    }
                }
                i = 0;
                state = START;
                break;



            case DELIMITER:
                currentToken[i] = '\0'; // Null-terminate the token
                writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
                i = 0; // Reset token index
                state = START; // Return to START state
                j--;  // Reprocess the current character
                break;

            case ERROR:
                currentToken[i] = '\0'; // Null-terminate the token
                writeToken(symbolTable, "Unknown Token", currentToken, lineNumber);
                i = 0; // Reset token index
                state = START; // Return to START state
                j--;  // Reprocess the current character
                break;


        }
    }

                    // Handle leftover tokens
                if (i > 0) {
                    currentToken[i] = '\0'; // Null-terminate the token
                    switch (state) {
                        case IDENTIFIER: {
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
                            break;

                        }

                        case INTEGER:
                            writeToken(symbolTable, "Integer Literal", currentToken, lineNumber);
                            break;
    
                        case FLOAT:
                            writeToken(symbolTable, "Float Literal", currentToken, lineNumber);
                            break;
    
                        case ARITHMETIC_OPERATOR:
                            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
                            break;
    
                        case ASSIGNMENT_OPERATOR:
                            writeToken(symbolTable, "Assignment Operator", currentToken, lineNumber);
                            break;
    
                        case RELATIONAL_OPERATOR:
                            writeToken(symbolTable, "Relational Operator", currentToken, lineNumber);
                            break;
    
                        case LOGICAL_OPERATOR:
                            writeToken(symbolTable, "Logical Operator", currentToken, lineNumber);
                            break;
    
                        case UNARY_OPERATOR:
                            writeToken(symbolTable, "Unary Operator", currentToken, lineNumber);
                            break;
    
                        case DELIMITER:
                            writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
                            break;
    
                        default:
                            writeToken(symbolTable, "Unknown", currentToken, lineNumber);
                            break;
                                }           
                            }           

} // end of processLine function

void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber) {
    if (symbolTable != NULL && type != NULL && value != NULL) {
        fprintf(symbolTable, "%-65s%-30s%-20d\n", type, value, lineNumber);
    }
}

void writeHorizantalBar(FILE *symbolTable) {
    if (symbolTable != NULL) {
        fprintf(symbolTable, "--------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
}

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
}
