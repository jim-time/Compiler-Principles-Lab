#include "main.h"
int local_cnt = 0;
int temp_cnt = 0;

struct List_t intercodes = {
    NULL,
    NULL,
    0,
    List_Create,
    List_Destroy,
    List_Ins_XY,
    List_Ins_YX,
    List_Del,
    List_PushBack,
    List_Pop
};

int translate_func(FuncTablePtr func){
    //func name
    InterCodeListNodePtr func_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    func_code->code.kind = FUNCTION;
    func_code->code.info.func.func_name = func->name;
    intercodes.push_back(&intercodes,func_code);
    //func parameter
    if(func->n_param){
        FieldListPtr pParam = func->param_list;
        VarTablePtr param_var;
        for(;pParam; pParam = pParam->tail){
            InterCodeListNodePtr param_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            param_code->code.kind = PARAM;

            //create the param codes
            find_var(pParam->name,&param_var);
            if(!param_var){
                printf("Not found the param in vartable\n");
                return 0;
            }
            param_var->alias = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
            sprintf(param_var->alias,"v%d",local_cnt++);
            param_code->code.info.param.x = param_var->alias;   //it must be a form like: letternum
            intercodes.push_back(&intercodes,param_code);

        }
    }
    return 1;
}

int translate_globalvar(char* var_name){
    VarTablePtr var;
    static int nr_globalvar = 0;
    find_var(var_name,&var);
    if(var){
        var->alias = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
        sprintf(var->alias,"g%d",nr_globalvar++);
        //create the allocation code
        if(var->type->kind == BASIC){
            
        }else if(var->type->kind == ARRAY){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            OperandPtr x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = VARIABLE;
            x->info.var_name = var->alias;

            // compute the size of array
            TypePtr parr = var->type;
            int arr_size = 1;
            for(;parr->kind == ARRAY; parr = parr->info.array.elem){
                arr_size *= parr->info.array.size;
            }
            if(parr->kind == BASIC)
                dec_code->code.info.dec.size = 4*arr_size;
            else if(parr->kind == STRUCTURE)
                dec_code->code.info.dec.size = parr->info.structure.size * arr_size;
            dec_code->code.kind = DEC;
            dec_code->code.info.dec.x = x;
            intercodes.push_back(&intercodes,dec_code);

            // generate intercodes to get address of array
            // tn = &tn
            InterCodeListNodePtr addr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            addr_code->code.kind = ASSIGN;
            addr_code->code.info.assign.left = x;
            addr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            addr_code->code.info.assign.right->kind = ADDRESS;
            addr_code->code.info.assign.right->info.var_name = var->alias;
            intercodes.push_back(&intercodes,addr_code);
        }else if(var->type->kind == STRUCTURE){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            
            OperandPtr x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = VARIABLE;
            x->info.var_name = var->alias;

            dec_code->code.kind = DEC;
            dec_code->code.info.dec.x = x;
            // get the size of array
            dec_code->code.info.dec.size = var->type->info.structure.size;
            intercodes.push_back(&intercodes,dec_code);

            // generate intercodes to get address of structure
            // tn = &tn
            InterCodeListNodePtr addr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            addr_code->code.kind = ASSIGN;
            addr_code->code.info.assign.left = x;
            addr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            addr_code->code.info.assign.right->kind = ADDRESS;
            addr_code->code.info.assign.right->info.var_name = var->alias;
            intercodes.push_back(&intercodes,addr_code);
        }  
    }
    return 1;
}

int translate_localvar(char* var_name){
    VarTablePtr var;
    static int nr_globalvar = 0;
    find_var(var_name,&var);
    if(var){
        var->alias = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
        sprintf(var->alias,"v%d",local_cnt++);
        //create the allocation code
        if(var->type->kind == BASIC){
            
        }else if(var->type->kind == ARRAY){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            OperandPtr x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = VARIABLE;
            x->info.var_name = var->alias;

            // compute the size of array
            TypePtr parr = var->type;
            int arr_size = 1;
            for(;parr->kind == ARRAY; parr = parr->info.array.elem){
                arr_size *= parr->info.array.size;
            }
            if(parr->kind == BASIC)
                dec_code->code.info.dec.size = 4*arr_size;
            else if(parr->kind == STRUCTURE)
                dec_code->code.info.dec.size = parr->info.structure.size * arr_size;
            dec_code->code.kind = DEC;
            dec_code->code.info.dec.x = x;
            intercodes.push_back(&intercodes,dec_code);

            // generate intercodes to get address of array
            // tn = &tn
            InterCodeListNodePtr addr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            addr_code->code.kind = ASSIGN;
            addr_code->code.info.assign.left = x;
            addr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            addr_code->code.info.assign.right->kind = ADDRESS;
            addr_code->code.info.assign.right->info.var_name = var->alias;
            intercodes.push_back(&intercodes,addr_code);
        }else if(var->type->kind == STRUCTURE){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            
            OperandPtr x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = VARIABLE;
            x->info.var_name = var->alias;

            dec_code->code.kind = DEC;
            dec_code->code.info.dec.x = x;
            // get the size of array
            dec_code->code.info.dec.size = var->type->info.structure.size;
            intercodes.push_back(&intercodes,dec_code);

            // generate intercodes to get address of structure
            // tn = &tn
            InterCodeListNodePtr addr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            addr_code->code.kind = ASSIGN;
            addr_code->code.info.assign.left = x;
            addr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            addr_code->code.info.assign.right->kind = ADDRESS;
            addr_code->code.info.assign.right->info.var_name = var->alias;
            intercodes.push_back(&intercodes,addr_code);
        }  
    }
    return 1;
}

int translate_ASSIGNOP(struct SyntaxTreeNode* Exp, void* place){


    return 1;
}

int translate_arr(struct SyntaxTreeNode* ArrBase,FieldListPtr* arr,FieldListPtr* ref_arr_base){
    int old_cnt = temp_cnt++;
    TypePtr parr;
    FieldListPtr pref = (*ref_arr_base)->tail;
    InterCodeListNodePtr arr_code,sum_code,index_code;

    // initialize operands
    OperandPtr ret,result,op1,op2;
    ret = (OperandPtr)malloc(sizeof(Operand));
    result = (OperandPtr)malloc(sizeof(Operand));


    // complete the result
    result->kind = VARIABLE;
    result->info.var_name = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
    sprintf(result->info.var_name,"t%d",temp_cnt++);
    // rename the ref_arr
    // int plen = 0;
    // (*ref_arr_base)->name = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
    // plen += sprintf((*ref_arr_base)->name,"%s",arr->name);

    // get the index to access the array
    int dim = 0;
    for(parr = (*arr)->type;parr->kind == ARRAY && pref;parr = parr->info.array.elem, pref = pref->tail){
        // complete the ref_index
        op1 = (OperandPtr)malloc(sizeof(Operand));
        op2 = (OperandPtr)malloc(sizeof(Operand));

        if(!strcmp(pref->alias,"int")){
            op1->kind = CONSTANT_INT;
            op1->info.int_val = *(int*)pref->val_ptr;
        }else{
            op1->kind = VARIABLE;
            op1->info.var_name = pref->alias;
        }
        // complete the arr_index
        op2->kind = CONSTANT_INT;
        op2->info.int_val = parr->info.array.index;

        sum_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));      
        if(dim == 0){  
            // generate code:  
            // sum = ref_index * arr_index (optimization)
            ret->kind = VARIABLE;
            ret->info.var_name =  (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
            sprintf(ret->info.var_name,"t%d",old_cnt);

            sum_code->code.kind = MUL;
            sum_code->code.info.binop.result = ret;
            sum_code->code.info.binop.op1 = op1;
            sum_code->code.info.binop.op2 = op2;
            intercodes.push_back(&intercodes,sum_code);
            dim++;
        }else{  
            // generate code: 
            // index = ref_index * arr_index
            index_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            index_code->code.kind = MUL;
            index_code->code.info.binop.result = result;
            index_code->code.info.binop.op1 = op1;
            index_code->code.info.binop.op2 = op2;
            intercodes.push_back(&intercodes,index_code);

            // generate code:
            // sum = sum + index  (tn = tn + tn+1)
            sum_code->code.kind = ADD;
            sum_code->code.info.binop.result = ret;
            sum_code->code.info.binop.op1 = ret;
            sum_code->code.info.binop.op2 = result;
            intercodes.push_back(&intercodes,sum_code);

            dim++;
        }
    }

    // error hint
    if(parr->kind == ARRAY){
        printf("Error type %d at line %d: array type \"%s\" is not applicable for your reference\n",NOT_ARRAY,ArrBase->lineno,(*arr)->type->name);
        // return tn
        (*ref_arr_base)->alias = ret->info.var_name;
        temp_cnt = old_cnt;
        return 0;
    }else{
        /* generate code:
           sum = sum * elem_size    (tn = tn * elem_size)
           sum = sum + arr_base     (tn = tn + arr_base)
           sum = *sum               (tn = *tn)
        */

        // tn = tn * elem_size 
        sum_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));     
        sum_code->code.kind = MUL;
        sum_code->code.info.binop.result = ret;
        sum_code->code.info.binop.op1 = ret;
        op2 = (OperandPtr)malloc(sizeof(Operand));
        op2->kind = CONSTANT_INT;
        if((*ref_arr_base)->type->kind == BASIC)
            op2->info.int_val = 4;
        else{   // structure
            op2->info.int_val = (*ref_arr_base)->type->info.structure.size;
        }
        sum_code->code.info.binop.op2 = op2;
        intercodes.push_back(&intercodes,sum_code);

        // tn = tn + arr_base
        arr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        arr_code->code.kind = ADD;
        arr_code->code.info.binop.result = ret;
        arr_code->code.info.binop.op1 = ret;
        op2 = (OperandPtr)malloc(sizeof(Operand));
        op2->kind = VARIABLE;
        op2->info.var_name = (*ref_arr_base)->alias;
        arr_code->code.info.binop.op2 = op2;
        intercodes.push_back(&intercodes,arr_code);

        // generate following intercodes when the type of element is BASIC
        // tn = *tn
        if((*ref_arr_base)->type->kind == BASIC){
            arr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            arr_code->code.kind = ASSIGN;
            arr_code->code.info.assign.left = ret;
            arr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            arr_code->code.info.assign.right->kind = REFERENCE;
            arr_code->code.info.assign.right->info.var_name = ret->info.var_name;
            intercodes.push_back(&intercodes,arr_code);
        }
    }
    // return tn
    (*ref_arr_base)->alias = ret->info.var_name;
    temp_cnt = old_cnt;
    return 1;
}

int translate_structfield(FieldListPtr* struct_hdr,FieldListPtr* ref_field,int offset){
    int old_cnt = temp_cnt++;
    // generate intercodes:
    // tn = hdr + offset
    // tn = *tn
    OperandPtr result,op1,op2;

    // tn = hdr + offset
    if(offset){
        InterCodeListNodePtr field_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        result = (OperandPtr)malloc(sizeof(Operand));
        op1 = (OperandPtr)malloc(sizeof(Operand));
        op2 = (OperandPtr)malloc(sizeof(Operand));

        result->kind = VARIABLE;
        result->info.var_name = (char*)malloc(sizeof(char)*10);
        sprintf(result->info.var_name,"t%d",old_cnt);

        op1->kind = VARIABLE;
        op1->info.var_name = (*struct_hdr)->alias;
    
        op2->kind = CONSTANT_INT;
        op2->info.int_val = offset;

        field_code->code.kind = ADD;
        field_code->code.info.binop.result = result;
        field_code->code.info.binop.op1 = op1;
        field_code->code.info.binop.op2 = op2;
        intercodes.push_back(&intercodes,field_code);
    }else{
        // tn = hdr
        InterCodeListNodePtr field_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        result = (OperandPtr)malloc(sizeof(Operand));
        op1 = (OperandPtr)malloc(sizeof(Operand));

        result->kind = VARIABLE;
        result->info.var_name = (char*)malloc(sizeof(char)*10);
        sprintf(result->info.var_name,"t%d",old_cnt);

        op1->kind = VARIABLE;
        op1->info.var_name = (*struct_hdr)->alias;

        // optimization for assignment
        // tn's name != hdr's name
        if(strcmp(result->info.var_name,op1->info.var_name) || result->kind != op1->kind){
            field_code->code.kind = ASSIGN;
            field_code->code.info.assign.left = result;
            field_code->code.info.assign.right = op1;
            intercodes.push_back(&intercodes,field_code);
        }else{
            free(field_code);
            free(op1);
        }
    }

    // if the type of field is BASIC
    // tn = *tn
    if((*ref_field)->type->kind == BASIC){
        InterCodeListNodePtr field_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        op1 = (OperandPtr)malloc(sizeof(Operand));
        op1->kind = REFERENCE;
        op1->info.var_name = result->info.var_name;

        field_code->code.kind = ASSIGN;
        field_code->code.info.assign.left = result;
        field_code->code.info.assign.right = op1;
        intercodes.push_back(&intercodes,field_code);
    }

    // return tn
    (*ref_field)->alias = result->info.var_name;
    return 1;
}

int print_intercodes(){
    InterCodeListNodePtr iter = intercodes.header->succ;
    for(;iter != intercodes.trailer; iter = iter->succ){
        switch(iter->code.kind){
            case ASSIGN:
                print_intercodes_assign(iter);
                break;
            case ADD:
            case SUB:
            case MUL:
            case DIV:
                print_intercodes_binop(iter);
                break;
            case LABEL:
                printf("LABEL %s :\n",iter->code.info.label.label_name);
                break;
            case FUNCTION:
                printf("FUNCTION %s :\n",iter->code.info.func.func_name);
                break;
            case GOTO:
                printf("GOTO %s\n",iter->code.info.goto_here.to);
                break;
            case COND:
                printf("IF %s %s %s GOTO %s\n",print_operand(iter->code.info.cond.x),iter->code.info.cond.relop,print_operand(iter->code.info.cond.y),iter->code.info.cond.to_z);
                break;
            case RET:
                printf("RETURN %s\n",print_operand(iter->code.info.ret.x));
                break;
            case DEC:
                printf("DEC %s %d\n",print_operand(iter->code.info.dec.x),iter->code.info.dec.size);
                break;
            case ARG:
                printf("ARG %s\n",print_operand(iter->code.info.arg.x));
                break;
            case CALL:
                printf("%s := CALL %s\n",print_operand(iter->code.info.call_func.x),iter->code.info.call_func.func_name);
                break;
            case PARAM:
                printf("PARAM %s\n",iter->code.info.param.x);
                break;
            case READ:
                printf("READ %s\n",print_operand(iter->code.info.read.x));
                break;
            case WRITE:
                printf("WRITE %s\n",print_operand(iter->code.info.read.x));
                break;
            default:
                break;
        }
    }
}

char* print_operand(OperandPtr op){
    char* op_name = (char*)malloc(sizeof(char)*10);
    switch(op->kind){
        case VARIABLE:
            sprintf(op_name,"%s",op->info.var_name);
            break;
        case CONSTANT_INT:
            sprintf(op_name,"#%d",op->info.int_val);
            break;
        case CONSTANT_FLOAT:
            sprintf(op_name,"#%f",op->info.float_val);
            break;
        case ADDRESS:
            sprintf(op_name,"&%s",op->info.var_name);
            break;
        case REFERENCE:
            sprintf(op_name,"*%s",op->info.var_name);
            break;
        default :
            sprintf(op_name," ");
            break;
    }
    return op_name;
}

int print_intercodes_assign(InterCodeListNodePtr code){
    OperandPtr left,right;
    left = code->code.info.assign.left;
    right = code->code.info.assign.right;
    switch(left->kind){
        case VARIABLE:
            printf("%s := %s\n",left->info.var_name,print_operand(right));
            break;
        case REFERENCE:
            printf("*%s := %s\n",left->info.var_name,print_operand(right));
            break;
        default:
            printf("Error type for left-val: %d!\n",left->kind);
    }
    return 1;
}

int print_intercodes_binop(InterCodeListNodePtr code){
    OperandPtr result,op1,op2;
    result = code->code.info.binop.result;
    op1 = code->code.info.binop.op1;
    op2 = code->code.info.binop.op2;

    char* out_buf = (char*)malloc(sizeof(char)*2*TYPE_NAME_LEN);
    char* op_name;
    int plen = 0;
    switch(code->code.kind){
        case ADD:
            op_name = "+";
            break;
        case SUB:
            op_name = "-";
            break;
        case MUL:
            op_name = "*";
            break;
        case DIV:
            op_name = "/";
            break;
        default:
            op_name = " ";
            break;
    }

    switch(result->kind){
        case VARIABLE:
            printf("%s := %s %s %s\n",result->info.var_name,print_operand(op1),op_name,print_operand(op2));
            break;
        case REFERENCE:
            printf("*%s := %s %s %s\n",result->info.var_name,print_operand(op1),op_name,print_operand(op2));
            break;
        default:
            break;
    }
    return 1;
}