#include "TypeCheck.h"
#include "stdio.h"
#include "stdlib.h"

int isTypeEqual(TypePtr ta, TypePtr tb){
    //is float or int ?
    if(ta->kind == BASIC && tb->kind == BASIC){
        if(ta->info.basic == tb->info.basic){
            return 1;
        }
        return 0;
    }
    //is array ?
    if(ta->kind == ARRAY && tb->kind == ARRAY){
        if(ta->info.array.size == tb->info.array.size){
            if(ta->info.array.elem && tb->info.array.elem)
                return isTypeEqual(ta->info.array.elem,tb->info.array.elem);
            else{
                printf("Error : the type of array elem is NULL!\n");
                return 0;
            }
        }else
            return 0;
       
    }
    //is structure ?
    if(ta->kind == STRUCTURE && tb->kind == STRUCTURE){
        FieldListPtr pa = ta->info.structure, pb = tb->info.structure;
        for(;pa && pb;pa = pa->tail, pb = pb->tail){
            if(isTypeEqual(pa->type, pb->type)){ //is the type same as the pb ?
                continue;
            }else
                return 0;
        }
        if((!pa->tail) && (!pb->tail)){
            //num of pa field is same as pb
            return 1;
        }else 
            return 0;
    }
    return 0;
}

int tt_create(){
    typetable = (struct TypeTable_t**)malloc(TYPEBUCKET_SIZE*sizeof(struct TypeTable_t*));
    if(typetable == NULL){
        printf("create typetable failed\n");
        return 0;
    }
    nt_capacity = TYPEBUCKET_SIZE;
     //initialize the type table
    nt_bucket = 0;
    tt_push_bucket();
    TypePtr type;
    type->kind = BASIC;
    type->info.basic = BASIC_INT;
    add_global_type(type);
    type->info.basic = BASIC_FLOAT;
    add_global_type(type);
    return 1;
}

int tt_push_bucket(){
    //expand the capacity
    if(nt_bucket == nt_capacity){
        struct TypeTable_t** oldtt = typetable;
        typetable = (struct TypeTable_t**)malloc(2*nt_capacity*sizeof(struct TypeTable_t*));
        if(typetable == NULL){
            printf("Expand the typetable failed\n");
            return 0;
        }
        int iter;
        for(iter = 0; iter < nt_capacity;iter++){
            typetable[iter] = oldtt[iter];
        }
        nt_capacity <<=1;
        free(oldtt);
    }
    typetable[nt_bucket] = (struct TypeTable_t*)malloc(sizeof(struct TypeTable_t));
    typetable[nt_bucket]->type = NULL;
    typetable[nt_bucket]->tail = NULL;
    typetable[nt_bucket]->nt_table = 0;
    nt_bucket++;
    return 1;
}

int tt_pop_bucket(){
    if(nt_bucket<1)
        return 0;
    struct TypeTable_t* tt = typetable[nt_bucket-1];
    TypePtr type;
    for(;tt->tail;tt = tt->tail){
        if(tt_pop_type(nt_bucket - 1,type))
            continue;
        else
            break;
    }
    nt_bucket--;
    return 1;
}

int tt_push_type(int bucket_index, TypePtr type){
    struct TypeTable_t* tt = (struct TypeTable_t*)malloc(sizeof(struct TypeTable_t));
    if(tt == NULL){
        printf("Push a type failed\n");
        return 0;
    }
    tt->type = type;
    tt->tail = typetable[bucket_index];
    tt->nt_table = typetable[bucket_index]->nt_table + 1;
    typetable[bucket_index] = tt;
    return 1;
}

int tt_pop_type(int bucket_index, TypePtr type){
    struct TypeTable_t* tt = typetable[bucket_index];
    type = typetable[bucket_index]->type;

    if(typetable[bucket_index]->tail)
        typetable[bucket_index] = typetable[bucket_index]->tail;
    else
        return 0;
    free(tt);
    return 1;
}

int add_global_type(TypePtr type){
    tt_push_type(0,type);
    return 1;
}

int find_type(TypePtr type){
    int i,j;
    for(i = nt_bucket -1;i >=0; i--){
        for(j = typetable[i]->nt_table; j>0 ;j--){
            if(isTypeEqual(typetable[i]->type,type))
                return 1;
        }
    }
    return 0;
}
