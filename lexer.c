case OPERATOR:
    if (currentToken[0] == '=') {
        if (line[j] == '=') {
            // Equal To Operator (==)
            currentToken[i++] = line[j++]; // Append second '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Relational Operator (Equal To)", currentToken, lineNumber);
        } else {
            // Assignment Operator (=)
            currentToken[i] = '\0';
            writeToken(symbolTable, "Assignment Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '!') {
        if (line[j] == '=') {
            // Not Equal To Operator (!=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Relational Operator (Not Equal To)", currentToken, lineNumber);
        } else {
            // Unknown Operator (!)
            currentToken[i] = '\0';
            writeToken(symbolTable, "Unknown Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '>') {
        if (line[j] == '=') {
            // Greater Than or Equal To Operator (>=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Relational Operator (Greater Than or Equal To)", currentToken, lineNumber);
        } else {
            // Greater Than Operator (>)
            currentToken[i] = '\0';
            writeToken(symbolTable, "Relational Operator (Greater Than)", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '<') {
        if (line[j] == '=') {
            // Less Than or Equal To Operator (<=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Relational Operator (Less Than or Equal To)", currentToken, lineNumber);
        } else {
            // Less Than Operator (<)
            currentToken[i] = '\0';
            writeToken(symbolTable, "Relational Operator (Less Than)", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '+') {
        if (line[j] == '+') { 
            // Increment Operator
            currentToken[i++] = line[j++]; // Append second '+'
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Increment Operator", currentToken, lineNumber);
        } else if (line[j] == '=') {
            // Addition Assignment Operator (+=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Addition Assignment Operator", currentToken, lineNumber);
        } else {
            // Addition Operator
            currentToken[i] = '\0';
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '-') {
        if (line[j] == '-') { 
            // Decrement Operator
            currentToken[i++] = line[j++]; // Append second '-'
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Decrement Operator", currentToken, lineNumber);
        } else if (line[j] == '=') {
            // Subtraction Assignment Operator (-=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Subtraction Assignment Operator", currentToken, lineNumber);
        } else {
            // Subtraction Operator
            currentToken[i] = '\0';
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '*') {
        if (line[j] == '=') {
            // Multiplication Assignment Operator (*=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Multiplication Assignment Operator", currentToken, lineNumber);
        } else {
            // Multiplication Operator
            currentToken[i] = '\0';
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '/') {
        if (line[j] == '/') {
            if (line[j + 1] == '=') {
                // Integer Division Assignment Operator (//=)
                currentToken[i++] = line[j++]; // Append second '/'
                currentToken[i++] = line[j++]; // Append '='
                currentToken[i] = '\0';       // Null-terminate
                writeToken(symbolTable, "Integer Division Assignment Operator", currentToken, lineNumber);
            } else {
                // Integer Division Operator (//)
                currentToken[i++] = line[j++]; // Append second '/'
                currentToken[i] = '\0';       // Null-terminate
                writeToken(symbolTable, "Arithmetic Operator (Integer Division)", currentToken, lineNumber);
            }
        } else if (line[j] == '=') {
            // Division Assignment Operator (/=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Division Assignment Operator", currentToken, lineNumber);
        } else {
            // Division Operator (/)
            currentToken[i] = '\0';
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '%') {
        if (line[j] == '=') {
            // Modulo Assignment Operator (%=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Modulo Assignment Operator", currentToken, lineNumber);
        } else {
            // Modulo Operator
            currentToken[i] = '\0';
            writeToken(symbolTable, "Arithmetic Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else if (currentToken[0] == '~') {
        if (line[j] == '=') {
            // Bitwise NOT Assignment Operator (~=)
            currentToken[i++] = line[j++]; // Append '='
            currentToken[i] = '\0';       // Null-terminate
            writeToken(symbolTable, "Bitwise NOT Assignment Operator", currentToken, lineNumber);
        } else {
            // Bitwise NOT Operator
            currentToken[i] = '\0';
            writeToken(symbolTable, "Bitwise NOT Operator", currentToken, lineNumber);
        }
        i = 0;
        state = START;

    } else {
        // Default case: Handle unknown operators
        currentToken[i] = '\0';
        writeToken(symbolTable, "Unknown Operator", currentToken, lineNumber);
        i = 0;
        state = START;
    }
    break;
