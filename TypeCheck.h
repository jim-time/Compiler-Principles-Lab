#ifndef __TYPECHECK_H_
#define __TYPECHECK_H_
#include "uthash/include/uthash.h"
typedef struct TypeItem_t TypeTable_t;
struct FieldList;

typedef struct TypeItem_t* TypePtr;
typedef struct TypeItem_t** TypeListPtr;
typedef struct FieldList* FieldListPtr;

#define BASIC_INT 1
#define BASIC_FLOAT 2

struct TypeItem_t
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    char* name;
    union
    {
        // basic info = BASIC_INT | BASIC_FLOAT
        int basic;
        // array info
        struct { TypePtr elem; int size; } array;
        // structure info
        struct {FieldListPtr elem; int define;}structure;
    }info;
    int level;
    struct TypeItem_t *next;
    UT_hash_handle hh;
};

struct FieldList
{
    char* name; // 域的名字
    TypePtr type; // 域的类型
    FieldListPtr tail; // 下一个域
};

//hash table
extern struct TypeItem_t *types;

//typetable stack
#define TYPEBUCKET_SIZE 10
int nt_bucket;
int nt_capacity;
TypeTable_t **typetable;

int tt_create();
int tt_push_bucket();
int tt_pop_bucket();

int tt_push_type(int bucket_index, TypePtr type);
int tt_pop_type(int bucket_index);

int add_type(int level, TypePtr type, int lineno);
int add_global_type(TypePtr type);
int isTypeEqual(TypePtr ta, TypePtr tb);
int find_type(TypePtr type, TypePtr ret_type);

#endif
