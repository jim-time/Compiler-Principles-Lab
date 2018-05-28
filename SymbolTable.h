#ifndef __SYMBOLTABLE_H_
#define __SYMBOLTABLE_H_
#include "uthash/include/uthash.h"
#include "stdio.h"
#include "TypeCheck.h"
#include "my_vector.h"
#include "SyntaxTree.h"

struct FuncTable_t{
    TypePtr ret_type;
    char* name;     //key
    int n_param;
    FieldListPtr param_list;
    int define;
    UT_hash_handle hh;
};

struct VarTable_t{
    TypePtr type;
    char* name;     //key
    int level;
    struct VarTable_t* next;
    UT_hash_handle hh;
};

//level
extern int CompStLevel;

//symbol table (hash)
extern struct FuncTable_t *functions;
extern struct VarTable_t *vars;

//symbol table (stack)
extern vec_t VarTableStack;
int vartab_stack_create();
int vartab_stack_pop();
int vartab_stack_push();

int vartab_list_push(struct VarTable_t* start, struct VarTable_t* var);
int vartab_list_pop(struct VarTable_t* start);

int vartab_isEqual(struct VarTable_t** a, struct VarTable_t** b); //not define
//function table
int add_func(struct FuncTable_t* entry, int lineno);
int find_func(char* name, struct FuncTable_t* entry);
void print_functable();

//variable table
int add_var(int level, TypePtr type, char* name, int lineno);
int find_var(char* name, struct VarTable_t* entry);
int clear_local_var();
void print_vartable();

//syntax tree analyze
int ST_FuncDec(struct SyntaxTreeNode* specifier,struct SyntaxTreeNode* dec);
int ST_ExtVarDec(struct SyntaxTreeNode* Specifier, struct SyntaxTreeNode* ExtDecList, TypePtr ret);

#endif
