#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef union moreMsg {
    int i;
    float f;
    void* p;
}MoreMsg;

typedef struct treeNode {
    char* val;
    char* type;
    int level;
    struct treeNode* parent;
    struct treeNode* child;
    struct treeNode* right;
    MoreMsg msg1;
    MoreMsg msg2;
    MoreMsg msg3;
    MoreMsg msg4;
}* TreeNode;

typedef struct stackNode {
    TreeNode node;
    struct stackNode* next;
}* StackNode;

TreeNode makeNode(char* val, char* type);

void connectParentNode(TreeNode child, TreeNode parent);

void connectLeftNode(TreeNode node, TreeNode left);

void adjustLevel(TreeNode root);

void printTree(TreeNode root);

void freeTree(TreeNode root);

StackNode pop(StackNode top, TreeNode* node);

StackNode push(StackNode top, TreeNode node);