#ifndef __SYMBOLTABLE_H_
#define __SYMBOLTABLE_H_
#include "uthash/include/uthash.h"
#include "TypeCheck.h"

struct FuncTable_t{
    TypePtr ret_type;
    char* name;
    int n_param;
    FieldListPtr param_list;
    int line;
    UT_hash_handle hh;
};

struct VarTable_t{
    TypePtr type;
    char* name;     //key
    int line;
    UT_hash_handle hh;
};

struct FuncTable_t *functions;
struct VarTable_t * globalvars;
struct VarTable_t * localvars;

struct VarTableVector_t{
    struct VarTable_t **elem;
    int capacity;
    int size;
    int (*create)();
    int (*push_back)();
    int (*pop)();
    int (*find)();
};




void InitTable(void);


#endif
