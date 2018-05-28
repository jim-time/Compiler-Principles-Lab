#include "SymbolTable.h"
#include "TypeCheck.h"
#include "SyntaxTree.h"
#include "errtype.h"
#include "my_vector.h"

//level
int CompStLevel = 0;

/** Symbol table(hash)
 *
 * 
**/ 

struct FuncTable_t *functions = NULL;
struct VarTable_t *vars = NULL;

/*Symbol table(stack)
 *
 * 
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
    while(bucket->next){
        vartab_list_pop(bucket);
    }
    return 1;
}

int vartab_isEqual(struct VarTable_t** a, struct VarTable_t** b){
    //no use
    return 1;
}

int vartab_list_push(struct VarTable_t* start, struct VarTable_t* var){
    var->next = start;
    start = var;
    return 1;
}

int vartab_list_pop(struct VarTable_t* start){
    if(!start){
        printf("start is null\n");
        return 0;
    }

    struct VarTable_t* old = start;
    start = start->next;
    free(old);
    return 1;
}


/**
 *function table
 * 
**/ 
int add_func(struct FuncTable_t* entry, int lineno){
    struct FuncTable_t* func;
    HASH_FIND_STR(functions,entry->name,func);
    if(func && func->define == 1){
        printf("Error type %d at line %d: Redefined function \"%s\"\n",REDEFINED_FUNC,lineno,entry->name);
        return 0;
    }
    //add a new func
    if(!func)
        HASH_ADD_KEYPTR(hh,functions,entry->name,strlen(entry->name),entry);
    else if(func->define == 0)
        func->define = 1;
    return 1;
}

int find_func(char* name, struct FuncTable_t* entry){
    HASH_FIND_STR(functions,name,entry);
    return 1;
}

void print_functable() {
    struct FuncTable_t* func;
    printf("\n%-15s%-15s%-15s\n","Name", "Ret","Param");
    for(func = functions; func != NULL; func = (struct FuncTable_t*)(func->hh.next)) {
        printf("%-15s%-15d\n", func->name, func->ret_type->kind);
    }
}

/**variable table
 *
 * 
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
    strcpy(entry->name,name);
    HASH_ADD_KEYPTR(hh,vars,entry->name,strlen(entry->name),entry);

    vartab_list_push(VarTableStack.elem[VarTableStack.size-1],entry);
    return 1;
}


int find_var(char* name, struct VarTable_t* entry){
    //entry->name = name;
    HASH_FIND_STR(vars,name,entry);
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
    printf("\n%-15s%-15s%-15s\n","Type", "Name","Level");
    for(var = vars; var != NULL; var = (struct VarTable_t*)(var->hh.next)) {
        printf("%-15d%-15s\n", var->type->kind, var->name);
    }
}

/*Syntax tree analize
 *
 * 
 *
**/  
int FuncDec(struct SyntaxTreeNode* specifier,struct SyntaxTreeNode* dec){
    int a;
    
    return 1;
}

int ST_ExtVarDec(struct SyntaxTreeNode* Specifier, struct SyntaxTreeNode* ExtDecList, TypePtr ret){
    TypePtr type = (TypePtr)malloc(sizeof(struct TypeItem_t));
    struct SyntaxTreeNode* st_iter;
    if(ExtDecList == NULL){                                 // ExtDef -> Specifier SEMI
        
    }else{                                                  // ExtDef -> Specifier ExtDecList SEMI

    }
    if(!strcmp(Specifier->children[0]->node_name,"TYPE")){   //int | float
        type->kind = BASIC;
        if(!strcmp(Specifier->data.string_value,"int")){ //int
            type->name = "int";
            type->info.basic = BASIC_INT;
        }else{
            type->name = "float";
            type->info.basic = BASIC_FLOAT;
        }
    }else{ //StructSpecifier
        //st_iter->n_children;
    }

    return 1;
}

int ST_Specifier(struct SyntaxTreeNode* Specifier,int hierarchy,TypePtr ret){
    TypePtr type;
    struct SyntaxTreeNode* st_iter = Specifier;
    st_iter = st_iter->children[0];                     // Specifier -> TYPE | StructSpecifier
    if(!strcmp(st_iter->node_name,"TYPE")){             // Specifier -> TYPE , st_iter = TYPE
        type = (TypePtr)malloc(sizeof(struct TypeItem_t));
        strcpy(type->name,st_iter->data.string_value);
        find_type(type,ret);
        free(type);
        return 1;
    }else{                                               // Specifier -> StructSpecifier , st_iter = StructSpecifier
        if(st_iter->n_children == 2){                    // StructSpecifier -> STRUCT Tag , declare a structure
            st_iter = st_iter->children[1];              // st_iter = Tag -> ID
            st_iter = st_iter->children[0];              // st_iter = ID
            type = (TypePtr)malloc(sizeof(struct TypeItem_t));
            type->kind = STRUCTURE;
            sprintf(type->name,"struct%s",st_iter->data.string_value);
            type->info.structure.define = 0;
            type->info.structure.elem = NULL;
            type->level = hierarchy;
            //TypePtr rtype;
            //find_type(type,rtype);
            add_type(hierarchy,type,st_iter->lineno);
            ret = type;
            return 1;
        }else if(st_iter->n_children == 5){             // StructSpecifier -> STRUCT OptTag LC DefList RC
            type = (TypePtr)malloc(sizeof(struct TypeItem_t));
            type->kind = STRUCTURE;
            if(st_iter->children[1]->n_children == 1){  // StructSpecifier -> STRUCT ID LC DefList RC
                sprintf(type->name,"struct%s",st_iter->children[1]->children[0]->data.string_value);
            }else
                sprintf(type->name,"struct");
            type->info.structure.define = 1;
            type->level = hierarchy;
        }
    }

}