#include "../include/my_vector.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"


int vec_create(vec_t *vec){
    if(vec == NULL){
        printf("The vector is NULL!\n");
        return 0;
    }
    vec->elem = (elem_type *)malloc(DEFAULT_CAPACITY*sizeof(elem_type));
    if(vec->elem == NULL){
        printf("Create vector failed\n");
        return 0;
    }
    vec->capacity = DEFAULT_CAPACITY;
    vec->size = 0;
    return 1;
}

int vec_destruct(vec_t *vec){
    if(vec == NULL){
        printf("The vector is NULL!\n");
        return 0;
    }
    free(vec->elem);
    return 1;
}

int vec_expand(vec_t *vec){
    if(vec->size == vec->capacity){
        int iter;
        elem_type* elem_old = vec->elem;
        vec->elem = (elem_type *)malloc(2*vec->capacity*sizeof(elem_type));
        for(iter = 0; iter < vec->capacity; iter++)
            vec->elem[iter] = elem_old[iter];
        vec->capacity <<=1;
        free(elem_old);
    }else
        return 0;
    return 1;
}

int vec_push_back(vec_t *vec, elem_type * e){
    vec->expand(vec);
    vec->elem[vec->size++] = *e;
    return 1;
}

int vec_pop(vec_t *vec, elem_type * e){
    if(vec->size < 1){
        printf("the size is zero\n");
        return 0;
    }else
        *e = vec->elem[vec->size--];
    return 1;
}

int vec_find(vec_t *vec,elem_type * e, elem_type* ret_e){
    int iter;
    if(vec->size < 1){
        printf("vector size is zero\n");
        return 0;
    }
    for(iter = vec->size-1; iter >= 0; iter--){
        if(vec->isEqual(vec->elem + iter,e)){
            ret_e = vec->elem + iter;
            return 1;
        }
    }
    ret_e = NULL;
    return 0;
}