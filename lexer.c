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
    CHAR_LITERAL, 
    STRING_LITERAL, 
    ERROR
} State;


// Function prototypes
void processLine(char *line, int lineNumber, FILE *symbolTable);
void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber);
void writeHorizantalBar(FILE *symbolTable);
void trimWhitespace(char *str);
int isDelimiter(char c);
Token* makeToken(const char* type, const char* value, int lineNumber);
Token* keywords(char* lexeme, int len, int line_number);
Token* reservedWords(char* lexeme, int len, int line_number);
Token* noiseWords(char* lexeme, int len, int line_number);
int validFiletype(const char *fileName);


int main() { // main function

    // Source file selection and validation
    const char *fileName = "SourceCode.prsm";

    if(validFiletype(fileName)){
        fprintf(stderr, "Error: Invalid file type. Only .prsm are accepted\n");
        return 1;
    }

        // Validate if the source has the .prsm extension
        FILE *sourceFile = fopen(fileName, "r");
        if (!sourceFile) {
            perror("Error opening file");
            return 1;
        }

        FILE *symbolTable = fopen("symbol_table.prsm", "w");
        if (!symbolTable) {
            perror("Error opening symbol_table.prsm");
            return 1;
        }

    // Write header
    writeHorizantalBar(symbolTable);
    fprintf(symbolTable, "%-65s%-30s%-20s\n", "Token Type", "Value", "LineNumber");
    writeHorizantalBar(symbolTable);

    // Writes the header ti the symbol table
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

        // Process multi-line comments like an NFA, comment symbols are read independently
        int startCommentIndex, endCommentIndex;
        startCommentIndex = strstr(line, "~/") ? strstr(line, "~/") - line : -1;
        endCommentIndex = strstr(line, "/~") ? strstr(line, "/~") - line : -1;

        // Process multi-line comments
        while (startCommentIndex != -1 || endCommentIndex != -1) {
            if (startCommentIndex != -1 && (endCommentIndex == -1 || startCommentIndex < endCommentIndex)) {
                // Found a "Multi-line Comment Start" before an "End" or no "End"
                writeToken(symbolTable, "Multi-line Comment Start", "~/", lineNumber);
                inComment = 1;
                startCommentIndex = strstr(line + startCommentIndex + 2, "~/") 
                                    ? strstr(line + startCommentIndex + 2, "~/") - line 
                                    : -1;
            } else if (endCommentIndex != -1) {
                // Found a "Multi-line Comment End"
                writeToken(symbolTable, "Multi-line Comment End", "/~", lineNumber);
                inComment = 0;
                endCommentIndex = strstr(line + endCommentIndex + 2, "/~") 
                                  ? strstr(line + endCommentIndex + 2, "/~") - line 
                                  : -1;
            }
        }

            // If still in a multi-line comment after processing the line
            if (inComment) {
                lineNumber++;
                continue;  // Ignore content inside the ongoing multi-line comment
            }


        // Handles multi-line comments, toggling the inComment flag
        processLine(line, lineNumber, symbolTable);
        lineNumber++;
    }
        // Closees the file opened
        fclose(sourceFile);
        fclose(symbolTable);

        // Console notice for successful reading
        printf("Lexical analysis completed. Tokens saved in symbol_table.prsm\n");
        return 0;
    }

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

        // Check if identifier matches reserved words, keywords, or noise words
        token = reservedWords(currentToken, strlen(currentToken), lineNumber);
        if (!token) token = keywords(currentToken, strlen(currentToken), lineNumber);
        if (!token) token = noiseWords(currentToken, strlen(currentToken), lineNumber);

        if (token) {
            writeToken(symbolTable, token->type, token->value, token->lineNumber);
            free(token);
        } else {
            writeToken(symbolTable, "Identifier", currentToken, lineNumber);
        }

        i = 0;
        memset(currentToken, 0, sizeof(currentToken));

        // Handle next character
        if (isDelimiter(c)) {
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
            state = START;
        }
    }
    break;

                        case INTEGER:
                            if (isdigit(c)) {
                                // Continue building the integer
                                currentToken[i++] = c;
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
















                        case FLOAT:
                            if (isdigit(c)) {
                                // Continue building the float literal
                                currentToken[i++] = c;
                            } else if (c == '.' && isdigit(line[j + 1])) {
                                // Allow another decimal point only if followed by a digit
                                currentToken[i++] = c;
                            } else {
                                // Float literal complete
                                currentToken[i] = '\0';
                                writeToken(symbolTable, "Float Literal", currentToken, lineNumber);
                                i = 0;
                                state = START;
                                j--; // Reprocess current character
                            }
                            break;








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

// Closing the leftover tokens
if (i > 0) {
    currentToken[i] = '\0'; // Null-terminate the token
    Token *token = NULL;

    switch (state) {
        case IDENTIFIER:
            token = reservedWords(currentToken, strlen(currentToken), lineNumber);
            if (!token) token = keywords(currentToken, strlen(currentToken), lineNumber);
            if (!token) token = noiseWords(currentToken, strlen(currentToken), lineNumber);

            if (token) {
                writeToken(symbolTable, token->type, token->value, token->lineNumber);
                free(token);
            } else {
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





void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber) {
    if (symbolTable != NULL && type != NULL && value != NULL) {
        fprintf(symbolTable, "%-65s%-30s%-20d\n", type, value, lineNumber);
    }
}


// Supporting functions
void writeHorizantalBar(FILE *symbolTable) {
    if (symbolTable != NULL) {
        fprintf(symbolTable, "--------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
}

// Whitespace removal function
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

int isDelimiter(char c) {
    return (c == ',' || c == '.' || c == ';' || c == ':' || 
            c == '(' || c == ')' || c == '{' || c == '}' || 
            c == '[' || c == ']' || c == '"' || c == '\'');
}

// Recursive tokenization
Token* makeToken(const char* type, const char* value, int lineNumber) {
    Token* newToken = (Token*)malloc(sizeof(Token));
    if (!newToken) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strncpy(newToken->type, type, sizeof(newToken->type) - 1);
    newToken->type[sizeof(newToken->type) - 1] = '\0';
    strncpy(newToken->value, value, sizeof(newToken->value) - 1);
    newToken->value[sizeof(newToken->value) - 1] = '\0';
    newToken->lineNumber = lineNumber;
    return newToken;
}


Token* keywords(char* lexeme, int len, int line_number) {
    switch (lexeme[0]) {
        case 'a': // array
            if (len == 5) {
                switch (lexeme[1]) {
                    case 'r':
                        switch (lexeme[2]) {
                            case 'r':
                                switch (lexeme[3]) {
                                    case 'a':
                                        switch (lexeme[4]) {
                                            case 'y':
                                                return makeToken("Keyword", "array", line_number);
                                        }
                                }
                        }
                }
            }
            break;

        case 'b': // bool, break
            if (len == 4) { // bool
                switch (lexeme[1]) {
                    case 'o':
                        switch (lexeme[2]) {
                            case 'o':
                                switch (lexeme[3]) {
                                    case 'l':
                                        return makeToken("Keyword", "bool", line_number);
                                }
                        }
                }
            }
            else if (len == 5) { // break
                switch (lexeme[1]) {
                    case 'r':
                        switch (lexeme[2]) {
                            case 'e':
                                switch (lexeme[3]) {
                                    case 'a':
                                        switch (lexeme[4]) {
                                            case 'k':
                                                return makeToken("Keyword", "break", line_number);
                                        }
                                }
                        }
                }
            }
            break;

        case 'c': // case, char, continue
            if (len == 4) { // case, char
                switch (lexeme[1]) {
                    case 'a':
                        switch (lexeme[2]) {
                            case 's':
                                switch (lexeme[3]) {
                                    case 'e':
                                        return makeToken("Keyword", "case", line_number);
                                }
                        }
                        break;
                    case 'h':
                        switch (lexeme[2]) {
                            case 'a':
                                switch (lexeme[3]) {
                                    case 'r':
                                        return makeToken("Keyword", "char", line_number);
                                }
                        }
                        break;
                }
            }
            else if (len == 8) { // continue
                switch (lexeme[1]) {
                    case 'o':
                        switch (lexeme[2]) {
                            case 'n':
                                switch (lexeme[3]) {
                                    case 't':
                                        switch (lexeme[4]) {
                                            case 'i':
                                                switch (lexeme[5]) {
                                                    case 'n':
                                                        switch (lexeme[6]) {
                                                            case 'u':
                                                                switch (lexeme[7]) {
                                                                    case 'e':
                                                                        return makeToken("Keyword", "continue", line_number);
                                                                }
                                                        }
                                                }
                                        }
                                }
                        }
                }
            }
            break;

        case 'd': // default, do
            if (len == 2) { // do
                switch (lexeme[1]) {
                    case 'o':
                        return makeToken("Keyword", "do", line_number);
                }
            }
            else if (len == 7) { // default
                switch (lexeme[1]) {
                    case 'e':
                        switch (lexeme[2]) {
                            case 'f':
                                switch (lexeme[3]) {
                                    case 'a':
                                        switch (lexeme[4]) {
                                            case 'u':
                                                switch (lexeme[5]) {
                                                    case 'l':
                                                        switch (lexeme[6]) {
                                                            case 't':
                                                                return makeToken("Keyword", "default", line_number);
                                                        }
                                                }
                                        }
                                }
                        }
                }
            }
            break;

        case 'e': // else, else if
            if (len == 4) { // else
                switch (lexeme[1]) {
                    case 'l':
                        switch (lexeme[2]) {
                            case 's':
                                switch (lexeme[3]) {
                                    case 'e':
                                        return makeToken("Keyword", "else", line_number);
                                }
                        }
                }
            }
            else if (len == 7) { // else if
                switch (lexeme[1]) {
                    case 'l':
                        switch (lexeme[2]) {
                            case 's':
                                switch (lexeme[3]) {
                                    case 'e':
                                        switch (lexeme[4]) {
                                            case ' ':
                                                switch (lexeme[5]) {
                                                    case 'i':
                                                        switch (lexeme[6]) {
                                                            case 'f':
                                                                return makeToken("Keyword", "else if", line_number);
                                                        }
                                                }
                                        }
                                }
                        }
                }
            }
            break;

        case 'f': // float, for
            if (len == 3) { // for
                switch (lexeme[1]) {
                    case 'o':
                        switch (lexeme[2]) {
                            case 'r':
                                return makeToken("Keyword", "for", line_number);
                        }
                }
            }
            else if (len == 5) { // float
                switch (lexeme[1]) {
                    case 'l':
                        switch (lexeme[2]) {
                            case 'o':
                                switch (lexeme[3]) {
                                    case 'a':
                                        switch (lexeme[4]) {
                                            case 't':
                                                return makeToken("Keyword", "float", line_number);
                                        }
                                }
                        }
                }
            }
            break;

        case 'g': // goto
            if (len == 4) {
                switch (lexeme[1]) {
                    case 'o':
                        switch (lexeme[2]) {
                            case 't':
                                switch (lexeme[3]) {
                                    case 'o':
                                        return makeToken("Keyword", "goto", line_number);
                                }
                        }
                }
            }
            break;

        case 'i': // if, int, input
            if (len == 2) { // if
                switch (lexeme[1]) {
                    case 'f':
                        return makeToken("Keyword", "if", line_number);
                }
            }
            else if (len == 3) { // int
                switch (lexeme[1]) {
                    case 'n':
                        switch (lexeme[2]) {
                            case 't':
                                return makeToken("Keyword", "int", line_number);
                        }
                }
            }
            else if (len == 5) { // input
                switch (lexeme[1]) {
                    case 'n':
                        switch (lexeme[2]) {
                            case 'p':
                                switch (lexeme[3]) {
                                    case 'u':
                                        switch (lexeme[4]) {
                                            case 't':
                                                return makeToken("Keyword", "input", line_number);
                                        }
                                }
                        }
                }
            }
            break;

        case 'm': // main
            if (len == 4) {
                switch (lexeme[1]) {
                    case 'a':
                        switch (lexeme[2]) {
                            case 'i':
                                switch (lexeme[3]) {
                                    case 'n':
                                        return makeToken("Keyword", "main", line_number);
                                }
                        }
                }
            }
            break;

        case 'p': // printf
            if (len == 6) {
                switch (lexeme[1]) {
                    case 'r':
                        switch (lexeme[2]) {
                            case 'i':
                                switch (lexeme[3]) {
                                    case 'n':
                                        switch (lexeme[4]) {
                                            case 't':
                                                switch (lexeme[5]) {
                                                    case 'f':
                                                        return makeToken("Keyword", "printf", line_number);
                                                }
                                        }
                                }
                        }
                }
            }
            break;

        case 'r': // return
            if (len == 6) {
                switch (lexeme[1]) {
                    case 'e':
                        switch (lexeme[2]) {
                            case 't':
                                switch (lexeme[3]) {
                                    case 'u':
                                        switch (lexeme[4]) {
                                            case 'r':
                                                switch (lexeme[5]) {
                                                    case 'n':
                                                        return makeToken("Keyword", "return", line_number);
                                                }
                                        }
                                }
                        }
                }
            }
            break;

        case 's': // string, switch
            if (len == 6) { // string
                switch (lexeme[1]) {
                    case 't':
                        switch (lexeme[2]) {
                            case 'r':
                                switch (lexeme[3]) {
                                    case 'i':
                                        switch (lexeme[4]) {
                                            case 'n':
                                                switch (lexeme[5]) {
                                                    case 'g':
                                                        return makeToken("Keyword", "string", line_number);
                                                }
                                        }
                                }
                        }
                }
            switch (lexeme[1]) {
                    case 'w':
                        switch (lexeme[2]) {
                            case 'i':
                                switch (lexeme[3]) {
                                    case 't':
                                        switch (lexeme[4]) {
                                            case 'c':
                                                switch (lexeme[5]) {
                                                    case 'h':
                                                        return makeToken("Keyword", "switch", line_number);
                                                }
                                        }
                                }
                        }
                }
            }

            break;

        case 'v': // void
            if (len == 4) {
                switch (lexeme[1]) {
                    case 'o':
                        switch (lexeme[2]) {
                            case 'i':
                                switch (lexeme[3]) {
                                    case 'd':
                                        return makeToken("Keyword", "void", line_number);
                                }
                        }
                }
            }
            break;

        case 'w': // while
            if (len == 5) {
                switch (lexeme[1]) {
                    case 'h':
                        switch (lexeme[2]) {
                            case 'i':
                                switch (lexeme[3]) {
                                    case 'l':
                                        switch (lexeme[4]) {
                                            case 'e':
                                                return makeToken("Keyword", "while", line_number);
                                        }
                                }
                        }
                }
            }
            break;
    }
    return NULL;
}


Token* reservedWords(char* lexeme, int len, int line_number) {
    switch (lexeme[0]) {
        case 'c': // const
            if (len == 5) {
                switch (lexeme[1]) {
                    case 'o':
                        switch (lexeme[2]) {
                            case 'n':
                                switch (lexeme[3]) {
                                    case 's':
                                        switch (lexeme[4]) {
                                            case 't':
                                                return makeToken("Reserved", "const", line_number);
                                        }
                                }
                        }
                }
            }
            break;

        case 'f': // false
            if (len == 5) {
                switch (lexeme[1]) {
                    case 'a':
                        switch (lexeme[2]) {
                            case 'l':
                                switch (lexeme[3]) {
                                    case 's':
                                        switch (lexeme[4]) {
                                            case 'e':
                                                return makeToken("Reserved", "false", line_number);
                                        }
                                }
                        }
                }
            }
            break;

        case 'n': // null
            if (len == 4) {
                switch (lexeme[1]) {
                    case 'u':
                        switch (lexeme[2]) {
                            case 'l':
                                switch (lexeme[3]) {
                                    case 'l':
                                        return makeToken("Reserved", "null", line_number);
                                }
                        }
                }
            }
            break;

        case 't': // true
            if (len == 4) {
                switch (lexeme[1]) {
                    case 'r':
                        switch (lexeme[2]) {
                            case 'u':
                                switch (lexeme[3]) {
                                    case 'e':
                                        return makeToken("Reserved", "true", line_number);
                                }
                        }
                }
            }
            break;
    }
    return NULL;
}

Token* noiseWords(char* lexeme, int len, int line_number) {
    switch (lexeme[0]) {
        case 'b': // by
            if (len == 2) {
                switch (lexeme[1]) {
                    case 'y':
                        return makeToken("Noise", "by", line_number);                                       
                }
            }
            break;

        case 'f': // from
            if (len == 4) {
                switch (lexeme[1]) {
                    case 'r':
                        switch (lexeme[2]) {
                            case 'o':
                                switch (lexeme[3]) {
                                    case 'm':
                                       return makeToken("Noise", "from", line_number);
                                }
                        }
                }
            }
            break;

        case 'u': // until
            if (len == 5) {
                switch (lexeme[1]) {
                    case 'n':
                        switch (lexeme[2]) {
                            case 't':
                                switch (lexeme[3]) {
                                    case 'i':
                                        switch (lexeme[4]) {
                                            case 'l':
                                                return makeToken("Noise", "until", line_number);
                                        }
                                }
                        }
                }
            }
            break;
    }
    return NULL;
}
int validFiletype(const char *fileName){
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
}
