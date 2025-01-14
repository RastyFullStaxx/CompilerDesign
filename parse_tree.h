#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHILDREN 50 // Example: Increase the max children allowed

// Parse Tree Node structure
typedef struct ParseTreeNode {
    char label[50];  // Label for the node (e.g., "Program", "Expression")
    char value[50];  // Value associated with the node
    struct ParseTreeNode* children[MAX_CHILDREN]; // Pointers to child nodes
    int childCount;   // Number of children
} ParseTreeNode;


// Function prototypes
ParseTreeNode* createParseTreeNode(const char* type, const char* value);
void setNodeValue(ParseTreeNode* node, const char* value);
void addChild(ParseTreeNode* parent, ParseTreeNode* child);
void printParseTree(ParseTreeNode* node, int depth);
void writeParseTreeToFile(ParseTreeNode* node, FILE* file);
void freeParseTree(ParseTreeNode* node);

#endif // PARSE_TREE_H
