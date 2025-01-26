#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H
#define MAX_TOKENS 5000 // Adjust size based on expected token count

#include "token.h"
#include "parse_tree.h"

// ---------------------------------------
// Global Variables - Declaration                   // Rasty
// ---------------------------------------
extern int currentTokenIndex;
extern int totalTokens;
extern Token* tokenStream;
extern Token tokens[MAX_TOKENS]; // Array of tokens

// ---------------------------------------
// Utility Functions - Defined in syntax_analyzer.c     // Rasty
// ---------------------------------------
void syntaxError(const char* message); // Report syntax errors
Token* getNextToken();                 // Retrieve the next token
Token* peekToken();                    // Peek at the current token
Token* peekNextToken();                // Peek at the next token
void trimWhitespace(char* str);        // Utility to trim whitespace
void mapToken(Token* token);           // Map token to its type/value
int loadTokensFromFile(const char* filename); // Load tokens from a file
ParseTreeNode* matchToken(const char* expectedType, const char* expectedValue); // Match token by type/value

// ---------------------------------------
// Parse Tree Handling - Defined in parse_tree.c    // Rasty
// ---------------------------------------
ParseTreeNode* createParseTreeNode(const char* type, const char* value); // Create a parse tree node
void addChild(ParseTreeNode* parent, ParseTreeNode* child);             // Add a child node
void writeParseTreeToFile(ParseTreeNode* node, FILE* file, int depth);  // Write parse tree to a file
void freeParseTree(ParseTreeNode* node);                                // Free parse tree memory

// ---------------------------------------
// Top-Level Grammar Rules                      // Rasty
// ---------------------------------------
ParseTreeNode* parseProgram();
ParseTreeNode* parseMainFunction();                              
ParseTreeNode* parseStatementList();

// ---------------------------------------
// Comments                                     // Rasty
// ---------------------------------------
ParseTreeNode* parseComment();                       

// ---------------------------------------
// Declaration Statements                       // Rasty
// ---------------------------------------
ParseTreeNode* parseDeclarationStatement();
ParseTreeNode* parseVariableDeclaration();
ParseTreeNode* parseTypeSpecifier();
ParseTreeNode* parseInitializer();

// ---------------------------------------
// Statements
// ---------------------------------------
ParseTreeNode* parseStatement();                   // rasty
ParseTreeNode* parseAssignmentStatement();
ParseTreeNode* parseInputStatement();
ParseTreeNode* parseOutputStatement();
ParseTreeNode* parseConditionalStatement();
ParseTreeNode* parseIterativeStatement();         
ParseTreeNode* parseFunctionStatement();
ParseTreeNode* parseExpressionStatement();      
ParseTreeNode* parseJumpStatement();

// ---------------------------------------
// Conditional                                  // ryan
// ---------------------------------------    
ParseTreeNode* parseIfStatement();               
ParseTreeNode* parseStatementBlock();

// ---------------------------------------
// Loop                                         // kurt
// ---------------------------------------
ParseTreeNode* parseForInit();
ParseTreeNode* parseForLoop();
ParseTreeNode* parseForUpdate();                

// ---------------------------------------
// Expressions and Operators                    // rasty
// ---------------------------------------
ParseTreeNode* parseExpression();
ParseTreeNode* parseRelationalExpr();     
ParseTreeNode* parseBoolExpr(); 
ParseTreeNode* parseAssignmentExpr();     
ParseTreeNode* parseArithmeticExpr();
ParseTreeNode* parseAssignExpr();
ParseTreeNode* parseBoolTerm();
ParseTreeNode* parseBoolFactor();
ParseTreeNode* parseUnaryExpr();
ParseTreeNode* parseExponentialExpr();
ParseTreeNode* parseTerm();
ParseTreeNode* parseBase();

// ---------------------------------------
// Literals and Identifiers                     
// ---------------------------------------
ParseTreeNode* parseLiteral();
ParseTreeNode* parseBoolLiteral();
ParseTreeNode* parsePrimaryExpr();

// ---------------------------------------
// Input and Output Specific Functions          // anne
// ---------------------------------------
ParseTreeNode* parseInputList();
ParseTreeNode* parseFormatVariablePair();
ParseTreeNode* parseFormatSpecifier();          
ParseTreeNode* parseAddressVariable();          
ParseTreeNode* parseOutputList();
ParseTreeNode* parseExpressionList();
ParseTreeNode* parseFormatString();
ParseTreeNode* parseIdentifierExpr();
ParseTreeNode* parseTerm();
ParseTreeNode* parseFactor();

// ---------------------------------------
// Error Handling                               // Rasty
// ---------------------------------------
int reportSyntaxError(const char *message);
int recoverFromError(); // Error recovery mechanism

#endif // SYNTAX_ANALYZER_H