#include "TypeCheck.h"
#include "stdio.h"
#include "stdlib.h"
#include "errtype.h"


struct TypeItem_t *types = NULL;

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
        FieldListPtr pa = ta->info.structure.elem, pb = tb->info.structure.elem;
        for(;pa && pb;pa = pa->tail, pb = pb->tail){
            if(isTypeEqual(pa->type, pb->type)){ //is the type same as the pb ?
                continue;
            }else
                return 0;
        }
        if((!pa) && (!pb)){
            //num of pa field is same as pb
            return 1;
        }else 
            return 0;
    }
    return 0;
}

int tt_create(){
    typetable = (struct TypeItem_t**)malloc(TYPEBUCKET_SIZE*sizeof(struct TypeItem_t*));
    if(typetable == NULL){
        printf("create typetable failed\n");
        return 0;
    }
    nt_capacity = TYPEBUCKET_SIZE;
     //initialize the type table
    nt_bucket = 0;
    tt_push_bucket();
    TypePtr type_int = (TypePtr)malloc(sizeof(struct TypeItem_t)), type_float = (TypePtr)malloc(sizeof(struct TypeItem_t));
    type_int->level = type_float->level = 0;
    type_int->kind = type_float->kind = BASIC;
    type_int->name = "int"; type_float->name = "float";
    type_int->info.basic = BASIC_INT; type_float->info.basic = BASIC_FLOAT;
    add_global_type(type_int);
    add_global_type(type_float);
    return 1;
}

int tt_push_bucket(){
    //expand the capacity
    if(nt_bucket == nt_capacity){
        struct TypeItem_t** oldtt = typetable;
        typetable = (struct TypeItem_t**)malloc(2*nt_capacity*sizeof(struct TypeItem_t*));
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
    typetable[nt_bucket] = (struct TypeItem_t*)malloc(sizeof(struct TypeItem_t));
    typetable[nt_bucket]->next = NULL;
    typetable[nt_bucket]->name = "tail";
    nt_bucket++;
    return 1;
}

int tt_pop_bucket(){
    if(nt_bucket<1)
        return 0;
    while(typetable[nt_bucket-1]->next){
        if(tt_pop_type(nt_bucket - 1))
            continue;
        else
            break;
    }
    nt_bucket--;
    return 1;
}

int tt_push_type(int bucket_index, TypePtr type){
    if(bucket_index >= 0 && bucket_index < nt_bucket){
        type->next = typetable[bucket_index];
        typetable[bucket_index] = type;
        return 1;
    }else{
        printf("Err bucket_index\n");
        return 0;
    }
}

int tt_pop_type(int bucket_index){
    if(bucket_index >= 0 && bucket_index < nt_bucket){
        if(typetable[bucket_index]->next){
            TypePtr oldtype = typetable[bucket_index];
            typetable[bucket_index] = typetable[bucket_index]->next;
            free(oldtype);
            return 1;
        }
    }
    return 0;
}

int add_type(int level, TypePtr type, int lineno){
    struct TypeItem_t* entry;
    HASH_FIND_STR(types,type->name,entry);
    if(entry){
        if(entry->kind == BASIC)
            return 1;
        if(entry->kind == ARRAY && (entry->level == level)){
            return 1;
        }
        if((entry->kind == STRUCTURE) && (entry->info.structure.define == 1) && (entry->level == level)){
            printf("Error type %d at line %d: Redefined structure \"%s\"\n",REDEFINED_STRUCT,lineno,type->name);
            return 0;
        }
    }
    //add a new entry
   // entry = (struct TypeItem_t*)malloc(sizeof(struct TypeItem_t));
    if(type->kind == ARRAY){
        //memmove(entry,type,sizeof(struct TypeItem_t));
        //entry->level = level;
        HASH_ADD_KEYPTR(hh,types,type->name,strlen(type->name),type);
        tt_push_type(nt_bucket-1,type);
    }
    if(type->kind == STRUCTURE){
        //entry->level = level;
        //memmove(entry,type,sizeof(struct TypeItem_t));
        HASH_ADD_KEYPTR(hh,types,type->name,strlen(type->name),type);
        tt_push_type(nt_bucket-1,type);
    }
    return 1;
}


int add_global_type(TypePtr type){
    return tt_push_type(0,type);
}

int find_type(TypePtr type, TypePtr ret_type){
    HASH_FIND_STR(types,type->name,ret_type);
    if(ret_type){
        if(isTypeEqual(type,ret_type))
            return 1;
        else 
            return 0;
    }
    return 0;
}
