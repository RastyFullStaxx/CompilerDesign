#include "parse_tree.h"

// Function to create a new parse tree node
ParseTreeNode* createParseTreeNode(const char* type, const char* value) {
    ParseTreeNode* node = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    if (!node) {
        fprintf(stderr, "[ERROR] Memory allocation failed for ParseTreeNode\n");
        exit(EXIT_FAILURE);
    }
    if (type) {
        strncpy(node->label, type, sizeof(node->label) - 1);
        node->label[sizeof(node->label) - 1] = '\0'; // Null-terminate
    } else {
        node->label[0] = '\0'; // Initialize as empty
    }

    if (value) {
        strncpy(node->value, value, sizeof(node->value) - 1);
        node->value[sizeof(node->value) - 1] = '\0'; // Null-terminate
    } else {
        node->value[0] = '\0'; // Initialize as empty
    }

    node->childCount = 0;
    return node;
}

// Function to add a child to a parse tree node
void addChild(ParseTreeNode* parent, ParseTreeNode* child) {
    if (!parent || !child) {
        printf("[ERROR] Invalid parent or child node. Cannot add child.\n");
        return; // Exit the function
    }

    if (parent->childCount < MAX_CHILDREN) {
        parent->children[parent->childCount++] = child;

        // Debug: Log successful addition of child
        printf("[DEBUG] Added child to parent.\n");
        printf("[DEBUG] Parent Node: Label='%s', Value='%s', Current Children=%d\n",
               parent->label, parent->value, parent->childCount);
        printf("[DEBUG] Child Node: Label='%s', Value='%s'\n", 
               child->label, child->value);

    } else {
        printf("[ERROR] Too many children for node %s (childCount=%d)\n",
               parent->label, parent->childCount);
    }
}



// Function to print the parse tree (preorder traversal)
void printParseTree(ParseTreeNode* node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; i++) printf("  ");
    if (node->value[0] != '\0') {
        printf("%s: %s\n", node->label, node->value);
    } else {
        printf("%s\n", node->label);
    }

    for (int i = 0; i < node->childCount; i++) {
        printParseTree(node->children[i], depth + 1);
    }
}

// Function to write the parse tree in parenthesized format to a file
void writeParseTreeToFile(ParseTreeNode* node, FILE* file) {
    if (!node) return;

    fprintf(file, "(%s", node->label); // Print the node's label

    if (node->value[0] != '\0') {
        fprintf(file, ":%s", node->value); // Include value if present
    }

    for (int i = 0; i < node->childCount; i++) {
        writeParseTreeToFile(node->children[i], file); // Recursive call for each child
    }

    fprintf(file, ")"); // Close parentheses for this node
}

// Function to free the parse tree
void freeParseTree(ParseTreeNode* node) {
    if (!node) return;

    for (int i = 0; i < node->childCount; i++) {
        freeParseTree(node->children[i]); // Recursively free children
    }

    free(node); // Free the current node
}

// Function to set the value of a parse tree node
void setNodeValue(ParseTreeNode* node, const char* value) {
    if (!node) {
        fprintf(stderr, "[ERROR] Cannot set value on a NULL node\n");
        return;
    }
    if (value) {
        strncpy(node->value, value, sizeof(node->value) - 1);
        node->value[sizeof(node->value) - 1] = '\0'; // Null-terminate
    } else {
        node->value[0] = '\0'; // Initialize as empty string
    }
}
