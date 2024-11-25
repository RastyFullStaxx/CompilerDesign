#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "file_selector.h"
#include "token.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"

void processLine(char *line, int lineNumber, FILE *symbolTable) {

    // processLine function
void processLine(char *line, int lineNumber, FILE *symbolTable) {
    State state = START;
    char currentToken[50] = "";
    int i = 0;
    int tokenLength = strlen(currentToken);


    for (int j = 0; line[j] != '\0'; j++) {
        char c = line[j];
    
        switch (state) {

            // START case for recursive use ----------------------------------------------------------------------------------------------------------------------------------------
            case START:
                if (c == '~' && line[j + 1] == '~') {
                    /// Single-line comment indicator (~~)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j]; // Consume the second '~'
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Single-line Comment", currentToken, lineNumber);
                    i = 0;
                    state = START; // Reset state
                    return; // Skip the rest of the line
                } else if ((c == '=' && line[j + 1] == '=') || 
                           (c == '!' && line[j + 1] == '=') || 
                           (c == '>' && line[j + 1] == '=') || 
                           (c == '<' && line[j + 1] == '=')) {
                    // Relational operators with '='
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '=') ? "Relational Operator (Equal To)" :
                                       (c == '!') ? "Relational Operator (Not Equal To)" :
                                       (c == '>') ? "Relational Operator (Greater Than or Equal To)" :
                                                    "Relational Operator (Less Than or Equal To)";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '>' || c == '<') {
                    // Relational operators (>, <)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    const char* type = (c == '>') ? "Relational Operator (Greater Than)" :
                                                    "Relational Operator (Less Than)";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '!' && line[j + 1] != '=') {
                    // Logical NOT operator (!)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Logical Operator (NOT)", currentToken, lineNumber);
                    i = 0;
                } else if ((c == '|' && line[j + 1] == '|') || (c == '&' && line[j + 1] == '&')) {
                    // Logical operators (||, &&)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '|') ? "Logical Operator (OR)" : "Logical Operator (AND)";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '=' && line[j + 1] != '=') {
                    // Assignment operator (=)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Assignment Operator", currentToken, lineNumber);
                    i = 0;
                } else if ((c == '+' && line[j + 1] == '=') || (c == '-' && line[j + 1] == '=') ||
                           (c == '*' && line[j + 1] == '=') || (c == '/' && line[j + 1] == '=') || 
                           (c == '%' && line[j + 1] == '=')) {
                    // Assignment operators (+=, -=, *=, /=, %=)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '+') ? "Addition Assignment Operator" :
                                       (c == '-') ? "Subtraction Assignment Operator" :
                                       (c == '*') ? "Multiplication Assignment Operator" :
                                       (c == '/') ? "Division Assignment Operator" :
                                                    "Modulo Assignment Operator";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (c == '/' && line[j + 1] == '/' && line[j + 2] == '=') {
                    // Integer division assignment operator (//=)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Integer Division Assignment Operator", currentToken, lineNumber);
                    i = 0;
                } else if (c == '/' && line[j + 1] == '/') {
                    // Integer division operator (//)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Integer Division)", currentToken, lineNumber);
                    i = 0;
                } else if (c == '/' || c == '*' || c == '%') {
                    // Arithmetic operators (/, *, %)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    const char* type = (c == '/') ? "Arithmetic Operator (Division)" :
                                       (c == '*') ? "Arithmetic Operator (Multiplication)" :
                                                    "Arithmetic Operator (Modulo)";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if ((c == '+' && line[j + 1] == '+') || (c == '-' && line[j + 1] == '-')) {
                    // Unary operators (++, --)
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j];
                    currentToken[i] = '\0';
                    const char* type = (c == '+') ? "Unary Operator (Increment)" : "Unary Operator (Decrement)";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if ((c == '+' || c == '-') && (isdigit(line[j + 1]) || isalpha(line[j + 1]) || line[j + 1] == '_')) {
                    // Unary + or -
                    state = UNARY_OPERATOR;
                    currentToken[i++] = c;
                } else if (c == '+' || c == '-') {
                    // Arithmetic operators (+, -)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    const char* type = (c == '+') ? "Arithmetic Operator (Addition)" : "Arithmetic Operator (Subtraction)";
                    writeToken(symbolTable, type, currentToken, lineNumber);
                    i = 0;
                } else if (isalpha(c) || c == '_') {
                    // Identifier or reserved word
                    state = IDENTIFIER;
                    currentToken[i++] = c;
                } else if (isdigit(c)) {
                    // Integer literal
                    state = INTEGER;
                    currentToken[i++] = c;
                } else if (c == '^') {
                    // Exponentiation operator (^)
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Arithmetic Operator (Exponentiation)", currentToken, lineNumber);
                    i = 0;
                } else if (c == '\'') {
                    // Start of character literal
                    currentToken[i++] = c;
                    state = CHAR_LITERAL;
                } else if (c == '"') {
                    // Start of string literal
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
                    writeToken(symbolTable, "Lexical Error (Invalid Identifier)", currentToken, lineNumber);
                    i = 0;
                    state = START; // Reset state
                    break;
                } else if (isdigit(c)) {
                    // Start of integer
                    state = INTEGER;
                    currentToken[i++] = c;
                } else if (c == '~' && line[j + 1] == '~') {
                    currentToken[i++] = c;
                    currentToken[i++] = line[++j]; // Consume the second '~'
                    currentToken[i] = '\0';
                    writeToken(symbolTable, "Single-line Comment", currentToken, lineNumber);
                    i = 0;
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
                    writeToken(symbolTable, (c == '(') ? "Left Parenthesis" :
                                                   (c == '{') ? "Left Curly Brace" :
                                                                "Left Bracket", currentToken, lineNumber);
                    i = 0; // Reset token index
                } else if (c == ')' || c == '}' || c == ']') {
                    // Right delimiters
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, (c == ')') ? "Right Parenthesis" :
                                                   (c == '}') ? "Right Curly Brace" :
                                                                "Right Bracket", currentToken, lineNumber);
                    i = 0; // Reset token index
                } else if (c == ',' || c == '.' || c == ';' || c == ':') {
                    // Independent delimiters
                    currentToken[i++] = c;
                    currentToken[i] = '\0';
                    writeToken(symbolTable, (c == ',') ? "Comma" :
                                                   (c == '.') ? "Dot" :
                                                   (c == ';') ? "Semicolon" :
                                                                "Colon", currentToken, lineNumber);
                    i = 0; // Reset token index
                } else if (!isspace(c)) {
                    // Other characters (unknown token)
                    currentToken[i++] = c;
                    state = ERROR;
                }
                break;

        







                        // FSM for IDENTIFIER (keywords, noise and reserved would be identified here since they are indetifier in a sense, just special) 
                        case IDENTIFIER:
                            if (isalnum(c) || c == '_') {
                                // Continue building the identifier
                                currentToken[i++] = c;
                            } else if (!isalpha(currentToken[0]) && !isDelimiter(c) && !isspace(c)) {
                                // If identifier starts with an invalid character or contains invalid ones
                                while (isalnum(c) || c == '_' || (!isDelimiter(c) && !isspace(c))) {
                                    currentToken[i++] = c;
                                    c = line[++j]; // Consume the invalid characters
                                }
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Lexical Error (Invalid Identifier)", currentToken, lineNumber);
                                i = 0;
                                state = START;
                                j--; // Reprocess the last character
                            } else {
                                // Finalize identifier when encountering a valid delimiter or space
                                currentToken[i] = '\0';
                                Token *token = NULL;

                                // Check if the identifier is a keyword
                                token = keywords(currentToken, lineNumber);
                                if (token) {
                                    writeToken(symbolTable, token->type, token->value, lineNumber);
                                    free(token);
                                } else if (isReservedWord(currentToken)) {
                                    // Check if the identifier is a reserved word
                                    writeToken(symbolTable, "Reserved Word", currentToken, lineNumber);
                                } else if (isNoiseWord(currentToken)) {
                                    // Check if the identifier is a noise word
                                    writeToken(symbolTable, "Noise Word", currentToken, lineNumber);
                                } else {
                                    // Default to identifier if no matches
                                    writeToken(symbolTable, "Identifier", currentToken, lineNumber);
                                }

                                // Reset token index and current token buffer
                                i = 0;
                                for (int k = 0; k < 50; k++) {
                                    currentToken[k] = '\0';
                                }

                                // Handle next character
                                if (isDelimiter(c)) {
                                    // If the next character is a delimiter, process it
                                    currentToken[i++] = c;
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "Delimiter", currentToken, lineNumber);
                                    i = 0;
                                    state = START;
                                } else if (!isspace(c)) {
                                    // Reprocess invalid character as a lexical error
                                    currentToken[i++] = c;
                                    currentToken[i] = '\0';
                                    writeToken(symbolTable, "Lexical Error (Invalid Identifier)", currentToken, lineNumber);
                                    i = 0;
                                    state = START;
                                } else {
                                    // Default case: reset to START state
                                    state = START;
                                }
                            }
                            break;














                        // FSM for integer literals
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
                                writeToken(symbolTable, "Lexical Error (Invalid Integer)", currentToken, lineNumber);
                                i = 0;
                                state = START;
                                j--; // Reprocess the last character

                            } else {
                                // Finalize integer when encountering a valid delimiter or space
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Integer Literal", currentToken, lineNumber);
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
                                    writeToken(symbolTable, "Lexical Error (Invalid Integer)", currentToken, lineNumber);
                                    i = 0;
                                    state = START;
                                } else {
                                    state = START;
                                }
                            }
                            break;






                        // FSM for float literals
                        case FLOAT:
                            if (isdigit(c)) {
                                // Continue building the float literal
                                currentToken[i++] = c;

                            } else {
                                // Float literal complete
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Float Literal", currentToken, lineNumber);
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
                                    writeToken(symbolTable, "Lexical Error (Invalid Float)", currentToken, lineNumber);
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
                                // End of character literal
                                currentToken[i++] = c;
                                currentToken[i] = '\0';
                                if (i == 3 || (i == 4 && currentToken[1] == '\\')) { // Valid if one character or escape sequence
                                    writeToken(symbolTable, "Character Literal", currentToken, lineNumber);
                                } else {
                                    writeToken(symbolTable, "Lexical Error (Invalid Character Literal)", currentToken, lineNumber);
                                }
                                i = 0;
                                state = START;

                            } else if (i == 2) {
                                // Error for more than one character (e.g., 'ab')
                                currentToken[i++] = c;
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Lexical Error (Invalid Character Literal)", currentToken, lineNumber);
                                i = 0;
                                state = START;
                            } else {

                                // Add character to the literal
                                currentToken[i++] = c;
                            }
                            break;







                        // FSM for string literals
                        case STRING_LITERAL:
                            if (c == '"') {
                                // End of string literal
                                currentToken[i++] = c;
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "String Literal", currentToken, lineNumber);
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





                        // FSM for asignal operator
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










                        // FSM for relational operators
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





                        // FSM for logical operators
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




                        // FSM for unary operators
                        case UNARY_OPERATOR:
                            if (currentToken[0] == '+' && line[j] == '+') {
                                // Increment Operator (++)
                                currentToken[i++] = line[j++]; // Append second '+'
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Unary Operator (Increment)", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '-' && line[j] == '-') {
                                // Decrement Operator (--)
                                currentToken[i++] = line[j++]; // Append second '-'
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Unary Operator (Decrement)", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '+') {
                                // Unary Plus (+i)
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Unary Operator (Positive)", currentToken, lineNumber);
                            
                            } else if (currentToken[0] == '-') {
                                // Unary Minus (-x)
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Unary Operator (Negative)", currentToken, lineNumber);
                            }

                            // Reprocess the next character after the operator
                            i = 0;
                            state = START;
                            j--; // Reprocess the current character
                            break;

        

                        // FSM for delimiters
                        case DELIMITER:
                            currentToken[i] = '\0'; // Null-terminate the token

                            if (currentToken[0] == '(') {
                                writeToken(symbolTable, "Delimiter (Left Parenthesis)", currentToken, lineNumber);
                            } else if (currentToken[0] == ')') {
                                writeToken(symbolTable, "Delimiter (Right Parenthesis)", currentToken, lineNumber);
                            } else if (currentToken[0] == '{') {
                                writeToken(symbolTable, "Delimiter (Left Curly Brace)", currentToken, lineNumber);
                            } else if (currentToken[0] == '}') {
                                writeToken(symbolTable, "Delimiter (Right Curly Brace)", currentToken, lineNumber);
                            } else if (currentToken[0] == '[') {
                                writeToken(symbolTable, "Delimiter (Left Bracket)", currentToken, lineNumber);
                            } else if (currentToken[0] == ']') {
                                writeToken(symbolTable, "Delimiter (Right Bracket)", currentToken, lineNumber);
                            } else if (currentToken[0] == ',') {
                                writeToken(symbolTable, "Delimiter (Comma)", currentToken, lineNumber);
                            } else if (currentToken[0] == '.') {
                                writeToken(symbolTable, "Delimiter (Dot)", currentToken, lineNumber);
                            } else if (currentToken[0] == ';') {
                                writeToken(symbolTable, "Delimiter (Semicolon)", currentToken, lineNumber);
                            } else if (currentToken[0] == ':') {
                                writeToken(symbolTable, "Delimiter (Colon)", currentToken, lineNumber);
                            } else if (currentToken[0] == '\'') {
                                writeToken(symbolTable, "Delimiter (Single Quotation)", currentToken, lineNumber);
                            } else if (currentToken[0] == '"') {
                                writeToken(symbolTable, "Delimiter (Double Quotation)", currentToken, lineNumber);
                            } else {
                                writeToken(symbolTable, "Unknown Delimiter", currentToken, lineNumber);
                            }

                            // Reset token index and state for the next character
                            i = 0;
                            state = START;
                            break;
                            

    

                        // FSM for unknown tokens
                        case ERROR:
                            currentToken[i] = '\0'; // Null-terminate the token
                            writeToken(symbolTable, "Lexical Error (Invalid Identifier)", currentToken, lineNumber);
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

                    switch (state) {
                        case IDENTIFIER:
                        token = keywords(currentToken, lineNumber); // Check for keywords
                        if (token) {
                            // If it's a keyword
                            writeToken(symbolTable, token->type, token->value, lineNumber);
                            free(token);
                        } else if (isReservedWord(currentToken)) {
                            // If it's a reserved word
                            writeToken(symbolTable, "Reserved Word", currentToken, lineNumber);
                        } else if (isNoiseWord(currentToken)) {
                            // If it's a noise word
                            writeToken(symbolTable, "Noise Word", currentToken, lineNumber);
                        } else {
                            // Otherwise, it's an identifier
                            writeToken(symbolTable, "Identifier", currentToken, lineNumber);
                        }
                        break;

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

    } // processLine end

}
