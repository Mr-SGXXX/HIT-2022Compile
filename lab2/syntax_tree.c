#include "syntax_tree.h"

TreeNode makeNode(char* val, char* type){
    TreeNode node = (TreeNode)calloc(1, sizeof(struct treeNode));
    char* valBuffer = (char*)calloc(64, sizeof(char));
    char* typeBuffer = (char*)calloc(64, sizeof(char));
    if (node == NULL || valBuffer == NULL || typeBuffer == NULL) {
        printf("MEMORY ERROR");
        return NULL;
    }
    strcpy(valBuffer, val);
    if (type == NULL) {
        free(typeBuffer);
        typeBuffer = NULL;
    }
    else strcpy(typeBuffer, type);
    node -> level = 0;
    node -> val = valBuffer;
    node -> type = typeBuffer;
    node -> parent = NULL;
    node -> child = NULL;
    node -> right = NULL;
    return node;
}

void connectParentNode(TreeNode child, TreeNode parent) {
    parent -> child = child;
    while(child != NULL) {
        child -> parent = parent;
        adjustLevel(child);
        child = child -> right;
    }
}

void connectLeftNode(TreeNode node, TreeNode left) {
    while (left -> right != NULL) left = left -> right;
    left -> right = node;
    node -> level = left -> level;
    node -> parent = left -> parent;
}

void adjustLevel(TreeNode root) {
    TreeNode child = root -> child;
    root -> level++;
    while(child != NULL) {
        adjustLevel(child);
        child = child -> right;
    }
}

void printTree(TreeNode root) {
    TreeNode child = root -> child;
    if (root -> type != NULL) {
        for (int i = 0; i < root -> level; i++) printf("  ");
        printf("%s%s\n", root -> type, root -> val);
    }
    while(child != NULL) {
        printTree(child);
        child = child -> right;
    }
}

void freeTree(TreeNode root) {
    if (root == NULL) return;
    TreeNode child = root -> child, temp;
    while(child != NULL)
    {
        temp = child -> right;
        freeTree(child);
        child = temp;
    }
    free(root -> val);
    free(root -> type);
    free(root);
}

StackNode pop(StackNode top, TreeNode* node) {
    StackNode tmp;
    if (top == NULL) return NULL;
    *node = top -> node;
    tmp = top -> next;
    free(top);
    return tmp;
}

StackNode push(StackNode top, TreeNode node) {
    StackNode tmp = (StackNode)calloc(1, sizeof(struct stackNode));
    if (tmp == NULL) {
        printf("MEMORY ERROR");
        return top;
    }
    tmp -> node = node;
    tmp -> next = top;
    return tmp;
}