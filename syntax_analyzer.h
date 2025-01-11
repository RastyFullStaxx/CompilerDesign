#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include "token.h"
#include "parse_tree.h"

// Parse Tree Handling
ParseTreeNode* createParseTreeNode(const char* type, const char* value);
void addChild(ParseTreeNode* parent, ParseTreeNode* child);
void writeParseTreeToFile(ParseTreeNode* node, FILE* file); // Write parse tree to a file
void freeParseTree(ParseTreeNode* node); // Free parse tree memory

// Function declarations for the top-level grammar rules
ParseTreeNode* parseProgram();
ParseTreeNode* parseMainFunction();
ParseTreeNode* parseBlock();
ParseTreeNode* parseStatementList();

// Function declarations for specific statement types
ParseTreeNode* parseDeclarationStatement();
ParseTreeNode* parseVariableDeclaration();
ParseTreeNode* parseTypeSpecifier();
ParseTreeNode* parseStatement();
ParseTreeNode* parseAssignmentStatement();
ParseTreeNode* parseInputStatement();
ParseTreeNode* parseOutputStatement();
ParseTreeNode* parseConditionalStatement();
ParseTreeNode* parseIterativeStatement();
ParseTreeNode* parseArrayStatement();
ParseTreeNode* parseFunctionStatement();
ParseTreeNode* parseExpressionStatement();
ParseTreeNode* parseJumpStatement();

// Function declarations for expressions and operators
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

// Function declarations for literals and identifiers
ParseTreeNode* parseLiteral();
ParseTreeNode* parseBoolLiteral();
ParseTreeNode* parsePrimaryExpr();

// Function declarations for control flow
ParseTreeNode* parseIfStatement();
ParseTreeNode* parseSwitchStatement();
ParseTreeNode* parseForLoop();
ParseTreeNode* parseWhileLoop();
ParseTreeNode* parseDoWhileLoop();

// Function declarations for array handling
ParseTreeNode* parseArrayDeclaration();
ParseTreeNode* parseArrayOperations();
ParseTreeNode* parseArrayAccess();
ParseTreeNode* parseArrayAssignment();

// Function declarations for functions
ParseTreeNode* parseForUpdate();
ParseTreeNode* parseFunctionDeclaration();
ParseTreeNode* parseFunctionCall();
ParseTreeNode* parseParameterList();
ParseTreeNode* parseArgumentList();

// Error handling
void reportSyntaxError(const char *message);
void recoverFromError(); // Error recovery mechanism

#endif // SYNTAX_ANALYZER_H
