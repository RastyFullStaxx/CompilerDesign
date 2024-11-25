#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdio.h>

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




// FSM special words functions
typedef enum {
    S,        // Start
    B, C, D, E, F, G, I, M, P, S1, V, W, // Initial letters for keywords
    A, R1, R2, A1, // States for 'array'
    O1, O2, R3, R4, R5, // States for 'bool', 'break'
    CA, CAS, CH, CHA, CO, CON, CONT, CONTI, CONTIN, CONTINU, // States for 'case', 'char', 'continue'
    DE, DEF, DEFA, DEFAU, DEFAUL, // States for 'default', 'do'
    EL, ELS, // States for 'else'
    FA, FAL, FALS, FL, FLO, FLT, FO, // States for 'false', 'float', 'for'
    GO1, GO2, // States for 'goto'
    IN1, INP, INPU, // States for 'if', 'int', 'input'
    MA, MAI, // States for 'main'
    PR, PRI, PRIN, PRINT, // States for 'printf'
    STR, STRI, STRIN, STRING, SW, SWI, SWIT, SWITC, // States for 'string', 'switch'
    VO, VOI, // States for 'void'
    WH, WHI, WHIL, // States for 'while'
    RE, RET, RETU, RETUR, RETURN, // States for 'return'
    ACCEPT // Final accepting state
} KeywordState;


// Function to process a single line using the FSM
void processLine(char *line, int lineNumber, FILE *symbolTable);

#endif // STATE_MACHINE_H
