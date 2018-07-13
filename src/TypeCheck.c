#include "../include/main.h"
#include "stdio.h"
#include "stdlib.h"


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

int isFieldEqual(FieldListPtr fla,FieldListPtr flb){
    for(;fla && flb;fla = fla->tail, flb = flb->tail){
        if(isTypeEqual(fla->type, flb->type)){
            continue;
        }else
            return 0;
    }
    if(!fla && !flb)
        return 1;
    else                    // the num of field of fla is not same as the flb
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
    add_type(0,type_int,0);
    add_type(0,type_float,0);
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
    while(typetable[nt_bucket-1]){
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
            HASH_DEL(types,oldtype);
            typetable[bucket_index] = typetable[bucket_index]->next;
            free(oldtype);
            return 1;
        }
    }
    return 0;
}

TypePtr add_type(int level, TypePtr type, int lineno){
    struct TypeItem_t* entry;
    HASH_FIND_STR(types,type->name,entry);    
    if(entry){
        if(entry->kind == BASIC)
            return entry;
        if(entry->kind == ARRAY && (entry->level == level)){
            return entry;
        }
        if(entry->kind == STRUCTURE){
            if(entry->level == level){
                if(entry->info.structure.define & TYPE_DECLARED){
                    if(type->info.structure.define & TYPE_DECLARED){
                        return entry;
                    }else if(type->info.structure.define & TYPE_DEFINED){
                        type->info.structure.define |=TYPE_DECLARED;
                        type->level = level;
                        type->next = entry->next;
                        type->hh = entry->hh;
                        memmove(entry,type,sizeof(struct TypeItem_t));
                        return entry;
                    }
                }else if(entry->info.structure.define & TYPE_DEFINED){
                    if(type->info.structure.define & TYPE_DECLARED){
                        entry->info.structure.define |=TYPE_DECLARED;
                        return entry;
                    }else if(type->info.structure.define & TYPE_DEFINED){
                        printf("Error type %d at line %d: Redefined structure \"%s\"\n",REDEFINED_STRUCT,lineno,type->name);
                        return entry;
                    }
                }
            }
        }
    }
    //add a new entry
    if(type->kind == BASIC){
        HASH_ADD_KEYPTR(hh,types,type->name,strlen(type->name),type);
        tt_push_type(nt_bucket-1,type);
    }else if(type->kind == ARRAY){
        HASH_ADD_KEYPTR(hh,types,type->name,strlen(type->name),type);
        tt_push_type(nt_bucket-1,type);
    }else if(type->kind == STRUCTURE){
        //entry->level = level;
        //memmove(entry,type,sizeof(struct TypeItem_t));
        HASH_ADD_KEYPTR(hh,types,type->name,strlen(type->name),type);
        tt_push_type(nt_bucket-1,type);
    }
    return type;
}


int add_global_type(TypePtr type){
    return tt_push_type(0,type);
}

int find_type(TypePtr type, TypePtr* ret_type){
    HASH_FIND_STR(types,type->name,*ret_type);
    if(*ret_type){
        if(isTypeEqual(type,*ret_type))
            return 1;
        else 
            return 0;
    }
    return 0;
}

//print info
int print_type(TypePtr type, int level){
    if(type && type->name){
        if(type->kind == BASIC){
            printf("%s\n",type->name);
        }else if(type->kind == ARRAY){
            printf("%s\n",type->name);
        }else{
            printf("%s {\n",type->name);
            if(type->info.structure.elem)
                print_field(type->info.structure.elem,level);
            printf("}\n");
        }
        return 1;
    }
    return 0;
}

int print_field(FieldListPtr field, int level){
    if(field){
        //print_type(field->type);
        if(field->type && field->type->name){
            int iter;
            for(iter = 0; iter <= level;iter++)
                printf("\t");
            if(field->type->kind == BASIC){
                printf("%s ",field->type->name);
            }else if(field->type->kind == ARRAY){
                printf("%s ",field->type->name);
            }else{
                printf("%s {\n",field->type->name);
                if(field->type->info.structure.elem)
                    print_field(field->type->info.structure.elem,level+1);
                for(iter = 0; iter <= level;iter++)
                    printf("\t");
                printf("}");
            }
        }
        if(field->name)
            printf("%s;\n",field->name);
        print_field(field->tail,level);
        return 1;
    }
    return 0;
}

int print_typetable(){
    TypePtr ptype;
    printf("\n");
    int l = nt_bucket - 1;
    //for(l = 0; l < nt_bucket;l++){
        if(l)
            printf("Local Type %d\n",l);
        else
            printf("Global Type\n");
        for(ptype = typetable[l];ptype->next;ptype = ptype->next){
            print_type(ptype,0);
        }
   // }
    return 1;
}