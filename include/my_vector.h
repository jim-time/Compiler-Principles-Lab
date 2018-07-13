#include "SymbolTable.h"
#ifndef __MY_VECTOR_H_
#define __MY_VECTOR_H_
#include "stdio.h"

//Variable Table Stack
#define DEFAULT_CAPACITY 10
typedef struct Vector_t vec_t;
typedef struct VarTable_t* elem_type;
typedef elem_type* elem_ptr;

struct Vector_t{
    elem_ptr elem;
    int capacity;
    int size;
    int (*create)(vec_t *vec);
    int (*destruct)(vec_t *vec);
    int (*expand)(vec_t *vec);
    int (*push_back)(vec_t *vec, elem_type * e);
    int (*pop)(vec_t *vec, elem_type * e);
    int (*find)(vec_t *vec,elem_type * e, elem_type* ret_e);
    int (*isEqual)(elem_type *a, elem_type *b);
};

int vec_create(vec_t *vec);
int vec_destruct(vec_t *vec);
int vec_expand(vec_t *vec);
int vec_push_back(vec_t *vec, elem_type* e);
int vec_pop(vec_t *vec, elem_type* e);
int vec_find(vec_t *vec, elem_type* e, elem_type* ret_e);
//int vec_isEqual(elem_type* a, elem_type* b);

#endif
