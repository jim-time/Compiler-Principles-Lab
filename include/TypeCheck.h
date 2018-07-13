#ifndef __TYPECHECK_H_
#define __TYPECHECK_H_
#include "uthash.h"

// external definition
// SymbolTabel.h
struct FuncTable_t;
struct VarTable_t;
typedef struct FuncTable_t* FuncTablePtr;
typedef struct VarTable_t* VarTablePtr;

// IntermediateCode.h
struct Operand_t;
typedef struct Operand_t Operand;
typedef Operand* OperandPtr;

#define TYPE_DECLARED 0x1
#define TYPE_DEFINED  0x2 
#define TYPE_REFERENCE 0x4

typedef struct TypeItem_t TypeTable_t;
typedef TypeTable_t TypeTable;

typedef struct TypeItem_t* TypePtr;
typedef struct TypeItem_t** TypeListPtr;
typedef struct FieldList* FieldListPtr;

#define BASIC_INT 1
#define BASIC_FLOAT 2
#define TYPE_NAME_LEN 32
struct TypeItem_t
{
    enum { NOTYPE,BASIC, ARRAY, STRUCTURE } kind;
    char* name;
    union
    {
        // basic info = BASIC_INT || BASIC_FLOAT
        int basic;
        // array info
        struct { TypePtr elem; int index; int size; } array;
        // structure info
        struct {FieldListPtr elem; int define;int size;}structure;
    }info;
    union{
        struct { uint32_t hierarchy:16; uint32_t compst:16;};
        uint32_t level;
    };
    struct TypeItem_t *next;
    UT_hash_handle hh;
};

struct FieldList
{
    char* name; // name of the field
    OperandPtr alias;
    TypePtr type; // type of field
    void* val_ptr;  //value
    int lineno;
    FieldListPtr tail; // next field
};


//is Equal
int isTypeEqual(TypePtr ta, TypePtr tb);
int isFieldEqual(FieldListPtr fla,FieldListPtr flb);

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

TypePtr add_type(int level, TypePtr type, int lineno);
int add_global_type(TypePtr type);
int find_type(TypePtr type, TypePtr* ret_type);

//print info
int print_type(TypePtr type, int level);
int print_field(FieldListPtr field, int level);

int print_typetable();
#endif
