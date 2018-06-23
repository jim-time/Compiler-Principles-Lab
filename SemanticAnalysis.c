#include "main.h"
#define PRINT_TBALE 1

//struct hierarchy
uint16_t hierarchy = 0;

/*Syntax tree analize
 *
 * 
 *
**/  
extern FILE* pic;
int ST_Program(struct SyntaxTreeNode* Program){                             // Program -> ExtDefList
    //create a type table & symbol table
    tt_create();
    vartab_stack_create();
    functab_create();

    //create intercodes
    intercodes.create(&intercodes);

    ST_ExtDefList(Program->children[0]);
    #if PRINT_TBALE
        print_typetable();
        print_vartable();
        print_functable();
    #else
        pic = stdout;
    #endif
    optimization_intercodes();
    print_intercodes(pic);

    return 1;
}

// ExtDefList -> ExtDef ExtDefList
// ExtDefList -> NULL
int ST_ExtDefList(struct SyntaxTreeNode* ExtDefList){
    struct SyntaxTreeNode* pExtDefList = ExtDefList;

    for(;pExtDefList->n_children == 2;pExtDefList = pExtDefList->children[1]){                  
        ST_ExtDef(pExtDefList->children[0]);
    }
    // check the definition of function 
    check_func_def();
    return 1;
}

// ExtDef -> Specifier ExtDecList SEMI
// ExtDef -> Specifier SEMI
// ExtDef -> Specifier FunDec SEMI
// ExtDef -> Specifier FunDec CompSt
int ST_ExtDef(struct SyntaxTreeNode* ExtDef){
    
    if(!strcmp(ExtDef->children[1]->node_name,"ExtDecList")){                   // ExtDef -> Specifier ExtDecList SEMI
        TypePtr type;
        if(ST_Specifier(ExtDef->children[0],&type)){                            // get the prefix of type
            if(ST_ExtDecList(ExtDef->children[1],type)){                        // pass the prefix of type to var
                return 1;
            }
        }
    }else if(!strcmp(ExtDef->children[1]->node_name,"SEMI")){                   // ExtDef -> Specifier SEMI 
        TypePtr gtype;
        if(ST_Specifier(ExtDef->children[0],&gtype)){                          // declare some type
            return 1;
        }else
            return 0;

    }else if(!strcmp(ExtDef->children[1]->node_name,"FunDec")){                 // ExtDef -> Specifier FunDec SEMI | Specifier FunDec CompSt
        if(!strcmp(ExtDef->children[2]->node_name,"SEMI")){                     // ExtDef -> Specifier FunDec SEMI
        //declare a function
            TypePtr ret_type;
            FuncTablePtr func;
            if(ST_Specifier(ExtDef->children[0],&ret_type)){
                func = (FuncTablePtr)malloc(sizeof(struct FuncTable_t));
                if(ST_FuncDec(ExtDef->children[1],func)){
                    func->ret_type = ret_type;
                    func->define = FUNC_DECLARED;
                    func->lineno = ExtDef->children[1]->lineno;
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
                    func->lineno = ExtDef->children[1]->lineno;
                    add_func(func,ExtDef->children[1]->lineno);
                    //enter CompSt, 
                    ST_CompSt(ExtDef->children[2],func);
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
        if(!add_var(0,field->type,field->name,pExtDecList->lineno)){
            return 0;
        }
        //create intercodes for global variable
        translate_globalvar(field->name);
    }
    if(ST_VarDec(pExtDecList->children[0],field_type, &field)){                   // ExtDecList -> VarDec
        if(!add_var(0,field->type,field->name,pExtDecList->lineno)){
            return 0;
        }
        //create intercodes for global varable
        translate_globalvar(field->name);
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
        type->info.structure.elem = NULL;
        type->compst = CompStLevel;
        type->hierarchy = hierarchy;
        
        if(!strcmp(StructSpecifier->parent->parent->children[1]->node_name,"SEMI")){       //DECLARED
            type->info.structure.define |= TYPE_DECLARED;
            *ret = add_type(type->level,type,st_iter->lineno);
        }else{                                                                  // TYPE_REFERENCE
            TypePtr ret_type;
            find_type(type,&ret_type);
            if(!ret_type || !(ret_type->info.structure.define & TYPE_DEFINED)){
                printf("Error type %d at line %d: Undefined structure \"%s\"\n",UNDEFINED_STRUCT,st_iter->lineno,type->name);
                ret = NULL;
                return 0;
            }
            *ret = ret_type;
        }
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
        type->info.structure.define |= TYPE_DEFINED;
        type->info.structure.size = 0;

        vartab_stack_push();
        //tt_push_bucket();
        //CompStLevel++;
        hierarchy++;
        type->compst = CompStLevel;
        type->hierarchy = hierarchy;
        FieldListPtr l_vars;
        if(!ST_DefList(StructSpecifier->children[3],&l_vars)){
            printf("Get the structure elem failed\n");
            return 0;
        }
        type->info.structure.elem = l_vars;
        // add local vars
        for(;l_vars;l_vars = l_vars->tail){
            //add_var(type->level,l_vars->type,l_vars->name,l_vars->lineno);
            // compute the size of struture
            int array_size = 1;
            TypePtr parr = l_vars->type;
            switch(l_vars->type->kind){
                case BASIC:
                    type->info.structure.size +=4;
                    break;
                case ARRAY:                    
                    for(;parr->kind == ARRAY; parr = parr->info.array.elem){
                        array_size *= (parr->info.array.size);
                    }
                    if(parr->kind == BASIC)
                        type->info.structure.size += (4*array_size);
                    else if(parr->kind == STRUCTURE)
                        type->info.structure.size += (parr->info.structure.size * array_size);
                    break;
                case STRUCTURE:
                    type->info.structure.size += l_vars->type->info.structure.size;
                    break;
                default:
                    break;
            }
        }
        //CompStLevel--;
        hierarchy--;
        //tt_pop_bucket();
        *ret = add_type(type->level,type,st_iter->children[0]->lineno);
        #if PRINT_TBALE
            print_vartable();
        #endif
        clear_local_var();
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
}

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
    int arr_index;
    if(strcmp(VarDec->parent->node_name,"VarDec")){                             // (not a VarDec)-> VarDec -> xxx
        (*ret_field) = (FieldListPtr)malloc(sizeof(struct FieldList));
        (*ret_field)->type = field_type;
    }
    for(arr_index = 1;pVarDec->n_children == 4;pVarDec = pVarDec->children[0]){               // VarDec -> VarDec LB INT RB
        TypePtr arr_type = (TypePtr)malloc(sizeof(struct TypeItem_t));
        arr_type->kind = ARRAY;
        arr_type->info.array.size = pVarDec->children[2]->data.int_value;
        // arr_index used for generate intercodes
        arr_type->info.array.index = arr_index;
        arr_index *= arr_type->info.array.size;

        arr_type->compst = CompStLevel;
        arr_type->hierarchy = hierarchy;
        //arr_type->level = CompStLevel + hierarchy;
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

int ST_CompSt(struct SyntaxTreeNode* CompSt,FuncTablePtr func){                               
    FieldListPtr l_vars;
    CompStLevel++;
    vartab_stack_push();
    tt_push_bucket();
    // add the function parameter
    if(CompSt->parent->n_children == 3){                                                      //ExtDef : Specifier FunDec CompSt
        if(func->n_param){
            FieldListPtr pParam = func->param_list;
            for(;pParam;pParam = pParam->tail){
                add_var(CompStLevel,pParam->type,pParam->name,pParam->lineno);
            }
        }
        // enter the function  and clear the cnt for local variable
        local_cnt = 1;
        temp_cnt = 1;
        // generate intercodes on function declaration
        translate_func_dec(func);
    }
    // CompSt -> LC DefList StmtList RC
    if(ST_DefList(CompSt->children[1],&l_vars)){
        /*if(CompSt->children[1]->n_children){
            // add local vars
            TypePtr temp;
            temp->compst = CompStLevel;
            temp->hierarchy = hierarchy;
            for(;l_vars;l_vars = l_vars->tail){
                add_var(temp->level,l_vars->type,l_vars->name,l_vars->lineno);
            }
        }*/
        if(!ST_StmtList(CompSt->children[2],func)){
            tt_pop_bucket();
            clear_local_var();
            CompStLevel--;
            return 0;
        }
    }
    #if PRINT_TBALE
        print_typetable();
        print_vartable();
    #endif

    tt_pop_bucket();
    clear_local_var();
    CompStLevel--;
    return 1;
}

// StmtList -> Stmt StmtList
// StmtList -> NULL
int ST_StmtList(struct SyntaxTreeNode* StmtList,FuncTablePtr func){
    struct SyntaxTreeNode* pStmtList = StmtList;
    for(;pStmtList->n_children == 2;pStmtList = pStmtList->children[1]){
        ST_Stmt(pStmtList->children[0],func);
    }
    return 1;
}

int ST_Stmt(struct SyntaxTreeNode* Stmt,FuncTablePtr func){
    FieldListPtr exp_val;
    struct SyntaxTreeNode* Exp;
    if(Stmt->n_children == 2){                                             // Stmt -> Exp SEMI
        Exp = Stmt->children[0];
        ST_Exp(Exp,&exp_val);
            /*  Legal Exp SEMI
                Exp ASSIGNOP Exp
                ID LP Args RP || ID LP RP
                Warning Item
                ID
                Exp LB Exp RB
                Exp DOT ID
            */
           /* Warning Hint
           if((!strcmp(Exp->children[1]->node_name,"ASSIGNOP")) || (!strcmp(Exp->children[1]->node_name,"LP"))){
               
           }else{
               printf("Error type %d at line %d: Redefined Variable \"%s\"\n",REDEFINED_VAR,Dec->lineno,(*ret_field)->name);
           }*/
    }else if(Stmt->n_children == 1){                                       // Stmt -> CompSt 
        ST_CompSt(Stmt->children[0],func);
    }else if(Stmt->n_children == 3){                                       // Stmt -> RETURN Exp SEMI
        //type check
        ST_Exp(Stmt->children[1],&exp_val);
        if(exp_val){
            if(!isTypeEqual(exp_val->type,func->ret_type)){
                printf("Error type %d at line %d: Type mismatched for return\n",MISMATCHED_RETURN,Stmt->children[1]->lineno);
            }
        }
        // generate intercodes for return statement
        translate_return(exp_val);
    }else if(Stmt->n_children == 5){
        if(!strcmp(Stmt->node_name,"IF")){                                 // Stmt -> IF LP Exp RP Stmt
            int true_cnt,false_cnt;
            true_cnt = label_cnt++;
            false_cnt = label_cnt++;
            //if(ST_Exp(Stmt->children[2],&exp_val)){
            if(translate_cond(Stmt->children[2],&exp_val,true_cnt,false_cnt)){
                //type check
                if(!(exp_val->type->kind == BASIC && exp_val->type->info.basic == BASIC_INT)){
                    printf("Error type %d at line %d: Statement requires expression of integer type\n",NOT_INT,Stmt->children[2]->lineno);
                }
                // generate true Label
                translate_label('t',true_cnt);
                if(ST_Stmt(Stmt->children[4],func)){
                    // generate next Label
                    translate_label('f',false_cnt);
                }
            }
        }else{                                                             // Stmt -> WHILE LP Exp RP Stmt
            int true_cnt,false_cnt,next_cnt;
            true_cnt = label_cnt++;
            false_cnt = label_cnt++;
            next_cnt = label_cnt++;
            // generate next Label
            translate_label('n',next_cnt);

            //ST_Exp(Stmt->children[2],&exp_val);
            translate_cond(Stmt->children[2],&exp_val,true_cnt,false_cnt);
            //type check
            if(exp_val){
                if(exp_val->type->kind != BASIC || exp_val->type->info.basic != BASIC_INT){
                    printf("Error type %d at line %d: Statement requires expression of integer type\n",NOT_INT,Stmt->children[2]->lineno);
                }
            }
            // generate true Label
            translate_label('t',true_cnt);

            if(ST_Stmt(Stmt->children[4],func)){
                // goto the while loop
                translate_goto(next_cnt);
                // if false, it goes to the false_label
                translate_label('f',false_cnt);
            }
        }
    }else if(Stmt->n_children == 7){                                       // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        int true_cnt,false_cnt,next_cnt;
        true_cnt = label_cnt++;
        false_cnt = label_cnt++;
        next_cnt = label_cnt++;
        //ST_Exp(Stmt->children[2],&exp_val);
        translate_cond(Stmt->children[2],&exp_val,true_cnt,false_cnt);
        //type check
        if(exp_val){
            if(!(exp_val->type->kind == BASIC && exp_val->type->info.basic == BASIC_INT)){
                printf("Error type %d at line %d: Statement requires expression of integer type\n",NOT_INT,Stmt->children[2]->lineno);
            }
        }
        // generate true label
        translate_label('t',true_cnt);
        
        if(ST_Stmt(Stmt->children[4],func)){
            // and then goto the next
            translate_goto(next_cnt);
            // othewise, goto the false
            translate_label('f',false_cnt);
            if(ST_Stmt(Stmt->children[6],func)){                             //else Stmt
                // the next
                translate_label('n',next_cnt);
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
            //printf("get the field failed\n");
            //return 0;
            continue;
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
            //printf("Get the Dec failed\n");
            //return 0;
        }
    }
    // DecList -> Dec
    if(!ST_Dec(pDecList->children[0],field_type,field)){
        //printf("Get the Dec failed\n");
        return 0;
    }
    (*field)->tail = NULL;
    return 1;
}

int ST_Dec(struct SyntaxTreeNode* Dec,TypePtr field_type,FieldListPtr* ret_field){                                      
    // update the CompStLevel info
    TypeTable t;
    t.compst = CompStLevel;
    t.hierarchy = hierarchy;

    if(Dec->n_children == 1){                                               // Dec -> VarDec
        if(!ST_VarDec(Dec->children[0],field_type, ret_field)){
            printf("Get the VarDec failed\n");
            return 0;
        }
        // add local variable
        add_var(t.level,(*ret_field)->type,(*ret_field)->name,(*ret_field)->lineno);
        // create the intercodes for local variable
        if(hierarchy == 0)
            translate_localvar((*ret_field)->name);
    }else if(Dec->n_children == 3){                                         // Dec -> VarDec ASSIGNOP Exp
        if(!ST_VarDec(Dec->children[0],field_type, ret_field)){
            printf("Get the VarDec failed\n");
            return 0;
        }
        // add local variable
        add_var(t.level,(*ret_field)->type,(*ret_field)->name,(*ret_field)->lineno);

        // create the intercodes for local variable
        if(hierarchy == 0)
            translate_localvar((*ret_field)->name);

        // assign the value of expression to VarDec
        FieldListPtr exp_val;
        if(!ST_Exp(Dec->children[2],&exp_val)){                                       
            printf("Get the Exp for VarDec failed\n");
            return 0;
        }
        
        // check the assignment 
        // variable assignment in structure definition is not allowed
        if(hierarchy>0){
            printf("Error type %d at line %d: Redefined Variable \"%s\"\n",REDEFINED_VAR,Dec->lineno,(*ret_field)->name);
            return 0;
        }

        // check the type
        if((*ret_field)->type->kind != exp_val->type->kind){
            printf("Error type %d at line %d: Type mismatched for assignment around \"%s\"\n",MISMATCHED_ASSIGNMENT,Dec->children[0]->lineno,(*ret_field)->name);
        }else{
            if((*ret_field)->type->kind == BASIC){
                if((*ret_field)->type->info.basic != exp_val->type->info.basic){
                    printf("Error type %d at line %d: Type mismatched for assignment around \"%s\"\n",MISMATCHED_ASSIGNMENT,Dec->children[0]->lineno,(*ret_field)->name);
                }
            }
            // update the alias
            VarTablePtr var;
            find_var((*ret_field)->name,&var);
            (*ret_field)->alias = var->alias;
            // generate intercodes for assignment
            translate_assign((*ret_field),exp_val);
        }
    }
    return 1;
}

int ST_Exp(struct SyntaxTreeNode* Exp,FieldListPtr* ret_val){

    if(Exp->n_children == 3){
        if(!strcmp(Exp->children[1]->node_name,"ASSIGNOP")){                // Exp : Exp ASSIGNOP Exp
            return ST_ASSIGNOP(Exp->children[0],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"AND")){               // Exp : Exp AND Exp
            return ST_2OP_Logic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"OR")){                // Exp : Exp OR Exp
            return ST_2OP_Logic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"RELOP")){             // Exp : Exp RELOP Exp
            return ST_2OP_Logic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"PLUS")){              // Exp : Exp PLUS Exp
            return ST_Arithmetic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"MINUS")){             // Exp : Exp MINUS Exp
            return ST_Arithmetic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"STAR")){              // Exp : Exp STAR Exp
            return ST_Arithmetic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"DIV")){               // Exp : Exp DIV Exp
            return ST_Arithmetic(Exp->children[0],Exp->children[1],Exp->children[2],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"Exp")){               // Exp : LP Exp RP
            return ST_Exp(Exp->children[1],ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"LP")){                // Exp : ID LP RP
            return ST_CallFunc(Exp->children[0], NULL, ret_val);
        }else if(!strcmp(Exp->children[1]->node_name,"DOT")){               // Exp : Exp DOT ID
            ST_StructField(Exp->children[0],Exp->children[2],ret_val);
        }
    }else if(Exp->n_children == 2){
        // Exp : MINUS Exp 
        // Exp : NOT Exp
        return ST_1OP_Logic(Exp->children[0],Exp->children[1],ret_val);
    }else if(Exp->n_children == 4){
        if(!strcmp(Exp->children[0]->node_name,"ID")){                      
            // Exp : ID LP Args RP
            return ST_CallFunc(Exp->children[0], Exp->children[2], ret_val);
        }else if(!strcmp(Exp->children[0]->node_name,"Exp")){               
            // Exp : Exp LB Exp RB
            return ST_Array(Exp->children[0],Exp->children[2],ret_val);
        }
    }else if(Exp->n_children == 1){
        if(!strcmp(Exp->children[0]->node_name,"ID")){                      // Exp : ID
            VarTablePtr var;
            (*ret_val) = (FieldListPtr)malloc(sizeof(struct FieldList));
            find_var(Exp->children[0]->data.string_value,&var);
            if(!var){
                printf("Error type %d at line %d: Undefined Variable \"%s\"\n",UNDEFINED_VAR,Exp->children[0]->lineno,Exp->children[0]->data.string_value);
                (*ret_val)->type = (TypePtr)malloc(sizeof(struct TypeItem_t));
                (*ret_val)->type->kind = NOTYPE;
                (*ret_val)->name = Exp->children[0]->data.string_value;
                (*ret_val)->val_ptr = NULL;
                return 0;
            }
            (*ret_val)->type = var->type;
            (*ret_val)->val_ptr = var->val_ptr;
            (*ret_val)->name = var->name;
            (*ret_val)->alias = var->alias;
            return 1;
        }else if(!strcmp(Exp->children[0]->node_name,"INT")){               // Exp : INT
            (*ret_val) = (FieldListPtr)malloc(sizeof(struct FieldList));
            TypePtr tp = (TypePtr)malloc(sizeof(struct TypeItem_t));
            tp->name = "int";
            find_type(tp,&(*ret_val)->type);
            free(tp);
            (*ret_val)->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
            sprintf((*ret_val)->name,"%d",Exp->children[0]->data.int_value);
            (*ret_val)->alias = "int";
            (*ret_val)->val_ptr = (void*)&(Exp->children[0]->data.int_value);
            return 1;
        }else if(!strcmp(Exp->children[0]->node_name,"FLOAT")){             // Exp : FLOAT
            (*ret_val) = (FieldListPtr)malloc(sizeof(struct FieldList));
            TypePtr tp = (TypePtr)malloc(sizeof(struct TypeItem_t));
            tp->name = "float";
            find_type(tp,&(*ret_val)->type);
            free(tp);
            (*ret_val)->name = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
            sprintf((*ret_val)->name,"%f",Exp->children[0]->data.float_value);
            (*ret_val)->alias = "float";
            (*ret_val)->val_ptr = (void*)&(Exp->children[0]->data.float_value);
            return 1;
        }
    }
    return 1;
}

int ST_Args(struct SyntaxTreeNode* Args, FieldListPtr* ret_args){                                   
    struct SyntaxTreeNode* pArgs = Args;
    /*if(!strcmp(Args->parent->node_name,"Args")){
        *ret_val = (ExpValPtr)malloc(sizeof(struct ExpVal));
    }*/
    FieldListPtr* val = ret_args;
    //ExpValPtr *exp;
    for(;pArgs->n_children == 3;pArgs = pArgs->children[2]){                // Args -> Exp COMMA Args
        if(ST_Exp(pArgs->children[0],val)){
            val = &((*val)->tail);
        }
    }
    if(pArgs->n_children == 1){                                              // Args -> Exp
        if(ST_Exp(pArgs->children[0],val)){
            (*val)->tail = NULL;
        }else{
            (*val) = NULL;
            return 0;
        }
    }
    return 1;
}


int ST_ASSIGNOP(struct SyntaxTreeNode* LVal,struct SyntaxTreeNode* RVal, FieldListPtr* ret_val){
    FieldListPtr lval, rval;
    if(ST_Exp(RVal,&rval)){
        
    }
    //  Legal lvalue:
    //  variable: Exp -> ID
    //  array:    Exp -> Exp LB Exp RB
    //  field:    Exp -> Exp DOT ID
    int var_cond,arr_cond,field_cond;
    if(ST_Exp(LVal,&lval)){
        var_cond   = (LVal->n_children == 1) && (!strcmp(LVal->children[0]->node_name,"ID"));
        arr_cond   = (LVal->n_children == 4) && (!strcmp(LVal->children[1]->node_name,"LB"));
        field_cond = (LVal->n_children == 3) && (!strcmp(LVal->children[1]->node_name,"DOT"));

        if(var_cond || arr_cond || field_cond){
            if(rval){
                if(!isTypeEqual(lval->type,rval->type)){
                    printf("Error type %d at line %d: Type mismatched for assignment\n",MISMATCHED_ASSIGNMENT,LVal->lineno);
                }else{
                    if(lval->type->kind == BASIC){
                        if(lval->type->info.basic != rval->type->info.basic){
                            printf("Error type %d at line %d: Type mismatched for assignment\n",MISMATCHED_ASSIGNMENT,LVal->lineno);
                        }
                    }
                    // generate the intercodes for assignment
                    translate_assign(lval,rval);
                }
                (*ret_val) = lval;
            }else{
                (*ret_val) = lval;
            }
        }else{
            printf("Error type %d at line %d: The left-hand side of an assignment must be a variable\n",ERR_LEFT_VAL,LVal->lineno);
            (*ret_val) = lval;
        }
    }
    return 1;
}


//  +---------+------------------+--------------------+---+------------------+
//  |elem_type|array[dimension k]|array[dimension k-1]|...|array[dimension 1]|
//  +---------+------------------+--------------------+---+------------------+ 
int ST_Array(struct SyntaxTreeNode* ArrBase, struct SyntaxTreeNode* ArrIndex, FieldListPtr* ret_val){
    FieldListPtr arr_index, arr_base;
    FieldListPtr  ptail;
    static FieldListPtr pfield;
    static int arr_cnt = 0;
    arr_cnt++;

    VarTablePtr arr;
    if(ST_Exp(ArrBase,&arr_base)){
        // ArrBase -> ID
        if(ArrBase->n_children == 1 && !strcmp(ArrBase->children[0]->node_name,"ID")){
            find_var(arr_base->name,&arr);
            if(arr->type->kind == ARRAY){
                //free(arr_base);
                pfield = arr_base;
                arr_base = (FieldListPtr)malloc(sizeof(struct FieldList));
                TypePtr ptype = arr->type;
                for(;ptype && ptype->kind == ARRAY;ptype = ptype->info.array.elem);
                arr_base->type = ptype;
                arr_base->name = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
                strcpy(arr_base->name,arr->name);
                // the base address of array
                arr_base->alias = arr->alias;
                ptail = arr_base;
                (*ret_val) = arr_base;
            }else{
                printf("Error type %d at line %d: \"%s\" is not an array\n",NOT_ARRAY,ArrBase->lineno,arr_base->name);
                (*ret_val) = arr_base;
                (*ret_val)->val_ptr = NULL;
                return 0;
            }// ArrBase -> Exp DOT ID
        }else if(ArrBase->n_children == 3 && !strcmp(ArrBase->children[2]->node_name,"ID")){         
            pfield = arr_base;
            arr_base = (FieldListPtr)malloc(sizeof(struct FieldList));
            TypePtr ptype = pfield->type;
            for(;ptype && ptype->kind == ARRAY;ptype = ptype->info.array.elem);
            arr_base->type = ptype;
            arr_base->name = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
            strcpy(arr_base->name,pfield->name);
            // the base address of array
            arr_base->alias = pfield->alias;
            ptail = arr_base;
            (*ret_val) = arr_base;
        }else{
            //get the tail of the array list
            ptail = arr_base;
            for(;ptail->tail;ptail = ptail->tail);
            (*ret_val) = arr_base;
        }
    }
    if(ST_Exp(ArrIndex,&arr_index)){
        if(arr_index->type->kind == BASIC && arr_index->type->info.basic == BASIC_INT){     // index of array is int (correct)
            ptail->tail = arr_index;
            arr_index->tail = NULL;
        }else{  // error hint
            if(arr_index->name)
                printf("Error type %d at line %d: Array subscript \"[%s]\" is not an integer \n",NOT_INT,ArrIndex->lineno,arr_index->name);
            else
                printf("Error type %d at line %d: Array subscript \"[%s]\" is not an integer \n",NOT_INT,ArrIndex->lineno,"Exp");
            arr_index->tail = NULL;
        }
    }
    // generate the intercodes for array
    // Does the production stem from the other Exp ?
    //|| ArrBase->parent->parent->n_children != 4 || (strcmp(ArrBase->parent->parent->children[1]->node_name,"LB"))
    arr_cnt--;
    if(arr_cnt == 0){
        if(arr_base)
            translate_arr(ArrBase,&pfield,ret_val);
    }
    return 1;
}

int ST_StructField(struct SyntaxTreeNode* Exp, struct SyntaxTreeNode* ID, FieldListPtr* ret_val){
    if(ST_Exp(Exp,ret_val)){
        if((*ret_val)->type->kind == STRUCTURE){
            char* id_name = ID->data.string_value;
            FieldListPtr pStruct;
            //find the field & get the offset
            int field_offset = 0;
            pStruct = (*ret_val)->type->info.structure.elem;
            for(;pStruct;pStruct = pStruct->tail){
                if(!strcmp(pStruct->name,id_name)){
                    break;
                }
                switch(pStruct->type->kind){
                    case BASIC:
                        field_offset += 4;
                        break;
                    case ARRAY:
                        field_offset += (pStruct->type->info.array.size * pStruct->type->info.array.index);
                        break;
                    case STRUCTURE:
                        field_offset += (pStruct->type->info.structure.size);
                        break;
                    default : break;
                }
            }
            if(!pStruct){                                            // find faield
                printf("Error type %d at line %d: Struct %s has no field \"%s\"\n",NOT_MEMBER,Exp->lineno,(*ret_val)->name,id_name);
                // what if ret_val is a NULL 
                char* fieldname = (char*)malloc(TYPE_NAME_LEN*sizeof(char));
                sprintf(fieldname,"%s.%s",(*ret_val)->name,id_name);
                (*ret_val)->name = fieldname;
                (*ret_val)->type->kind = NOTYPE;
                (*ret_val)->val_ptr = NULL;
                return 0;
            }else{                                                  // find successfully
                FieldListPtr id;
                id = (FieldListPtr)malloc(sizeof(struct FieldList));
                id->type = (TypePtr)malloc(sizeof(struct TypeItem_t));
                memcpy(id->type,pStruct->type,sizeof(struct TypeItem_t));
                id->name = id_name;
                id->val_ptr = NULL;
                
                // generate the intercodes for the field of structure
                translate_structfield(ret_val,&id,field_offset);
                (*ret_val) = id;
                return 1;
            }
        }else{
            if((*ret_val)->name)
                printf("Error type %d at line %d: Expression \"%s\" must have a structure \n",NOT_STURCT,Exp->lineno,(*ret_val)->name);
            else
                printf("Error type %d at line %d: Expression \"%s\" must have a structure \n",NOT_STURCT,Exp->lineno,"Exp");
            (*ret_val)->val_ptr = NULL;
            return 0;
        }
    }
    return 1;
}

//  +---------+------+------+---+------+
//  |func_name|args 0|args 1|...|args n|
//  +---------+------+------+---+------+
int ST_CallFunc(struct SyntaxTreeNode* func_id, struct SyntaxTreeNode* args, FieldListPtr* ret_val){
    (*ret_val) = (FieldListPtr)malloc(sizeof(struct FieldList));
    FuncTablePtr func;

    // attempt to use a variable as a func
    VarTablePtr var;
    find_var(func_id->data.string_value,&var);
    if(var){
        printf("Error type %d at line %d: \"%s\" is not a function\n",NOT_FUNC,func_id->lineno,func_id->data.string_value);
        (*ret_val)->type = var->type;
        (*ret_val)->name = func_id->data.string_value;
        return 0;
    }
    find_func(func_id->data.string_value, &func);
    if(!func || !(func->define & FUNC_DEFINED)){
        printf("Error type %d at line %d: Undefined Function \"%s\"\n",UNDEFINED_FUNC,func_id->lineno,func_id->data.string_value);
        if(func){
            (*ret_val)->type = func->ret_type;
            (*ret_val)->name = func->name;
        }else{
            (*ret_val)->type = (TypePtr)malloc(sizeof(struct TypeItem_t));
            (*ret_val)->type->kind = NOTYPE;
            (*ret_val)->name = func_id->data.string_value;
        }
        (*ret_val)->val_ptr = NULL;
        return 0;
    }

    (*ret_val)->type = func->ret_type;
    (*ret_val)->name = func->name;

    //  Exp -> ID LP RP
    //  Function "func(int)" is not applicable for arguments"(int, int)"
    if(!args){                              
        if(func->n_param != 0){
            printf("Error type %d at line %d: Function \"%s(",ERR_FUNC_ARGS,func_id->lineno,func->name);
            print_param(func->param_list);
            printf(")\" is not applicable for arguments \"%s()",func_id->data.string_value);
            (*ret_val)->val_ptr = NULL;
            return 0;
        }else{
            // generate intercodes for call of function
            translate_func_call(func,*ret_val);
            return 1;
        }
    }
    // Exp -> ID LP Args RP
    FieldListPtr ref_args;
    if(ST_Args(args,&ref_args)){
        FuncTablePtr ref_func = (FuncTablePtr)malloc(sizeof(struct FuncTable_t));
        FieldListPtr pargs = ref_args;
        ref_func->ret_type = func->ret_type;
        ref_func->name = func->name;
        ref_func->n_param = 0;
        for(;pargs;pargs = pargs->tail) ref_func->n_param++;
        ref_func->param_list = ref_args;
        
        if(isFuncEqual(func,ref_func)){
            //  generate intercodes for call func
            //  +---------+------+------+---+------+
            //  |func_name|args 0|args 1|...|args n|
            //  +---------+------+------+---+------+
            (*ret_val)->tail = ref_args;
            translate_func_call(func,*ret_val);
        }else{
            printf("Error type %d at line %d: Function \"%s(",ERR_FUNC_ARGS,func_id->lineno,func->name);
            print_param(func->param_list);
            printf(")\" is not applicable for arguments \"%s(",func_id->data.string_value);
            print_param(ref_args);
            printf(")\"\n");
            (*ret_val)->val_ptr = NULL;
            return 0;
        }
    }else{
        (*ret_val)->val_ptr = NULL;
        return 0;
    }
    return 1;
}

int ST_Arithmetic(struct SyntaxTreeNode* a, struct SyntaxTreeNode* operation, struct SyntaxTreeNode* b, FieldListPtr* ret_val){
    FieldListPtr opa, opb;
    ST_Exp(a,&opa);
    // be care for opa
    if(!opa)
        return 0;
    if(opa->type->kind != BASIC){
        if(opa->name)
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,a->lineno,opa->name);
        else
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,a->lineno,"Exp");
    }
    ST_Exp(b,&opb);
    if(!opb)
        return 0;
    if(opb->type->kind != BASIC){
        if(opb->name)
            printf("Error type %d at line %d: The expression \"%s\" is not a number\n",NOT_BASIC,b->lineno,opb->name);
        else
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,b->lineno,"Exp");
        (*ret_val) = opa;
        return 0;
    }
    if(opa->type->info.basic == opb->type->info.basic){    // Success
        (*ret_val) = opa;
        //opa +-*/ opb
        // generate the intercodes for arithmetic operation
        char op_name;
        switch(operation->node_name[0]){
            // "PLUS"
            case 'P': op_name = '+'; break;
            // "MINUS"
            case 'M': op_name = '-'; break;
            // "STAR"
            case 'S': op_name = '*'; break;
            // "DIV"
            case 'D': op_name = '/'; break;
            default: op_name = ' '; break; 
        }
        translate_arithmetic(opa,op_name,opb);
        return 1;
    }else{
        if(opb->name)
            printf("Error type %d at line %d: Type mismatiched for operation around \"%s\"\n",MISMATCHED_OPERAND,b->lineno,opb->name);
        else
            printf("Error type %d at line %d: Type mismatiched for operation around \"%s\"\n",MISMATCHED_OPERAND,b->lineno,"Exp");
        (*ret_val) = opa;
        return 0;
    }
}
// Exp -> Exp AND Exp
int ST_2OP_Logic(struct SyntaxTreeNode* a, struct SyntaxTreeNode* operation, struct SyntaxTreeNode* b, FieldListPtr* ret_val){
    FieldListPtr opa, opb;
    int and_cond, or_cond, bool_cond;
 
    and_cond = !strcmp(operation->node_name,"AND");
    or_cond = !strcmp(operation->node_name,"OR");
    bool_cond =  and_cond || or_cond;

    ST_Exp(a,&opa);
    if(!opa)
        return 0;
       
    if(opa->type->kind != BASIC){
        if(opa->name)
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,a->lineno,opa->name);
        else
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,a->lineno,"Exp");
    }
    ST_Exp(b,&opb);
    if(!opb)
        return 0;
    if(opb->type->kind != BASIC){
        if(opb->name)
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,b->lineno,opb->name);
        else
            printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,b->lineno,"Exp");
        (*ret_val) = opa;
        return 0;
    }
    if(bool_cond){  //opa && ||  opb
        if(opa->type->info.basic == BASIC_INT){                                     // Success
            if(opb->type->info.basic == BASIC_INT){
                (*ret_val) = opa;
               // generate intercodes
               translate_logic(a->parent,ret_val);
                return 1;
            }else{
                if(opb->name)
                    printf("Error type %d at line %d: The expression \"%s\" must be an integer \n",NOT_INT,b->lineno,opb->name);
                else
                    printf("Error type %d at line %d: The expression \"%s\" must be an integer \n",NOT_INT,b->lineno,"Exp");
                (*ret_val) = opa;
                return 0;
            }
        }else{
            if(opa->name)
                printf("Error type %d at line %d: The expression \"%s\" must be an integer \n",NOT_INT,a->lineno,opa->name);
            else
                printf("Error type %d at line %d: The expression \"%s\" must be an integer \n",NOT_INT,a->lineno,"Exp");
            (*ret_val) = opa;
            return 0;
        }
    }else{  //opa > < >= <= == != opb
        if(opa->type->info.basic == opb->type->info.basic){    // Success
            (*ret_val) = opa;
            // generate intercodes
            translate_logic(a->parent,ret_val);
            return 1;
        }else{
            if(opb->name)
                printf("Error type %d at line %d: Type mismatiched for relation operator around \"%s\"\n",MISMATCHED_RELOP,b->lineno,opb->name);
            else
                printf("Error type %d at line %d: Type mismatiched for relation operator around \"%s\"\n",MISMATCHED_RELOP,b->lineno,"Exp");
            (*ret_val) = opa;
            return 0;
        }
    }
    
}

int ST_1OP_Logic(struct SyntaxTreeNode* operation, struct SyntaxTreeNode* a, FieldListPtr* ret_val){
    if(!strcmp(operation->node_name,"MINUS")){                   // Exp : MINUS Exp
        //FieldListPtr minus_exp;
        if(ST_Exp(a,ret_val)){
            if((*ret_val)->type->kind == BASIC){
                // generate intercodes for minus exp
                // code = 0 - exp
                struct FieldList zero;
                int o = 0;
                zero.alias = "int";
                zero.val_ptr = (void*)(&o);
                translate_arithmetic(&zero,'-',(*ret_val));
                // return the result to ret_val
                (*ret_val)->alias = zero.alias;
            }else{
                printf("Error type %d at line %d: The Exp is not an integer or float \"%s\"\n",NOT_BASIC,a->lineno,a->node_name);
                (*ret_val)->val_ptr = NULL;
                return 0;
            }
        }
    }else if(!strcmp(operation->node_name,"NOT")){               // Exp : NOT Exp 
        if(ST_Exp(a,ret_val)){
            //(*ret_val) = not_exp;
            if((*ret_val)->type->kind == BASIC && (*ret_val)->type->info.basic == BASIC_INT){
                // generate intercodes for not operation
               translate_logic(a->parent,ret_val);
            }else{
                printf("Error type %d at line %d: The Exp is not an integer \"%s\"\n",NOT_INT,a->lineno,a->node_name);
                (*ret_val)->val_ptr = NULL;
                return 0;
            }
        }
    }
    return 1;
}