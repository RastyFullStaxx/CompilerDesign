#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H
#define MAX_TOKENS 1000 // Adjust size based on expected token count

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
void writeParseTreeToFile(ParseTreeNode* node, FILE* file);             // Write parse tree to a file
void freeParseTree(ParseTreeNode* node);                                // Free parse tree memory



// !!!! All lines below are defined in syntax_analyzer.c


// ---------------------------------------
// Top-Level Grammar Rules                      // Rasty
// ---------------------------------------
ParseTreeNode* parseProgram();
ParseTreeNode* parseMainFunction();             
ParseTreeNode* parseBlock();                    
ParseTreeNode* parseStatementList();

// ---------------------------------------
// Comments                                     // Rasty
// ---------------------------------------
ParseTreeNode* parseComment();                  // norecovery yet

// ---------------------------------------
// Initializer and Lists                        // Rasty
// ---------------------------------------
// ParseTreeNode* parseInitializer();              
// ParseTreeNode* parseInitializerList();          

// ---------------------------------------
// Declaration Statements                       // Rasty
// ---------------------------------------
ParseTreeNode* parseDeclarationStatement();
ParseTreeNode* parseVariableDeclaration();
ParseTreeNode* parseTypeSpecifier();

// ---------------------------------------
// Statements
// ---------------------------------------
ParseTreeNode* parseStatement();                   // no recovery yet
ParseTreeNode* parseAssignmentStatement();
ParseTreeNode* parseInputStatement();
ParseTreeNode* parseOutputStatement();
ParseTreeNode* parseConditionalStatement();
ParseTreeNode* parseIterativeStatement();
//ParseTreeNode* parseArrayStatement();           
ParseTreeNode* parseFunctionStatement();
ParseTreeNode* parseExpressionStatement();      
//ParseTreeNode* parseJumpStatement();

// ---------------------------------------
// Conditional                                  // Lei
// ---------------------------------------
ParseTreeNode* parseCompoundStatement();        
ParseTreeNode* parseIfStatement();              // alraedy has <if-else-statement>,<else-if-statement>
//ParseTreeNode* parseCaseList();                 
//ParseTreeNode* parseSwitchStatement();
//ParseTreeNode* parseCaseStatement();
//ParseTreeNode* parseDefaultCase();
ParseTreeNode* parseStatementBlock();

// ---------------------------------------
// Loop                                         // Alexa
// ---------------------------------------
ParseTreeNode* parseForInit();
ParseTreeNode* parseForLoop();
ParseTreeNode* parseWhileLoop();
ParseTreeNode* parseDoWhileLoop();
ParseTreeNode* parseForUpdate();                // for-loops ata e2
ParseTreeNode* parseDecList();
ParseTreeNode* parseIncDec();
ParseTreeNode* parseDeclarator();

// ---------------------------------------
// Expressions and Operators                    // Kurt
// ---------------------------------------
ParseTreeNode* parseExpression();
ParseTreeNode* parseLogicalOrExpr();
ParseTreeNode* parseLogicalAndExpr();
ParseTreeNode* parseEqualityExpr();
ParseTreeNode* parseRelationalExpr();
ParseTreeNode* parseAdditiveExpr();
ParseTreeNode* parseMultiplicativeExpr();
ParseTreeNode* parseExponentialExpr();
ParseTreeNode* parseUnaryExpr();
ParseTreeNode* parsePostfixExpr();      
ParseTreeNode* parseAssignmentExpr();      

// ---------------------------------------
// Literals and Identifiers                     // Rasty
// ---------------------------------------
ParseTreeNode* parseLiteral();
ParseTreeNode* parseBoolLiteral();
ParseTreeNode* parsePrimaryExpr();

// ---------------------------------------
// Input and Output Specific Functions          // Kurt
// ---------------------------------------
ParseTreeNode* parseInputList();
ParseTreeNode* parseFormatVariablePair();
ParseTreeNode* parseFormatSpecifier();          
ParseTreeNode* parseAddressVariable();          
ParseTreeNode* parseOutputList();
ParseTreeNode* parseExpressionList();
ParseTreeNode* parseFormatString();

// ---------------------------------------
// Array Handling                               // Rasty
// ---------------------------------------
//ParseTreeNode* parseArrayDeclaration();         //
//ParseTreeNode* parseArrayOperations();          //
ParseTreeNode* parseArrayAccess();              //
//ParseTreeNode* parseArrayAssignment();          //
//ParseTreeNode* parseArrayDimensions();          // 
//ParseTreeNode* parseArrayIndex();               // 
//ParseTreeNode* parseArrayType();

// ---------------------------------------
// Functions                                    // Rasty
// ---------------------------------------
ParseTreeNode* parseFunctionDeclaration();      
ParseTreeNode* parseFunctionCall();             
ParseTreeNode* parseParameterList();            
ParseTreeNode* parseArgumentList();             
ParseTreeNode* parseReturnType();               // <return-type>
ParseTreeNode* parseParameter();

// ---------------------------------------
// Error Handling                               // Rasty
// ---------------------------------------
int reportSyntaxError(const char *message);
int recoverFromError(); // Error recovery mechanism

#endif // SYNTAX_ANALYZER_H