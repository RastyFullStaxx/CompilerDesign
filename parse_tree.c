#include "parse_tree.h"

// Function to create a new parse tree node
ParseTreeNode* createParseTreeNode(const char* label) {
    ParseTreeNode* node = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    if (!node) {
        printf("Error: Memory allocation failed for ParseTreeNode\n");
        exit(EXIT_FAILURE);
    }
    strcpy(node->label, label);
    node->childCount = 0;
    return node;
}

// Function to add a child to a parse tree node
void addChild(ParseTreeNode* parent, ParseTreeNode* child) {
    if (parent->childCount < 10) {
        parent->children[parent->childCount++] = child;
    } else {
        printf("Error: Too many children for node %s\n", parent->label);
    }
}

// Function to print the parse tree (preorder traversal)
void printParseTree(ParseTreeNode* node, int depth) {
    if (!node) return;

    // Indent based on depth
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s\n", node->label);

    // Recursively print children
    for (int i = 0; i < node->childCount; i++) {
        printParseTree(node->children[i], depth + 1);
    }
}

// Function to write the parse tree in parenthesized format to a file
void writeParseTreeToFile(ParseTreeNode* node, FILE* file) {
    if (!node) return;

    fprintf(file, "(%s", node->label);

    for (int i = 0; i < node->childCount; i++) {
        writeParseTreeToFile(node->children[i], file);
    }

    fprintf(file, ")");
}

// Function to free the parse tree
void freeParseTree(ParseTreeNode* node) {
    if (!node) return;
    for (int i = 0; i < node->childCount; i++) {
        freeParseTree(node->children[i]);
    }
    free(node);
}
