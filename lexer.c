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






// FSM states for identifiers (words classification)
typedef enum {
    S,        // Start
    A, R1, R2, A1, // States for 'array'
    B, O1, O2, R3, R4, R5, // States for 'bool', 'break'
    C, CA, CAS, CH, CHA, CO, CON, CONT, CONTI, CONTIN, CONTINU, // States for 'case', 'char', 'continue'
    D, DE, DEF, DEFA, DEFAU, DEFAUL, // States for 'default', 'do'
    E, EL, ELS, // States for 'else'
    F, FA, FAL, FALS, // States for 'false'
    FL, FLO, FLT, FO, // States for 'float', 'for'
    G, GO1, GO2, // States for 'goto'
    I, IN1, INP, INPU, // States for 'if', 'int', 'input'
    R, RE, RET, RETU, RETUR, RETURN, // States for 'return'
    M, MA, MAI, // States for 'main'
    P, PR, PRI, PRIN, PRINT, // States for 'printf'
    S1, STR, STRI, STRIN, STRING, SW, SWI, SWIT, SWITC, // States for 'string', 'switch'
    V, VO, VOI, // States for 'void'
    W, WH, WHI, WHIL, // States for 'while'
    ACCEPT    // Accepting state
} KeywordState;







// Function prototypes
void processLine(char *line, int lineNumber, FILE *symbolTable);
void writeToken(FILE *symbolTable, const char *type, const char *value, int lineNumber);
void writeHorizantalBar(FILE *symbolTable);
void trimWhitespace(char *str);
int isDelimiter(char c);
Token* keywords(char* lexeme, int lineNumber);
Token* makeToken(const char* type, const char* value, int lineNumber); // Updated prototype
int validFiletype(const char *fileName);
int isReservedWord(const char *token);
int isNoiseWord(const char *token);


int main() { // main function

    // Source file selection and validation "fileName.WithValidExtension"
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

Token* makeToken(const char* type, const char* value, int lineNumber) {
    // Allocate memory for the new token
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        // Handle memory allocation failure
        fprintf(stderr, "Error: Memory allocation failed for token.\n");
        exit(EXIT_FAILURE); // Exit if memory allocation fails
    }

    // Copy the type to the token->type array manually
    int i = 0;
    while (type[i] != '\0' && i < (sizeof(token->type) - 1)) {
        token->type[i] = type[i];
        i++;
    }
    token->type[i] = '\0'; // Null-terminate the string

    // Copy the value to the token->value array manually
    i = 0;
    while (value[i] != '\0' && i < (sizeof(token->value) - 1)) {
        token->value[i] = value[i];
        i++;
    }
    token->value[i] = '\0'; // Null-terminate the string

    // Assign the line number
    token->lineNumber = lineNumber;

    return token;
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

// State Machines for Words (keywords, noise and reserved)
Token* keywords(char* lexeme, int lineNumber) {
    char* current = lexeme;
    KeywordState state = S; // Initial state

    while (*current != '\0') {
        switch (state) {
            case S:
                switch (*current) {
                    case 'b': state = B; break; // For 'bool', 'break'
                    case 'c': state = C; break; // For 'case', 'char', 'continue'
                    case 'd': state = D; break; // For 'default', 'do'
                    case 'e': state = E; break; // For 'else'
                    case 'f': state = F; break; // For 'false', 'float', 'for'
                    case 'g': state = G; break; // For 'goto'
                    case 'i': state = I; break; // For 'if', 'int', 'input'
                    case 'm': state = M; break; // For 'main'
                    case 'p': state = P; break; // For 'printf'
                    case 's': state = S1; break; // For 'string', 'switch'
                    case 'v': state = V; break; // For 'void'
                    case 'w': state = W; break; // For 'while'
                    default: return NULL; // Not a keyword
                }
                break;
            
            // state for 'array'   
            case A:
                if (*current == 'r') state = R1;
                else return NULL;
                break;
            case R1: if (*current == 'r') state = R2; else return NULL; break;
            case R2: if (*current == 'a') state = A1; else return NULL; break;
            case A1: if (*current == 'y') state = ACCEPT; else return NULL; break;

            // States for 'bool' and 'break'
            case B:
                if (*current == 'o') state = O1;
                else if (*current == 'r') state = R3;
                else return NULL;
                break;
            case O1: if (*current == 'o') state = O2; else return NULL; break;
            case O2: if (*current == 'l') state = ACCEPT; else return NULL; break;
            case R3: if (*current == 'e') state = R4; else return NULL; break;
            case R4: if (*current == 'a') state = R5; else return NULL; break;
            case R5: if (*current == 'k') state = ACCEPT; else return NULL; break;

            // States for 'case', 'char', 'continue'
            case C:
                if (*current == 'a') state = CA;
                else if (*current == 'h') state = CH;
                else if (*current == 'o') state = CO;
                else return NULL;
                break;
            case CA: if (*current == 's') state = CAS; else return NULL; break;
            case CAS: if (*current == 'e') state = ACCEPT; else return NULL; break;
            case CH: if (*current == 'a') state = CHA; else return NULL; break;
            case CHA: if (*current == 'r') state = ACCEPT; else return NULL; break;
            case CO: if (*current == 'n') state = CON; else return NULL; break;
            case CON: if (*current == 't') state = CONT; else return NULL; break;
            case CONT: if (*current == 'i') state = CONTI; else return NULL; break;
            case CONTI: if (*current == 'n') state = CONTIN; else return NULL; break;
            case CONTIN: if (*current == 'u') state = CONTINU; else return NULL; break;
            case CONTINU: if (*current == 'e') state = ACCEPT; else return NULL; break;

            // States for 'default', 'do'
            case D:
                if (*current == 'e') state = DE;
                else if (*current == 'o') state = ACCEPT; // For 'do'
                else return NULL;
                break;
            case DE: if (*current == 'f') state = DEF; else return NULL; break;
            case DEF: if (*current == 'a') state = DEFA; else return NULL; break;
            case DEFA: if (*current == 'u') state = DEFAU; else return NULL; break;
            case DEFAU: if (*current == 'l') state = DEFAUL; else return NULL; break;
            case DEFAUL: if (*current == 't') state = ACCEPT; else return NULL; break;

            // States for 'else'
            case E: if (*current == 'l') state = EL; else return NULL; break;
            case EL: if (*current == 's') state = ELS; else return NULL; break;
            case ELS: if (*current == 'e') state = ACCEPT; else return NULL; break;

            // States for 'false', 'float', 'for'
            case F:
                if (*current == 'a') state = FA;
                else if (*current == 'l') state = FL;
                else if (*current == 'o') state = FO;
                else return NULL;
                break;
            case FA: if (*current == 'l') state = FAL; else return NULL; break;
            case FAL: if (*current == 's') state = FALS; else return NULL; break;
            case FALS: if (*current == 'e') state = ACCEPT; else return NULL; break;
            case FL: if (*current == 'o') state = FLO; else return NULL; break;
            case FLO: if (*current == 'a') state = FLT; else return NULL; break;
            case FLT: if (*current == 't') state = ACCEPT; else return NULL; break;
            case FO: if (*current == 'r') state = ACCEPT; else return NULL; break;

            // States for 'goto'
            case G: if (*current == 'o') state = GO1; else return NULL; break;
            case GO1: if (*current == 't') state = GO2; else return NULL; break;
            case GO2: if (*current == 'o') state = ACCEPT; else return NULL; break;

            // States for 'if', 'int', 'input'
            case I:
                if (*current == 'n') state = IN1; // For 'int', 'input'
                else if (*current == 'f') state = ACCEPT; // For 'if'
                else return NULL;
                break;
            case IN1: if (*current == 't') state = ACCEPT; // For 'int'
                      else if (*current == 'p') state = INP; // For 'input'
                      else return NULL;
                      break;
            case INP: if (*current == 'u') state = INPU; else return NULL; break;
            case INPU: if (*current == 't') state = ACCEPT; else return NULL; break;

            // States for 'main'
            case M: if (*current == 'a') state = MA; else return NULL; break;
            case MA: if (*current == 'i') state = MAI; else return NULL; break;
            case MAI: if (*current == 'n') state = ACCEPT; else return NULL; break;

            // States for 'printf'
            case P: if (*current == 'r') state = PR; else return NULL; break;
            case PR: if (*current == 'i') state = PRI; else return NULL; break;
            case PRI: if (*current == 'n') state = PRIN; else return NULL; break;
            case PRIN: if (*current == 't') state = PRINT; else return NULL; break;
            case PRINT: if (*current == 'f') state = ACCEPT; else return NULL; break;

            // States for 'string', 'switch'
            case S1:
                if (*current == 't') state = STR;
                else if (*current == 'w') state = SW;
                else return NULL;
                break;
            case STR: if (*current == 'r') state = STRI; else return NULL; break;
            case STRI: if (*current == 'i') state = STRIN; else return NULL; break;
            case STRIN: if (*current == 'n') state = STRING; else return NULL; break;
            case STRING: if (*current == 'g') state = ACCEPT; else return NULL; break;
            case SW: if (*current == 'i') state = SWI; else return NULL; break;
            case SWI: if (*current == 't') state = SWIT; else return NULL; break;
            case SWIT: if (*current == 'c') state = SWITC; else return NULL; break;
            case SWITC: if (*current == 'h') state = ACCEPT; else return NULL; break;

            // States for 'void'
            case V: if (*current == 'o') state = VO; else return NULL; break;
            case VO: if (*current == 'i') state = VOI; else return NULL; break;
            case VOI: if (*current == 'd') state = ACCEPT; else return NULL; break;

            // States for 'while'
            case W: if (*current == 'h') state = WH; else return NULL; break;
            case WH: if (*current == 'i') state = WHI; else return NULL; break;
            case WHI: if (*current == 'l') state = WHIL; else return NULL; break;
            case WHIL: if (*current == 'e') state = ACCEPT; else return NULL; break;

            // State for 'return'
            case R:
                if (*current == 'e') state = RE;
                else return NULL;
                break;
            case RE: if (*current == 't') state = RET; else return NULL; break;
            case RET: if (*current == 'u') state = RETU; else return NULL; break;
            case RETU: if (*current == 'r') state = RETUR; else return NULL; break;
            case RETUR: if (*current == 'n') state = RETURN; else return NULL; break;


            // Accepting state
            case ACCEPT:
                return makeToken("Keyword", lexeme, lineNumber);

            default:
                return NULL;
        }
        current++;
    }

    return (state == ACCEPT) ? makeToken("Keyword", lexeme, lineNumber) : NULL;
}




// file type validation
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

// reserved word helper
int isReservedWord(const char *token) {
    const char *reservedWords[] = {"true", "false", "null", "const"};
    for (int i = 0; i < 4; i++) {
        int j = 0;
        while (token[j] == reservedWords[i][j] && token[j] != '\0' && reservedWords[i][j] != '\0') {
            j++;
        }
        if (token[j] == '\0' && reservedWords[i][j] == '\0') {
            return 1; // Match found
        }
    }
    return 0; // No match
}

// noise word helper
int isNoiseWord(const char *token) {
    const char *noiseWords[] = {"by", "from", "until"};
    for (int i = 0; i < 3; i++) {
        int j = 0;
        while (token[j] == noiseWords[i][j] && token[j] != '\0' && noiseWords[i][j] != '\0') {
            j++;
        }
        if (token[j] == '\0' && noiseWords[i][j] == '\0') {
            return 1; // Match found
        }
    }
    return 0; // No match
}



