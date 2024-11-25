#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_selector.h"
#include "token.h"
#include "state_machine.h"
#include "keywords.h"
#include "utils.h"
#include "comment_handler.h"
#include "config.h"


// Keywords finite state machine (FSM)
Token* keywords(char *lexeme, int lineNumber) {

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

} // end of keyword function




// Reserved word helper
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

} // end of isReservedWord function




// Noise word helper
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

 } // end of isNoiseWord function
