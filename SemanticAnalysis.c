#include "main.h"

int hierarchy = 0;

/*Syntax tree analize
 *
 * 
 *
**/  
int ST_Program(struct SyntaxTreeNode* Program){                             // Program -> ExtDefList
    //create a type table & symbol table
    tt_create();
    vartab_stack_create();

    ST_ExtDefList(Program->children[0]);
    print_typetable();
    print_vartable();
    print_functable();
    return 1;
}

// ExtDefList -> ExtDef ExtDefList
// ExtDefList -> NULL
int ST_ExtDefList(struct SyntaxTreeNode* ExtDefList){
    struct SyntaxTreeNode* pExtDefList = ExtDefList;

    for(;pExtDefList->n_children == 2;pExtDefList = pExtDefList->children[1]){                  
        ST_ExtDef(pExtDefList->children[0]);
    }
    return 1;
}

// ExtDef -> Specifier ExtDecList SEMI
// ExtDef -> Specifier SEMI
// ExtDef -> Specifier FunDec SEMI
// ExtDef -> Specifier FunDec CompSt
int ST_ExtDef(struct SyntaxTreeNode* ExtDef){

    if(!strcmp(ExtDef->children[1]->node_name,"ExtDecList")){               // ExtDef -> Specifier ExtDecList SEMI
        TypePtr type;
        if(ST_Specifier(ExtDef->children[0],&type)){                         // get the prefix of type
            if(ST_ExtDecList(ExtDef->children[1],type)){                    // pass the prefix of type to var
                return 1;
            }
        }
    }else if(!strcmp(ExtDef->children[1]->node_name,"SEMI")){               // ExtDef -> Specifier SEMI 
        TypePtr gtype;
        if(ST_Specifier(ExtDef->children[0],&gtype)){                          // get the global type
            return 1;
        }else
            return 0;

    }else if(!strcmp(ExtDef->children[1]->node_name,"FunDec")){             // ExtDef -> Specifier FunDec SEMI | Specifier FunDec CompSt
        if(!strcmp(ExtDef->children[2]->node_name,"SEMI")){                 // ExtDef -> Specifier FunDec SEMI
        //declare a function
            TypePtr ret_type;
            FuncTablePtr func;
            if(ST_Specifier(ExtDef->children[0],&ret_type)){
                func = (FuncTablePtr)malloc(sizeof(struct FuncTable_t));
                if(ST_FuncDec(ExtDef->children[1],func)){
                    func->ret_type = ret_type;
                    func->define = FUNC_DECLARED;
                    add_func(func,ExtDef->children[1]->lineno);
                }
            }
            
        }else{                                                              // ExtDef -> Specifier FunDec CompSt
        //define a function
            TypePtr ret_type;
            FuncTablePtr func;
            if(ST_Specifier(ExtDef->children[0],&ret_type)){
                func = (FuncTablePtr)malloc(sizeof(struct FuncTable_t));
                if(ST_FuncDec(ExtDef->children[1],func)){
                    func->ret_type = ret_type;
                    func->define = FUNC_DEFINED;
                    add_func(func,ExtDef->children[1]->lineno);
                    //enter CompSt, 
                    ST_CompSt(ExtDef->children[2]);
                }
            }
        }
    }

    return 1;
}

// ExtDecList -> VarDec COMMA ExtDecList
// ExtDecList -> VarDec
int ST_ExtDecList(struct SyntaxTreeNode* ExtDecList, TypePtr field_type){
    struct SyntaxTreeNode* pExtDecList = ExtDecList;
    FieldListPtr field;
    for(;pExtDecList->n_children == 3;pExtDecList = pExtDecList->children[2]){
        if(!ST_VarDec(pExtDecList->children[0],field_type, &field)){
            printf("Get VarDec failed\n");
            return 0;
        }
        if(!add_var(CompStLevel,field->type,field->name,pExtDecList->lineno)){
            return 0;
        }
    }
    if(ST_VarDec(pExtDecList->children[0],field_type, &field)){                   // ExtDecList -> VarDec
        if(!add_var(CompStLevel,field->type,field->name,pExtDecList->lineno)){
            return 0;
        }
    }
    return 1;
}

//Specifier : TYPE
//Specifier : StructSpecifier
int ST_Specifier(struct SyntaxTreeNode* Specifier,TypePtr* ret){
    TypePtr type;
    struct SyntaxTreeNode* st_iter = Specifier;
    st_iter = st_iter->children[0];                     // Specifier -> TYPE | StructSpecifier
    if(!strcmp(st_iter->node_name,"TYPE")){             // Specifier -> TYPE , st_iter = TYPE
        type = (TypePtr)malloc(sizeof(struct TypeItem_t));
        type->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
        strcpy(type->name,st_iter->data.string_value);
        find_type(type,ret);
        free(type);
        return 1;
    }else{                                               // Specifier -> StructSpecifier , st_iter = StructSpecifier
       if(ST_StructSpecifier(st_iter,&type)){
            *ret = type;
            return 1;
       }else{
           *ret = NULL;
           return 0;
       }
    }
}

// StructSpecifier -> STRUCT Tag
// StructSpecifier -> STRUCT OptTag LC DefList RC
int ST_StructSpecifier(struct SyntaxTreeNode* StructSpecifier,TypePtr* ret){
    struct SyntaxTreeNode* st_iter = StructSpecifier;
    TypePtr type;
    if(st_iter->n_children == 2){                    // StructSpecifier -> STRUCT Tag , declare a structure
        char* id;
        ST_Tag(StructSpecifier->children[1],(const char**)&id);
        type = (TypePtr)malloc(sizeof(struct TypeItem_t));
        if(!type){
            printf("create type failed\n");
            return 0;
        }
        type->kind = STRUCTURE;
        type->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
        sprintf(type->name,"struct %s",id);
        type->info.structure.define = TYPE_DECLARED;
        type->info.structure.elem = NULL;
        type->level = CompStLevel + hierarchy;
        *ret = add_type(CompStLevel + hierarchy,type,st_iter->lineno);
        //ret = type;
        return 1;
    }else if(st_iter->n_children == 5){             // StructSpecifier -> STRUCT OptTag LC DefList RC
        type = (TypePtr)malloc(sizeof(struct TypeItem_t));
        if(!type){
            printf("create type failed\n");
            return 0;
        }
        type->kind = STRUCTURE;
        char* id;
        ST_OptTag(st_iter->children[1],(const char**)&id);
        type->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
        if(id){                                     // StructSpecifier -> STRUCT ID LC DefList RC
            sprintf(type->name,"struct %s",id);
        }else
            sprintf(type->name,"struct");
        type->info.structure.define = TYPE_DEFINED;
        type->level = CompStLevel + hierarchy;
        hierarchy++;
        //tt_push_bucket(); 
        if(!ST_DefList(StructSpecifier->children[3],&type->info.structure.elem)){
            printf("Get the structure elem failed\n");
            return 0;
        }
        //tt_pop_bucket();
        hierarchy--;
        *ret = add_type(CompStLevel + hierarchy,type,st_iter->lineno);
        return 1;
    }
}

int ST_OptTag(struct SyntaxTreeNode* OptTag,const char** ret_id){

    if(OptTag->n_children == 1){                                             // OptTag -> ID 
        *ret_id = OptTag->children[0]->data.string_value;
    }else if(OptTag->n_children == 0){                                       // OptTag -> NULL
        *ret_id = NULL;
    }
    return 1;
}int print_typetable();

int ST_Tag(struct SyntaxTreeNode* Tag, const char** ret_id){                  // Tag -> ID
    *ret_id = Tag->children[0]->data.string_value;
    return 1;
}

//ExtDecList : VarDec
//ExtDecList :VarDec COMMA ExtDecList 
//ParamDec : Specifier VarDec
//Dec : VarDec 
// VarDec -> ID
// VarDec -> VarDec LB INT RB
int ST_VarDec(struct SyntaxTreeNode* VarDec,TypePtr field_type, FieldListPtr* ret_field){   
    struct SyntaxTreeNode* pVarDec = VarDec;
    int n_bracket;
    if(strcmp(VarDec->parent->node_name,"VarDec")){                             // (not a VarDec)-> VarDec -> xxx
        (*ret_field) = (FieldListPtr)malloc(sizeof(struct FieldList));
        (*ret_field)->type = field_type;
    }
    for(n_bracket = 0;pVarDec->n_children == 4;pVarDec = pVarDec->children[0],n_bracket++){               // VarDec -> VarDec LB INT RB
        TypePtr arr_type = (TypePtr)malloc(sizeof(struct TypeItem_t));
        arr_type->kind = ARRAY;
        arr_type->info.array.size = pVarDec->children[2]->data.int_value;
        arr_type->level = CompStLevel + hierarchy;
        arr_type->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
        if(strcmp(pVarDec->parent->node_name,"VarDec"))
            sprintf(arr_type->name,"%s []",(*ret_field)->type->name);
        else
            sprintf(arr_type->name,"%s[]",(*ret_field)->type->name);
        // append the array info
        arr_type->info.array.elem = (*ret_field)->type;
        (*ret_field)->type = arr_type;
    }
    if(pVarDec->n_children == 1){                                                // VarDec -> ID
        (*ret_field)->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
        strcpy((*ret_field)->name,pVarDec->children[0]->data.string_value);
        (*ret_field)->lineno = pVarDec->lineno;
        if((*ret_field)->type->kind == ARRAY){
            memset((*ret_field)->type->name,'\0',TYPE_NAME_LEN*sizeof(char));
            int plen = 0;TypePtr parray = (*ret_field)->type;
            plen = sprintf((*ret_field)->type->name,"%s ",field_type->name);
            for(;parray->kind == ARRAY;parray = parray->info.array.elem){
                plen += sprintf((*ret_field)->type->name+plen,"[%d]",parray->info.array.size);
            }
        }
        //add_type();    
    }
    return 1;
}

int ST_FuncDec(struct SyntaxTreeNode* FuncDec,FuncTablePtr func){
    int a;
    if(FuncDec->n_children == 4){                                           // FunDec -> ID LP VarList RP 
        func->name = FuncDec->children[0]->data.string_value;
        if(ST_VarList(FuncDec->children[2],func)){
            return 1;
        }else
            return 0;
    }else if(FuncDec->n_children == 3){                                     // FunDec -> ID LP RP 
        func->name = FuncDec->children[0]->data.string_value;
        func->n_param = 0;
        func->param_list = NULL;
        return 1;
    }
    return 0;
}

int ST_VarList(struct SyntaxTreeNode* VarList,FuncTablePtr func){
    struct SyntaxTreeNode* pVarList = VarList;
    func->n_param = 0;
    FieldListPtr* para_ptr = &(func->param_list);
    for(;pVarList->n_children == 3;pVarList = pVarList->children[2]){       // VarList -> ParamDec COMMA VarList 
        if(ST_ParamDec(pVarList->children[0],para_ptr)){
            para_ptr = &((*para_ptr)->tail);
            func->n_param++;
        }else{
            printf("Get the func para failed\n");
            return 0;
        }    
    }
    if(pVarList->n_children == 1){                                          // VarList -> ParamDec
        if(ST_ParamDec(pVarList->children[0],para_ptr)){
            para_ptr = &((*para_ptr)->tail);
            func->n_param++;
        }else{
            printf("Get the func para failed\n");
            return 0;
        }    
    }
    (*para_ptr) = NULL;
    return 1;
}

int ST_ParamDec(struct SyntaxTreeNode* ParamDec, FieldListPtr* para){                           // ParamDec -> Specifier VarDec
    TypePtr para_type;
    if(ST_Specifier(ParamDec->children[0],&para_type)){
        if(ST_VarDec(ParamDec->children[1],para_type,para)){
            return 1;
        }
    }
    return 0;
}

int ST_CompSt(struct SyntaxTreeNode* CompSt){                               // CompSt -> LC DefList StmtList RC
    FieldListPtr l_vars;
    CompStLevel++;
    vartab_stack_push();
    tt_push_bucket();
    if(ST_DefList(CompSt->children[1],&l_vars)){
        // add local vars
        for(;l_vars;l_vars = l_vars->tail){
            add_var(CompStLevel,l_vars->type,l_vars->name,l_vars->lineno);
        }
        
        if(!ST_StmtList(CompSt->children[2])){
            tt_pop_bucket();
            clear_local_var();
            CompStLevel--;
            return 0;
        }
    }
    print_typetable();
    print_vartable();
    tt_pop_bucket();
    clear_local_var();
    CompStLevel--;
    return 1;
}

// StmtList -> Stmt StmtList
// StmtList -> NULL
int ST_StmtList(struct SyntaxTreeNode* StmtList){
    struct SyntaxTreeNode* pStmtList = StmtList;
    for(;pStmtList->n_children == 2;pStmtList = pStmtList->children[1]){
        ST_Stmt(pStmtList->children[0]);
    }
    
    return 1;
}

int ST_Stmt(struct SyntaxTreeNode* Stmt){

    if(Stmt->n_children == 2){                                             // Stmt -> Exp SEMI
        ST_Exp(Stmt->children[0]);
    }else if(Stmt->n_children == 1){                                       // Stmt -> CompSt 
        ST_CompSt(Stmt->children[0]);
    }else if(Stmt->n_children == 3){                                       // Stmt -> RETURN Exp SEMI
        //type check
        ST_Exp(Stmt->children[1]);
    }else if(Stmt->n_children == 5){
        if(!strcmp(Stmt->node_name,"IF")){                                 // Stmt -> IF LP Exp RP Stmt
            if(ST_Exp(Stmt->children[2])){
                //type check
                if(ST_Stmt(Stmt->children[4])){

                }
            }
        }else{                                                             // Stmt -> WHILE LP Exp RP Stmt
            if(ST_Exp(Stmt->children[2])){
                //type check
                if(ST_Stmt(Stmt->children[4])){
                    
                }
            }
        }
    }else if(Stmt->n_children == 7){                                       // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if(ST_Exp(Stmt->children[2])){
                //type check
            if(ST_Stmt(Stmt->children[4])){

                if(ST_Stmt(Stmt->children[6])){                             //else Stmt

                }
            }
        }
    }
    return 1;
}

// StructSpecifier : STRUCT OptTag LC DefList RC
// CompSt : LC DefList StmtList RC
// DefList -> Def DefList || NULL
int ST_DefList(struct SyntaxTreeNode* DefList,FieldListPtr* ret_field){      //complete the tail field    
    struct SyntaxTreeNode* pDefList = DefList; 
    FieldListPtr* field;                                               
    // DefList -> Def DefList 
    for(field = ret_field;pDefList->n_children;pDefList = pDefList->children[1]){
        if(!ST_Def(pDefList->children[0],field)){
            printf("get the field failed\n");
            return 0;
        }
        for(;(*field);field = &((*field)->tail));
    }
    // DefList -> NULL 
    //*field = NULL;
    return 1;
}

int ST_Def(struct SyntaxTreeNode* Def,FieldListPtr* ret_field){            // Def -> Specifier DecList SEMI
    TypePtr field_type;
    if(ST_Specifier(Def->children[0],&field_type)){
        if(ST_DecList(Def->children[1],field_type,ret_field)){
            return 1;
        }
    }
    return 0;
}
// DecList -> Dec 
// DecList -> Dec COMMA DecList 
int ST_DecList(struct SyntaxTreeNode* DecList,TypePtr field_type,FieldListPtr *ret_field){  //complete the decnext field
    struct SyntaxTreeNode* pDecList = DecList;
    FieldListPtr* field;                                                 // allocate the memory in DefList    
    // DecList -> Dec COMMA DecList                                                  
    for(field = ret_field;pDecList->n_children == 3;pDecList = pDecList->children[2]){
        if(ST_Dec(pDecList->children[0],field_type,field)){
            field = &((*field)->tail);
            continue;
        }else{
            printf("Get the Dec failed\n");
            return 0;
        }
    }
    // DecList -> Dec
    if(!ST_Dec(pDecList->children[0],field_type,field)){
        printf("Get the Dec failed\n");
        return 0;
    }
    (*field)->tail = NULL;
    return 1;
}

int ST_Dec(struct SyntaxTreeNode* Dec,TypePtr field_type,FieldListPtr* ret_field){                                      

    if(Dec->n_children == 1){                                               // Dec -> VarDec
        if(!ST_VarDec(Dec->children[0],field_type, ret_field)){
            printf("Get the VarDec failed\n");
            return 0;
        }
    }else if(Dec->n_children == 3){                                         // Dec -> VarDec ASSIGNOP Exp
        if(!ST_VarDec(Dec->children[0],field_type, ret_field)){
            printf("Get the VarDec failed\n");
            return 0;
        }
        if(!ST_Exp(Dec->children[2])){                                       // assign the value of expression to VarDec
            printf("Get the Exp for VarDec failed\n");
            return 0;
        }
        // check the assignment 

    }
    return 1;
}

int ST_Exp(struct SyntaxTreeNode* Exp){

    if(Exp->n_children == 3){
        if(!strcmp(Exp->children[1]->node_name,"ASSIGNOP")){                // Exp : Exp ASSIGNOP Exp

        }else if(!strcmp(Exp->children[1]->node_name,"AND")){               // Exp : Exp AND Exp

        }else if(!strcmp(Exp->children[1]->node_name,"OR")){                // Exp : Exp OR Exp

        }else if(!strcmp(Exp->children[1]->node_name,"RELOP")){             // Exp : Exp RELOP Exp

        }else if(!strcmp(Exp->children[1]->node_name,"PLUS")){              // Exp : Exp PLUS Exp

        }else if(!strcmp(Exp->children[1]->node_name,"MINUS")){             // Exp : Exp MINUS Exp

        }else if(!strcmp(Exp->children[1]->node_name,"STAR")){              // Exp : Exp STAR Exp

        }else if(!strcmp(Exp->children[1]->node_name,"DIV")){               // Exp : Exp DIV Exp

        }else if(!strcmp(Exp->children[1]->node_name,"Exp")){               // Exp : LP Exp RP

        }else if(!strcmp(Exp->children[1]->node_name,"LP")){                // Exp : ID LP RP

        }else if(!strcmp(Exp->children[1]->node_name,"DOT")){               // Exp : Exp DOT ID

        }
    }else if(Exp->n_children == 2){
        if(!strcmp(Exp->children[0]->node_name,"MINUS")){                   // Exp : MINUS Exp

        }else if(!strcmp(Exp->children[0]->node_name,"NOT")){               // Exp : NOT Exp 

        }
    }else if(Exp->n_children == 4){
        if(!strcmp(Exp->children[0]->node_name,"ID")){                      // Exp : ID LP Args RP

        }else if(!strcmp(Exp->children[0]->node_name,"Exp")){               // Exp : Exp LB Exp RB

        }
    }else if(Exp->n_children == 1){
        if(!strcmp(Exp->children[0]->node_name,"ID")){                      // Exp : ID

        }else if(!strcmp(Exp->children[0]->node_name,"INT")){               // Exp : INT

        }else if(!strcmp(Exp->children[0]->node_name,"FLOAT")){             // Exp : FLOAT

        }
    }else{

    }
    return 1;
}

int ST_Args(struct SyntaxTreeNode* Args){                                   
    
    if(Args->n_children == 3){                                              // Args -> Exp COMMA Args

    }else if(Args->n_children == 1){                                        // Args -> Exp

    }
    return 1;
}


