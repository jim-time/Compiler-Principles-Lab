#include "SymbolTable.h"
#include "TypeCheck.h"
#include "SyntaxTree.h"
#include "errtype.h"
#include "my_vector.h"

//level
uint16_t CompStLevel = 0;

/**
 *  @file   SymbolTable.c
 *  @brief  Symbol Table(hash) Header
 *  @author jim
 *  @date   2018-5-29
 *  @version: v1.0
**/ 

struct FuncTable_t *functions = NULL;
struct VarTable_t *vars = NULL;
/**
 *  @file   SymbolTable.c
 *  @brief  Symbol Table(Stack) Operation
 *  @author jim
 *  @date   2018-5-29
 *  @version: v1.0
**/ 
vec_t VarTableStack = {
    NULL,
    DEFAULT_CAPACITY,
    0,
    vec_create,
    vec_destruct,
    vec_expand,
    vec_push_back,
    vec_pop,
    vec_find,
    vartab_isEqual
};

int vartab_stack_create(){
    VarTableStack.create(&VarTableStack);
    vartab_stack_push();
    return 1;
}

int vartab_stack_push(){
    struct VarTable_t* bucket = (struct VarTable_t*)malloc(sizeof(struct VarTable_t));
    bucket->name = "tail";
    bucket->next = NULL;
    VarTableStack.push_back(&VarTableStack,&bucket);
    return 1;
}

int vartab_stack_pop(){
    struct VarTable_t* bucket, *discard;
    VarTableStack.pop(&VarTableStack,&bucket);
    while(bucket){
        vartab_list_pop(&bucket);
    }
    return 1;
}

int vartab_isEqual(struct VarTable_t** a, struct VarTable_t** b){
    //no use
    return 1;
}

int vartab_list_push(struct VarTable_t** start, struct VarTable_t** var){
    (*var)->next = *start;
    *start = *var;
    return 1;
}

int vartab_list_pop(struct VarTable_t** start){
    if(!start){
        printf("start is null\n");
        return 0;
    }

    struct VarTable_t* old = *start;
    //HASH_DEL(types,old);
    *start = (*start)->next;
    free(old);
    return 1;
}


/**
 *  @file   SymbolTable.c
 *  @brief  Function Table Operation
 *  @author jim
 *  @date   2018-5-29
 *  @version: v1.0
**/ 

int isFuncEqual(struct FuncTable_t* fa, struct FuncTable_t* fb){
    if(fa->n_param == fb->n_param){
        if(isTypeEqual(fa->ret_type,fb->ret_type)){             //ret_type
            if(isFieldEqual(fa->param_list,fb->param_list))     //para_type
                return 1;
        }
    }
    return 0;
}

int add_func(struct FuncTable_t* entry, int lineno){
    struct FuncTable_t* func;
    HASH_FIND_STR(functions,entry->name,func);
    if(func){
        if(func->define & FUNC_DECLARED){
           if(entry->define & FUNC_DEFINED){
               if(!isFuncEqual(func,entry)){
                   printf("Error type %d at line %d: Confilcting types for \"%s\"\n",CONFLIT_FUNC,lineno,entry->name);
                   //return func;
                   return 0;
               }
               entry->define |=FUNC_DECLARED;
               entry->hh = func->hh;
               memmove(func,entry,sizeof(struct FuncTable_t));
               //return func;
               return 1;
           }else if(entry->define & FUNC_DECLARED){
                if(!isFuncEqual(entry,func))
                    printf("Error type %d at line %d: Inconsistent declaration of function \"%s\"\n",INCOSISTENT_FUNC_DEC,lineno,entry->name);
                //return func;
                return 0;
           }
        }else if(func->define & FUNC_DEFINED){
            if(entry->define & FUNC_DECLARED){          // defined a func and then use it
                if(!isFuncEqual(func,entry)){
                    printf("Error type %d at line %d: Confilcting types for \"%s\"\n",CONFLIT_FUNC,lineno,entry->name);
                    //return func;
                    return 0;
                }
                func->define |=FUNC_DECLARED;
                //return func;
                return 1;
            }else if(entry->define & FUNC_DEFINED){
                printf("Error type %d at line %d: Redefined function \"%s\"\n",REDEFINED_FUNC,lineno,entry->name);
                //return func;
                return 1;
            }
        }
    }
    //add a new func
    if(!func)
        HASH_ADD_KEYPTR(hh,functions,entry->name,strlen(entry->name),entry);
    //return entry;
    return 1;
}

int find_func(char* name, struct FuncTable_t** entry){
    HASH_FIND_STR(functions,name,*entry);
    return 1;
}

void check_func_def(){
    struct FuncTable_t* func;
    for(func = functions; func != NULL; func = (struct FuncTable_t*)(func->hh.next)){
        if(func->define & FUNC_DEFINED)
            continue;
        printf("Error type %d at line %d: Undefined Function \"%s\"\n",UNDEFINED_FUNC,func->lineno,func->name);
    }
}

void print_functable() {
    struct FuncTable_t* func;
    FieldListPtr para_ptr;
    printf("\n%-24s%-24s%-24s\n","RetType", "Name","Param");
    for(func = functions; func != NULL; func = (struct FuncTable_t*)(func->hh.next)) {
        if(func->ret_type->name){
            printf("%-24s",func->ret_type->name);
        }
        if(func->name){
            printf("%-24s",func->name);
        }
        printf("(");
        for(para_ptr = func->param_list;para_ptr;para_ptr = para_ptr->tail){
            if(para_ptr->type->name){
                printf("%s ",para_ptr->type->name);
            }
            if(para_ptr->name){
                if(para_ptr->tail)
                    printf("%s,",para_ptr->name);
                else
                    printf("%s",para_ptr->name);
            }
        }
        printf(")\n");
    }
    return;
}

void print_param(FieldListPtr paramlist){
    FieldListPtr pParam = paramlist;
    for(;pParam;pParam = pParam->tail){
        if(pParam->tail)
            printf("%s, ",pParam->type->name);
        else
            printf("%s",pParam->type->name);
    }
}

/**
 *  @file   SymbolTable.c
 *  @brief  Variable Table Operation
 *  @author jim
 *  @date   2018-5-29
 *  @version: v1.0
**/ 
int add_var(int level, TypePtr type, char* name, int lineno){
    struct VarTable_t* entry;
    HASH_FIND_STR(vars,name,entry);
    if(entry && (entry->level == level)){
        printf("Error type %d at line %d: Redefined variable \"%s\"\n",REDEFINED_VAR,lineno,name);
        return 0;
    }

    //add a new entry to hash table
    entry = (struct VarTable_t*)malloc(sizeof(struct VarTable_t));
    entry->type = type;
    entry->level = level;
    entry->name = name;
    entry->val_ptr = NULL;
    HASH_ADD_KEYPTR(hh,vars,entry->name,strlen(entry->name),entry);

    vartab_list_push(&VarTableStack.elem[VarTableStack.size-1],&entry);
    return 1;
}

int find_var(char* name, struct VarTable_t** entry){
    //entry->name = name;
    HASH_FIND_STR(vars,name,*entry);
    return 1;
}

int clear_local_var(){
    if(VarTableStack.size <= 1){
        printf("No local var\n");
        return 0;
    }
    struct VarTable_t* p = VarTableStack.elem[VarTableStack.size-1], *oldp;
    for(;p->next;){
        oldp = p;
        HASH_DEL(vars,p);
        p = p->next;
        free(oldp);
    }
    VarTableStack.elem[VarTableStack.size-1] = p;
    vec_pop(&VarTableStack, &p);
    return 1;
}

void print_vartable(){
    struct VarTable_t* var;
    printf("\n%-24s%-24s%s\n","Type", "Name","Level");
    for(var = VarTableStack.elem[VarTableStack.size-1];var->next;var = var->next){
        if(var->type->name){
            printf("%-24s",var->type->name);
        }
        if(var->name){
            printf("%-24s",var->name);
        }
        if(VarTableStack.size-1)
            printf("Local  %d\n",VarTableStack.size-1);
        else
            printf("Global\n");
    }
}
