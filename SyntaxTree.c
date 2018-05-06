#include "SyntaxTree.h"
struct SyntaxTreeNode* CreateNode(struct SyntaxTreeNode* parent,char* name,union Node_Data data,int line){
    struct SyntaxTreeNode* child = (struct SyntaxTreeNode*)malloc(sizeof(struct SyntaxTreeNode));
    if(child == NULL){
        printf("Fail to create a tree node!\n");
        return 0;
    }

    child->parent = parent;
    child->n_children = 0;
    memset(child->children,0,MAX_CHILDREN);

    child->node_name = name;
    child->data = data;
    child->lineno = line;
    child->height = 0;
    return child;
}
struct SyntaxTreeNode* InsertNode(struct SyntaxTreeNode* parent,struct SyntaxTreeNode* child){
   //int iter;
    if(parent == NULL){
        printf("The parent node is NULL!\n");
        return 0;
    }
    if(child != NULL){
        /*update the parent node*/
        parent->children[parent->n_children] = child;
        parent->n_children++;
        if(parent->lineno > 0){
            if(parent->lineno > child->lineno)
                parent->lineno = child->lineno;
        }else   
            parent->lineno = child->lineno;
        /*update the height*/        
        /*update the child node*/
        child->parent = parent;
    }
    return child;
}

int DestructTree(struct SyntaxTreeNode* root){
    int iter;
    for(iter = 0;iter <root->n_children;iter++){
        if(root->children[iter] != NULL)
            DestructTree(root->children[iter]);
    }
    free(root);
    return 0;
}

void PreOrderTraverse(struct SyntaxTreeNode* node,int level){
    int iter,cnt_child;

    for(iter = 0; iter < level; iter++){
        printf(" ");
    }
    if(!strcmp(node->node_name,"INT")){
        printf("INT: %d\n",node->data.int_value);
    }else if(!strcmp(node->node_name,"FLOAT")){
        printf("FLOAT: %.6f\n",node->data.float_value);
    }else if(!strcmp(node->node_name,"TYPE")){
        printf("TYPE: %s\n",node->data.string_value);
    }else if(!strcmp(node->node_name,"ID")){
        printf("ID: %s\n",node->data.string_value);
    }else{
        if(node->n_children == 0)
            printf("%s\n",node->node_name);
        else
            printf("%s (%d)\n",node->node_name,node->lineno);
    }
    for(cnt_child = 0; cnt_child < node->n_children;cnt_child++){
        PreOrderTraverse(node->children[cnt_child],level+1);
    }
}