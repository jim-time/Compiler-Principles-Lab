#include "SymbolTable.h"
#include "TypeCheck.h"
#include "SyntaxTree.h"
#include "errtype.h"

void InitTable(void){
    functions   = NULL;
    globalvars  = NULL;
    localvars   = NULL;
}

int add_func(){

    return 1;
}

int add_globalvar(TypePtr type, char* name, int name_lineno){
    struct GlobalVarTable_t* entry;
    HASH_FIND_STR(globalvars,name,entry);
    if(entry){
        printf("Error type %d at line %d: Redefined variable \"%s\"\n",REDEFINED_VAR,name_lineno,name);
        return 0;
    }
    entry = (struct GlobalVarTable_t*)malloc(sizeof(struct GlobalVarTable_t));
    entry->type = type;
    entry->line = name_lineno;
    strcpy(entry->name,name);
    
    HASH_ADD_KEYPTR(hh,globalvars,entry->name,strlen(entry->name),entry);
    return 1;
}

int add_localvar(){

    return 1;
}