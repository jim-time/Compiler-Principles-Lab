#ifndef __TYPECHECK_H_
#define __TYPECHECK_H_

struct TypeItem;
struct FieldList;

typedef struct TypeItem* TypePtr;
typedef struct TypeItem** TypeListPtr;
typedef struct FieldList* FieldListPtr;

#define BASIC_INT 1
#define BASIC_FLOAT 2

struct TypeItem
{
    enum { BASIC, ARRAY, STRUCTURE } kind;
    union
    {
        // basic type
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { TypePtr elem; int size; } array;
        // 结构体类型信息是一个链表
        FieldListPtr structure;
    }info;
};

struct FieldList
{
    char* name; // 域的名字
    TypePtr type; // 域的类型
    FieldListPtr tail; // 下一个域
};


#define TYPEBUCKET_SIZE 10

struct TypeTable_t{
    TypePtr type;
    int nt_table;
    struct TypeTable_t *tail;
};
int nt_bucket;
int nt_capacity;
struct TypeTable_t **typetable;

int isTypeEqual(TypePtr ta, TypePtr tb);
int find_type(TypePtr type);
int tt_create();
int tt_push_bucket();
int tt_pop_bucket();
int tt_push_type(int bucket_index, TypePtr type);
int tt_pop_type(int bucket_index, TypePtr type);
int add_global_type(TypePtr type);

#endif
