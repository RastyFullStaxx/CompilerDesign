#include "state_machine.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "file_selector.h"
#include "token.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

// processLine function
void processLine(char *line, int lineNumber, FILE *symbolTable) {
    State state = START;
    char currentToken[50] = "";
    int i = 0;
    static int inComment = 0; // Track multi-line comment state

    // Delegate comment handling
    if (handleComments(line, &inComment, lineNumber, symbolTable)) {
        return; // Skip processing the rest of the line if inside a comment
    }

    for (int j = 0; line[j] != '\0'; j++) {
        char c = line[j];

        switch (state) {
            case START:
                if ((c == '=' && line[j + 1] == '=') || 
                           (c == '!' && line[j + 1] == '=') || 
                           (c == '>' && line[j + 1] == '=') || 
                           (c == '<' && line[j + 1] == '=')) {
                    // Relational operators with '='
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '=') ? "RelationalOperator" :
                                       (c == '!') ? "RelationalOperator" :
                                       (c == '>') ? "RelationalOperator" :
                                                    "RelationalOperator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '>' || c == '<') {
                    // Relational operators (>, <)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    const char* type = (c == '>') ? "RelationalOperator" :
                                                    "RelationalOperator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '!' && line[j + 1] != '=') {
                    // Logical NOT operator (!)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "LogicalOperator", currentToken, lineNumber);
                    i = 0;
                } else if ((c == '|' && line[j + 1] == '|') || (c == '&' && line[j + 1] == '&')) {
                    // Logical operators (||, &&)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '|') ? "LogicalOperator" : "LogicalOperator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '=' && line[j + 1] != '=') {
                    // AssignmentOperator (=)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                    i = 0;
                } else if ((c == '+' && line[j + 1] == '=') || (c == '-' && line[j + 1] == '=') ||
                           (c == '*' && line[j + 1] == '=') || (c == '/' && line[j + 1] == '=') || 
                           (c == '%' && line[j + 1] == '=')) {
                    // AssignmentOperators (+=, -=, *=, /=, %=)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '+') ? "AssignmentOperator" :
                                       (c == '-') ? "AssignmentOperator" :
                                       (c == '*') ? "AssignmentOperator" :
                                       (c == '/') ? "AssignmentOperator" :
                                                    "AssignmentOperator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '/' && line[j + 1] == '/' && line[j + 2] == '=') {
                    // Integer AssignmentOperator (//=)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                    i = 0;
                } else if (c == '/' && line[j + 1] == '/') {
                    // Integer division operator (//)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                    i = 0;
                } else if (c == '/' || c == '*' || c == '%') {
                    // Arithmetic operators (/, *, %)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    const char* type = (c == '/') ? "ArithmeticOperator" :
                                       (c == '*') ? "ArithmeticOperator" :
                                                    "ArithmeticOperator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if ((c == '+' && line[j + 1] == '+') || (c == '-' && line[j + 1] == '-')) {
    // Unary operators (++, --)
    currentToken[i++] = c;
    currentToken[i++] = line[++j];
    currentToken[i] = '\0';
    writeToken(symbolTable, "UnaryOperator", currentToken, lineNumber);
    i = 0;
    state = START; // Stay in START to process the next character
                } else if (c == '+' || c == '-') {
                    // Arithmetic operators (+, -)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    const char* type = (c == '+') ? "ArithmeticOperator" : "ArithmeticOperator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '&') {
                    // Handle specifier identifier (&identifier)
                    if (isalnum(line[j + 1]) || line[j + 1] == '_') {
                        currentToken[i++] = c; // Append '&'
                        c = line[++j];         // Move to the next character
                        state = IDENTIFIER;    // Transition to IDENTIFIER state
                        currentToken[i++] = c; // Start building the identifier
                    } else {
                        // Invalid token starting with '&'
                        currentToken[i++] = c;
                        currentToken[i] = '\0';
                        writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                        i = 0;
                        state = START; // Reset state
                    }
                } else if (isalpha(c) || c == '_') {
                    // IDENTIFIER or ReservedWord
                    state = IDENTIFIER;
                    currentToken[i++] = c;
                } else if (isdigit(c)) {
                    // INT_LITERAL
                    state = INTEGER;
                    currentToken[i++] = c;
                } else if (c == '^') {
                    // Exponentiation operator (^)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                    i = 0;
                } else if (c == '\'') {
                    // Start of CHAR_LITERAL
                    currentToken[i++] = c;
                    state = CHAR_LITERAL;
                } else if (c == '"') {
                    // Start of STRING_LITERAL
                    currentToken[i++] = c;
                    state = STRING_LITERAL;
                } else if (isalpha(c) || c == '_') {
                    state = IDENTIFIER;
                    currentToken[i++] = c;
                } else if (!isspace(c) && !isDelimiter(c) && !isalnum(c) && c != '_') {
                    // Invalid token starting with a special character
                    currentToken[i++] = c; // Capture the invalid starting character
                    while (line[j + 1] != '\0' && !isspace(line[j + 1]) && !isDelimiter(line[j + 1])) {
                        currentToken[i++] = line[++j]; // Append subsequent characters
                    }
                    currentToken[i] = '\0'; // Null-terminate
                    writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                    i = 0;
                    state = START; // Reset state
                    break;
                } else if (isdigit(c)) {
                    // Start of integer
                    state = INTEGER;
                    currentToken[i++] = c;
                } else if (isalpha(c) || c == '_') {
                    // Start of identifier
                    state = IDENTIFIER;
                    currentToken[i++] = c;
                } else if (isdigit(c)) {
                    // Start of integer
                    state = INTEGER;
                    currentToken[i++] = c;
                } else if (c == '(' || c == '{' || c == '[') {
                    // Left delimiters
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, (c == '(') ? "Delimiter" :
                                                   (c == '{') ? "Delimiter" :
                                                                "Delimiter", currentToken, lineNumber);
                    i = 0; // Reset token index
                } else if (c == ')' || c == '}' || c == ']') {
                    // Right delimiters
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, (c == ')') ? "Delimiter" :
                                                   (c == '}') ? "Delimiter" :
                                                                "Delimiter", currentToken, lineNumber);
                    i = 0; // Reset token index
                } else if (c == ',' || c == '.' || c == ';' || c == ':') {
                    // Independent delimiters
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, (c == ',') ? "Delimiter" :
                                                   (c == '.') ? "Delimiter" :
                                                   (c == ';') ? "Delimiter" :
                                                                "Delimiter", currentToken, lineNumber);
                    i = 0; // Reset token index
                } else if (!isspace(c)) {
                    // Other characters (unknown token)
                    currentToken[i++] = c;
                    state = ERROR;
                }
                break;

        







                        case IDENTIFIER:
    if (isalnum(c) || c == '_') {
        // Continue building the identifier
        currentToken[i++] = c;
    } else if (currentToken[0] == '&' && isalpha(c)) {
        // Continue building identifier prefixed with '&'
        currentToken[i++] = c;
    } else if (!isalpha(currentToken[0]) && !isDelimiter(c) && !isspace(c)) {
        // Invalid identifier
        while (isalnum(c) || c == '_' || (!isDelimiter(c) && !isspace(c))) {
            currentToken[i++] = c;
            c = line[++j];
        }
        currentToken[i] = '\0';
        printf("Lexical Error: %s\n", currentToken);
        writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
        i = 0;
        state = START;
        j--;
    } else {
        // Finalize identifier
        currentToken[i] = '\0'; // Null-terminate the token
        printf("Finalizing token: %s\n", currentToken); // Debug

        // Check if the identifier is prefixed with '&'
        if (currentToken[0] == '&') {
            // Handle specifier identifier
            writeToken(symbolTable, "SpecifierIdentifier", currentToken, lineNumber);
        } else {
            Token *token = keywords(currentToken, lineNumber); // Check keywords
            if (token) {
                printf("Keyword detected: %s\n", token->value); // Debug
                writeToken(symbolTable, token->type, token->value, lineNumber);
                free(token);
            } else if (isReservedWord(currentToken)) {
                printf("ReservedWord detected: %s\n", currentToken); // Debug
                writeToken(symbolTable, "ReservedWord", currentToken, lineNumber);
            } else if (isNoiseWord(currentToken)) {
                printf("NoiseWord detected: %s\n", currentToken); // Debug
                writeToken(symbolTable, "NoiseWord", currentToken, lineNumber);
            } else {
                printf("IDENTIFIER detected: %s\n", currentToken); // Debug
                writeToken(symbolTable, "IDENTIFIER", currentToken, lineNumber);
            }
        }

        // Reset token index
        i = 0;
        memset(currentToken, '\0', sizeof(currentToken));

        // Handle next character
        if ((c == '+' && line[j + 1] == '+') || (c == '-' && line[j + 1] == '-')) {
            // Handle unary operators (++, --) following an identifier
            currentToken[i++] = c;
            currentToken[i++] = line[++j];
            currentToken[i] = '\0';
            writeToken(symbolTable, "UnaryOperator", currentToken, lineNumber);
            i = 0;
            state = START;
        } else if (isDelimiter(c)) {
            currentToken[i++] = c;
            currentToken[i] = '\0';
            writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
            i = 0;
            state = START;
        } else if (!isspace(c)) {
            currentToken[i++] = c;
            currentToken[i] = '\0';
            writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
            i = 0;
            state = START;
        } else {
            state = START;
        }
    }
    break;



















                        // FSM for INT_LITERALs
                        case INTEGER:
                            if (isdigit(c)) {
                                // Continue building the integer
                                currentToken[i++] = c;

                            } else if (c == '.' && isdigit(line[j + 1])) {
                                // Transition to FLOAT state if '.' is followed by a digit
                                currentToken[i++] = c;
                                state = FLOAT;

                            } else if (isalpha(c) || c == '_') {
                                // Invalid integer (e.g., "14anneVariable")
                                while (isalnum(c) || c == '_') {
                                    currentToken[i++] = c;
                                    c = line[++j];
                                }
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                                i = 0;
                                state = START;
                                j--; // Reprocess the last character

                            } else {
                                // Finalize integer when encountering a valid delimiter or space
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "INT_LITERAL", currentToken, lineNumber);
                                i = 0;

                                // Handle next character
                                if (isDelimiter(c)) {
                                    currentToken[i++] = c;
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
                                    i = 0;
                                    state = START;

                                } else if (!isspace(c)) {
                                    // Reprocess invalid character
                                    currentToken[i++] = c;
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                                    i = 0;
                                    state = START;
                                } else {
                                    state = START;
                                }
                            }
                            break;






                        // FSM for FLOAT_LITERALs
                        case FLOAT:
                            if (isdigit(c)) {
                                // Continue building the FLOAT_LITERAL
                                currentToken[i++] = c;

                            } else {
                                // FLOAT_LITERAL complete
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "FLOAT_LITERAL", currentToken, lineNumber);
                                i = 0;

                                // Handle next character
                                if (isDelimiter(c)) {
                                    currentToken[i++] = c;
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
                                    i = 0;
                                    state = START;

                                } else if (!isspace(c)) {
                                    // Reprocess invalid character
                                    currentToken[i++] = c;
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                                    i = 0;
                                    state = START;
                                } else {
                                    state = START;
                                }
                            }
                            break;








                        // FSM for char literal
                        case CHAR_LITERAL:
                            if (c == '\'') {
                                // End of CHAR_LITERAL
                                currentToken[i++] = c;
                                currentToken[i] = '\0';
                                if (i == 3 || (i == 4 && currentToken[1] == '\\')) { // Valid if one character or escape sequence
                                    writeToken(symbolTable, "CHAR_LITERAL", currentToken, lineNumber);
                                } else {
                                    writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                                }
                                i = 0;
                                state = START;

                            } else if (i == 2) {
                                // Error for more than one character (e.g., 'ab')
                                currentToken[i++] = c;
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                                i = 0;
                                state = START;
                            } else {

                                // Add character to the literal
                                currentToken[i++] = c;
                            }
                            break;







                        // FSM for STRING_LITERALs
                        case STRING_LITERAL:
                            if (c == '"') {
                                // End of STRING_LITERAL
                                currentToken[i++] = c;
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "STRING_LITERAL", currentToken, lineNumber);
                                i = 0;
                                state = START;

                            } else {
                                // Add character to the string
                                currentToken[i++] = c;
                            }
                            break;





                        // FSM for arithmetic operations
                        case ARITHMETIC_OPERATOR:
                            if (currentToken[0] == '+') {
                                // Addition operator
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '-') {
                                // Subtraction operator
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '*') {
                                // Multiplication operator
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '/') {
                                if (line[j] == '/') {
                                    // Integer division operator (//)
                                    currentToken[i++] = line[j++]; // Append the second '/'
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                                } else {
                                    // Division operator
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                                }
                            
                            } else if (currentToken[0] == '%') {
                                // Modulo operator
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '^') {
                                // Exponentiation operator
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "ArithmeticOperator", currentToken, lineNumber);
                            }
        
                            // Reset state for next token
                            i = 0;
                            state = START;
                            break;





                        // FSM for asignal operator
                        case ASSIGNMENT_OPERATOR:
                            if (currentToken[0] == '=') {
                                if (line[j] == '=') {
                                    // Relational Operator (==)
                                    currentToken[i++] = line[j++]; // Append second '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                                
                                } else {
                                    // AssignmentOperator (=)
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                                
                                }
                            } else if (currentToken[0] == '+') {
                                if (line[j] == '=') {
                                    // AssignmentOperator (+=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                                
                                }
                            } else if (currentToken[0] == '-') {
                                if (line[j] == '=') {
                                    // AssignmentOperator (-=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                                
                                }
                            } else if (currentToken[0] == '*') {
                                if (line[j] == '=') {
                                    // AssignmentOperator (*=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                                
                                }
                            } else if (currentToken[0] == '/') {
                                if (line[j] == '=') {
                                    // AssignmentOperator (/=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                                
                                }
                            } else if (currentToken[0] == '%') {
                                if (line[j] == '=') {
                                    // AssignmentOperator (%=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "AssignmentOperator", currentToken, lineNumber);
                                }
                            }
                            i = 0;
                            state = START;
                            break;










                        // FSM for relational operators
                        case RELATIONAL_OPERATOR:
                            if (currentToken[0] == '=' && line[j] == '=') {
                                // Equal to operator (==)
                                currentToken[i++] = line[j++]; // Append second '='
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '!' && line[j] == '=') {
                                // Not equal to operator (!=)
                                currentToken[i++] = line[j++]; // Append second '='
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '<') {
                                if (line[j] == '=') {
                                    // Less than or equal to operator (<=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                                } else {
                                    // Less than operator (<)
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                                }
                            
                            } else if (currentToken[0] == '>') {
                                if (line[j] == '=') {
                                    // Greater than or equal to operator (>=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                                } else {
                                    // Greater than operator (>)
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                                }
                            }
                            i = 0;
                            state = START;
                            break;





                        // FSM for logical operators
                        case LOGICAL_OPERATOR:
                            if (currentToken[0] == '!') {
                                if (line[j] == '=') {
                                    // Relational Operator (!=)
                                    currentToken[i++] = line[j++]; // Append '='
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "RelationalOperator", currentToken, lineNumber);
                                
                                } else {
                                    // Logical Operator (!)
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "LogicalOperator", currentToken, lineNumber);
                                }
                            
                            } else if (currentToken[0] == '&') {
                                if (line[j] == '&') {
                                    // Logical AND Operator (&&)
                                    currentToken[i++] = line[j++]; // Append second '&'
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "LogicalOperator", currentToken, lineNumber);
                                } else {
                                    // Single & (fallback to unknown or further usage)
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "UnknownOperator", currentToken, lineNumber);
                                }
                            
                            } else if (currentToken[0] == '|') {
                                if (line[j] == '|') {
                                    // Logical OR Operator (||)
                                    currentToken[i++] = line[j++]; // Append second '|'
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "LogicalOperator", currentToken, lineNumber);
                                } else {
                                    // Single | (fallback to unknown or further usage)
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "UnknownOperator", currentToken, lineNumber);
                                }
                            }
                            i = 0;
                            state = START;
                            break;




                        // FSM for unary operators
                        case UNARY_OPERATOR:
    if (currentToken[0] == '+' && line[j] == '+') {
        // Increment Operator (++)
        currentToken[i++] = line[j++]; // Append second '+'
        currentToken[i] = '\0';        // Null-terminate
        writeToken(symbolTable, "UnaryOperator", currentToken, lineNumber);
    } else if (currentToken[0] == '-' && line[j] == '-') {
        // Decrement Operator (--)
        currentToken[i++] = line[j++]; // Append second '-'
        currentToken[i] = '\0';        // Null-terminate
        writeToken(symbolTable, "UnaryOperator", currentToken, lineNumber);
    }

    // Reset state for next token
    i = 0;
    state = START;
    j--; // Reprocess the current character if not consumed
    break;


        

                        // FSM for delimiters
                        case DELIMITER:
                            currentToken[i] = '\0'; // Null-terminate the token

                            if (currentToken[0] == '(') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == ')') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == '{') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == '}') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == '[') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == ']') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == ',') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == '.') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == ';') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == ':') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == '\'') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else if (currentToken[0] == '"') {
                                writeToken(symbolTable, "Delimiter (Delimiter)", currentToken, lineNumber);
                            } else {
                                writeToken(symbolTable, "UnknownDelimiter", currentToken, lineNumber);
                            }

                            // Reset token index and state for the next character
                            i = 0;
                            state = START;
                            break;
                            

    

                        // FSM for unknown tokens
                        case ERROR:
                            currentToken[i] = '\0'; // Null-terminate the token
                            writeToken(symbolTable, "LexicalError", currentToken, lineNumber);
                            i = 0; // Reset token index
                            memset(currentToken, 0, sizeof(currentToken)); // Clear the currentToken
                        
                            // Reprocess current character if not whitespace or delimiter
                            if (!isspace(c) && !isDelimiter(c)) {
                                j--; // Reprocess the character in the START state
                            }
                        
                            state = START; // Return to START state
                            break;




                    }
    }

                // Closing the leftover tokens after tokenization (reverts back to START state)
                if (i > 0) {
                    currentToken[i] = '\0'; // Null-terminate the token
                    Token *token = NULL;
                
                    // Ensure the leftover token is valid and not part of a comment
                    if (inComment) {
                        // Skip leftover tokens inside comments
                        i = 0; // Reset token index
                        memset(currentToken, 0, sizeof(currentToken)); // Clear currentToken
                        return; // Exit processing for this line
                    }
                
                    // Process the leftover token based on its state
                    switch (state) {
                        case IDENTIFIER:
                            token = keywords(currentToken, lineNumber); // Check for keywords
                            if (token) {
                                writeToken(symbolTable, token->type, token->value, lineNumber);
                                free(token);
                            } else if (isReservedWord(currentToken)) {
                                writeToken(symbolTable, "ReservedWord", currentToken, lineNumber);
                            } else if (isNoiseWord(currentToken)) {
                                writeToken(symbolTable, "NoiseWord", currentToken, lineNumber);
                            } else {
                                writeToken(symbolTable, "IDENTIFIER", currentToken, lineNumber);
                            }
                            break;
                
                        case INTEGER:
                            writeToken(symbolTable, "INT_LITERAL", currentToken, lineNumber);
                            break;
                
                        case FLOAT:
                            writeToken(symbolTable, "FLOAT_LITERAL", currentToken, lineNumber);
                            break;
                
                        // Add more cases if needed for other states...
                
                        default:
                            writeToken(symbolTable, "Unknown", currentToken, lineNumber);
                            break;
                    }
                
                    // Reset token index and currentToken
                    i = 0;
                    memset(currentToken, 0, sizeof(currentToken));
                }


        

    } // processLine end

