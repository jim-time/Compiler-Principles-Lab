#ifndef _SYNTAXTREE_H_
#define _SYNTAXTREE_H_
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define MAX_CHILDREN 10

union Node_Data{
    int int_value;
    float float_value;
    char* string_value;
};

//typedef struct SyntaxTree_Node SyntaxTree_Node;
struct SyntaxTreeNode{
    char* node_name;
    union Node_Data data;
    int lineno;
    struct SyntaxTreeNode* parent;
    struct SyntaxTreeNode* children[MAX_CHILDREN];
    int n_children;
    /*to update the height of tree*/
    int height;
    int max_h_child;
};

struct SyntaxTreeNode* CreateNode(struct SyntaxTreeNode* parent,char* name,union Node_Data data,int line);
struct SyntaxTreeNode* InsertNode(struct SyntaxTreeNode* parent,struct SyntaxTreeNode* child);
int DestructTree(struct SyntaxTreeNode* root);
void PreOrderTraverse(struct SyntaxTreeNode* node,int level);

#endif
