#ifndef __SYMBOLTABLE_H_
#define __SYMBOLTABLE_H_
#include "uthash/include/uthash.h"
#include "stdio.h"
#include "TypeCheck.h"
#include "my_vector.h"
#include "SyntaxTree.h"

#define FUNC_DECLARED 0x1
#define FUNC_DEFINED 0x2

typedef struct FuncTable_t* FuncTablePtr;
typedef struct VarTable_t* VarTablePtr;
typedef struct ExpVal_t* ExpValPtr;

struct FuncTable_t{
    TypePtr ret_type;
    char* name;     //key
    int n_param;
    FieldListPtr param_list;
    int lineno;
    int define;
    UT_hash_handle hh;
};

struct VarTable_t{
    TypePtr type;
    char* name;     //key
    void* val_ptr;
    int level;
    struct VarTable_t* next;
    UT_hash_handle hh;
};

struct ExpVal_t{
    VarTablePtr var;
    ExpValPtr next;
};
//level
extern uint16_t CompStLevel;

//is func equal 
int isFuncEqual(struct FuncTable_t* fa, struct FuncTable_t* fb);

//symbol table (hash)
extern struct FuncTable_t *functions;
extern struct VarTable_t *vars;

//symbol table (stack)
extern vec_t VarTableStack;
int vartab_stack_create();
int vartab_stack_pop();
int vartab_stack_push();

int vartab_list_push(struct VarTable_t** start, struct VarTable_t** var);
int vartab_list_pop(struct VarTable_t** start);

int vartab_isEqual(struct VarTable_t** a, struct VarTable_t** b); //not define
//function table
int add_func(struct FuncTable_t* entry, int lineno);
int find_func(char* name, struct FuncTable_t** entry);
void print_functable();
void check_func_def();
void print_param(FieldListPtr paramlist);

//variable table
int add_var(int level, TypePtr type, char* name, int lineno);
int find_var(char* name, struct VarTable_t** entry);
int clear_local_var();
void print_vartable();

#endif
