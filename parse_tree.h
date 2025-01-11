#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parse Tree Node structure
typedef struct ParseTreeNode {
    char label[50]; // Label for the node (e.g., "Program", "Expression")
    struct ParseTreeNode* children[10]; // Pointers to child nodes
    int childCount; // Number of children
} ParseTreeNode;

// Function prototypes
ParseTreeNode* createParseTreeNode(const char* label);
void addChild(ParseTreeNode* parent, ParseTreeNode* child);
void printParseTree(ParseTreeNode* node, int depth);
void writeParseTreeToFile(ParseTreeNode* node, FILE* file);
void freeParseTree(ParseTreeNode* node);

#endif // PARSE_TREE_H
