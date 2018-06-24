#include "main.h"
#define VAR_NAME_LEN 10
int local_cnt = 1;
int temp_cnt = 1;

// condition judgment
int label_cnt = 1;

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

int translate_func_dec(FuncTablePtr func){
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
            param_var->alias = (OperandPtr)malloc(sizeof(Operand));
            param_var->alias->kind = VARIABLE;
            param_var->alias->info.var_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
            sprintf(param_var->alias->info.var_name,"v%d",local_cnt++);
            param_code->code.info.param.x = param_var->alias->info.var_name;   //it must be a form like: letternum
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
        var->alias =  (OperandPtr)malloc(sizeof(Operand));
        var->alias->kind = VARIABLE;
        var->alias->info.var_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
        sprintf(var->alias->info.var_name,"g%d",nr_globalvar++);
        //create the allocation code
        if(var->type->kind == BASIC){
            
        }else if(var->type->kind == ARRAY){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            OperandPtr x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = var->alias->kind;
            x->info.var_name = var->alias->info.var_name;

            // compute the size of array
            // DEC tn size
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
            
            addr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            addr_code->code.info.assign.right->kind = ADDRESS;
            addr_code->code.info.assign.right->info.var_name = var->alias->info.var_name;

            var->alias->info.var_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
            sprintf(var->alias->info.var_name,"g%d",nr_globalvar++);
            addr_code->code.info.assign.left = var->alias;
            intercodes.push_back(&intercodes,addr_code);
        }else if(var->type->kind == STRUCTURE){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            
            OperandPtr x = var->alias;

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
            addr_code->code.info.assign.right->info.var_name = var->alias->info.var_name;
            intercodes.push_back(&intercodes,addr_code);
        }  
    }
    return 1;
}

int translate_localvar(char* var_name){
    VarTablePtr var;
    find_var(var_name,&var);
    if(var){
        var->alias = (OperandPtr)malloc(sizeof(Operand));
        var->alias->kind = VARIABLE;
        var->alias->info.var_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
        sprintf(var->alias->info.var_name,"v%d",local_cnt++);
        //create the allocation code
        if(var->type->kind == BASIC){
            
        }else if(var->type->kind == ARRAY){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            OperandPtr x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = var->alias->kind;
            x->info.var_name = var->alias->info.var_name;

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

            addr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            addr_code->code.info.assign.right->kind = ADDRESS;
            addr_code->code.info.assign.right->info.var_name = var->alias->info.var_name;

            var->alias->info.var_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
            sprintf(var->alias->info.var_name,"v%d",local_cnt++);
            addr_code->code.info.assign.left = var->alias;
            intercodes.push_back(&intercodes,addr_code);
        }else if(var->type->kind == STRUCTURE){
            InterCodeListNodePtr dec_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            
            OperandPtr x = var->alias;

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
            addr_code->code.info.assign.right->info.var_name = var->alias->info.var_name;
            intercodes.push_back(&intercodes,addr_code);
        }  
    }
    return 1;
}

int translate_arr(struct SyntaxTreeNode* ArrBase,FieldListPtr* arr,FieldListPtr* ref_arr_base){
    //int old_cnt = temp_cnt++;
    TypePtr parr;
    FieldListPtr pref = (*ref_arr_base)->tail;
    InterCodeListNodePtr arr_code,sum_code,index_code;

    // initialize operands
    OperandPtr result,op1,op2;
    OperandPtr sum;
    OperandPtr left;
    sum = (OperandPtr)malloc(sizeof(Operand));
    

    char* index_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
    sprintf(index_name,"t%d",temp_cnt++);
    char* sum_name = (char*)malloc(sizeof(char)*VAR_NAME_LEN);
    sprintf(sum_name,"t%d",temp_cnt++);
    
    // get the index to access the array
    int dim = 0;
    for(parr = (*arr)->type;parr->kind == ARRAY && pref;parr = parr->info.array.elem, pref = pref->tail){
        // complete the ref_index
        //op1 = (OperandPtr)malloc(sizeof(Operand));
        op1 = pref->alias;
        op2 = (OperandPtr)malloc(sizeof(Operand));

        // complete the arr_index
        op2->kind = CONSTANT_INT;
        op2->info.int_val = parr->info.array.index;
        
        sum_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));      
        if(dim == 0){  
            // generate code:  
            // sum = ref_index * arr_index (optimization)
            if(op1->kind == CONSTANT_INT){
                sum->kind = CONSTANT_INT;
                sum->info.int_val = op1->info.int_val * op2->info.int_val;
            }else{
                sum->kind = op1->kind;
                sum->info.var_name = op1->info.var_name;
            }
            // sum->info.var_name =  (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
            // sprintf(sum->info.var_name,"t%d",temp_cnt++);

            // sum_code->code.kind = MUL;
            // sum_code->code.info.binop.result = ret;
            // sum_code->code.info.binop.op1 = op1;
            // sum_code->code.info.binop.op2 = op2;
            // intercodes.push_back(&intercodes,sum_code);
            dim++;
        }else{  
            // generate code: 
            // index = ref_index * arr_index
            if(op1->kind == CONSTANT_INT){
                result = (OperandPtr)malloc(sizeof(Operand));
                result->kind = CONSTANT_INT;
                result->info.int_val = op1->info.int_val * op2->info.int_val;
            }else{
                result = (OperandPtr)malloc(sizeof(Operand));
                result->kind = VARIABLE;
                result->info.var_name = index_name; 
                index_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
                index_code->code.kind = MUL;
                index_code->code.info.binop.result = result;
                index_code->code.info.binop.op1 = op1;
                index_code->code.info.binop.op2 = op2;
                intercodes.push_back(&intercodes,index_code);
            }
            // generate code:
            // sum = sum + index  (tn = tn + tn+1)
            if(sum->kind == CONSTANT_INT){
                if(result->kind == CONSTANT_INT){
                    sum->info.int_val = sum->info.int_val + result->info.int_val;
                }else{
                    sum_code->code.kind = ADD;
                    sum_code->code.info.binop.op1 = sum;
                    sum_code->code.info.binop.op2 = result;

                    sum = (OperandPtr)malloc(sizeof(Operand));
                    sum->kind = VARIABLE;
                    sum->info.var_name = sum_name;
                    sum_code->code.info.binop.result = sum;
                    intercodes.push_back(&intercodes,sum_code);
                }
            }else{
                sum_code->code.kind = ADD;
                sum_code->code.info.binop.op1 = sum;
                sum_code->code.info.binop.op2 = result;

                if(sum->kind != VARIABLE){
                    sum = (OperandPtr)malloc(sizeof(OperandPtr));
                    sum->kind = VARIABLE;
                }
                sum->info.var_name = sum_name;
                sum_code->code.info.binop.result = sum;
                intercodes.push_back(&intercodes,sum_code);
            }
            dim++;
        }
    }

    // error hint
    if(parr->kind == ARRAY){
        printf("Error type %d at line %d: array type \"%s\" is not applicable for your reference\n",NOT_ARRAY,ArrBase->lineno,(*arr)->type->name);
        // return tn
        (*ref_arr_base)->alias = sum;
        //temp_cnt = old_cnt;
        return 0;
    }else{
        /* generate code:
           sum = sum * elem_size    (tn = tn * elem_size)
           sum = sum + arr_base     (tn = tn + arr_base)
           sum = *sum               (tn = *tn)
        */

       // get the elem_size
        int elem_size;
        if((*ref_arr_base)->type->kind == BASIC)
            elem_size = 4;
        else{   // structure
           elem_size = (*ref_arr_base)->type->info.structure.size;
        }

        // tn = tn * elem_size 
        if(sum->kind == CONSTANT_INT){
            sum->info.int_val = sum->info.int_val * elem_size;
        }else{
            sum_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));     
            sum_code->code.kind = MUL;
   
            sum_code->code.info.binop.op1 = sum;

            op2 = (OperandPtr)malloc(sizeof(Operand));
            op2->kind = CONSTANT_INT;
            op2->info.int_val = elem_size;
            sum_code->code.info.binop.op2 = op2;

            sum = (OperandPtr)malloc(sizeof(Operand));
            sum->kind = VARIABLE;
            sum->info.var_name = sum_name;
            sum_code->code.info.binop.result = sum;

            intercodes.push_back(&intercodes,sum_code);
        }
        

        // tn = tn + arr_base
        if(sum->kind == CONSTANT_INT){
            arr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            if(sum->info.int_val){
                arr_code->code.kind = ADD;
                arr_code->code.info.binop.op1 = sum;

                op2 = (OperandPtr)malloc(sizeof(Operand));
                op2->kind = VARIABLE;
                op2->info.var_name = (*ref_arr_base)->alias->info.var_name;
                arr_code->code.info.binop.op2 = op2;

                sum = (OperandPtr)malloc(sizeof(Operand));
                sum->kind = VARIABLE;
                sum->info.var_name = sum_name;
                arr_code->code.info.binop.result = sum;

                intercodes.push_back(&intercodes,arr_code);
            }else{
                arr_code->code.kind = ASSIGN;
                op2 = (OperandPtr)malloc(sizeof(Operand));
                op2->kind = VARIABLE;
                op2->info.var_name = (*ref_arr_base)->alias->info.var_name;
                arr_code->code.info.assign.right = op2;

                sum->kind = VARIABLE;
                sum->info.var_name = sum_name;
                arr_code->code.info.assign.left = sum;
                intercodes.push_back(&intercodes,arr_code);
            }
        }else{
            arr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            arr_code->code.kind = ADD;
            arr_code->code.info.binop.op1 = sum;

            op2 = (OperandPtr)malloc(sizeof(Operand));
            op2->kind = VARIABLE;
            op2->info.var_name = (*ref_arr_base)->alias->info.var_name;
            arr_code->code.info.binop.op2 = op2;

            sum = (OperandPtr)malloc(sizeof(Operand));
            sum->kind = VARIABLE;
            sum->info.var_name = sum_name;
            arr_code->code.info.binop.result = sum;
            intercodes.push_back(&intercodes,arr_code);
        }
        

        // generate following intercodes when the type of element is BASIC
        // tn = *tn
        if((*ref_arr_base)->type->kind == BASIC){
            // arr_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
            // left = (OperandPtr)malloc(sizeof(Operand));
            // arr_code->code.kind = ASSIGN;
            
            // left->kind = VARIABLE;
            // left->info.var_name = (char*)malloc(sizeof(char)*10);
            // sprintf(left->info.var_name,"t%d",++temp_cnt);

            // arr_code->code.info.assign.left = left;
            // arr_code->code.info.assign.right = (OperandPtr)malloc(sizeof(Operand));
            // arr_code->code.info.assign.right->kind = REFERENCE;
            // arr_code->code.info.assign.right->info.var_name = ret->info.var_name;
            // intercodes.push_back(&intercodes,arr_code);
            sum = (OperandPtr)malloc(sizeof(Operand));
            sum->kind = REFERENCE;
            sum->info.var_name = sum_name;
        }
    }
    // return tn
    (*ref_arr_base)->alias = sum;
    //temp_cnt = old_cnt+1;
    return 1;
}

int translate_structfield(FieldListPtr* struct_hdr,FieldListPtr* ref_field,int offset){
    //int old_cnt = temp_cnt++;
    // generate intercodes:
    // tn = hdr + offset
    // tn = *tn
    OperandPtr result,op1,op2;
    InterCodeListNodePtr field_code;
    // tn = hdr + offset
    if(offset){
        field_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        result = (OperandPtr)malloc(sizeof(Operand));
        //op1 = (OperandPtr)malloc(sizeof(Operand));
        op1 = (*struct_hdr)->alias;
        op2 = (OperandPtr)malloc(sizeof(Operand));

        result->kind = VARIABLE;
        result->info.var_name = (char*)malloc(sizeof(char)*10);
        sprintf(result->info.var_name,"t%d",temp_cnt++);

        // op1->kind = VARIABLE;
        // op1->info.var_name = (*struct_hdr)->alias;
    
        op2->kind = CONSTANT_INT;
        op2->info.int_val = offset;

        field_code->code.kind = ADD;
        field_code->code.info.binop.result = result;
        field_code->code.info.binop.op1 = op1;
        field_code->code.info.binop.op2 = op2;
        intercodes.push_back(&intercodes,field_code);
    }else{
        // tn = hdr
        // InterCodeListNodePtr field_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        // result = (OperandPtr)malloc(sizeof(Operand));
        // op1 = (OperandPtr)malloc(sizeof(Operand));

        // result->kind = VARIABLE;
        // result->info.var_name = (char*)malloc(sizeof(char)*10);
        // sprintf(result->info.var_name,"t%d",temp_cnt++);

        // op1->kind = VARIABLE;
        // op1->info.var_name = (*struct_hdr)->alias;

        // // optimization for assignment
        // // tn's name != hdr's name
        // if(strcmp(result->info.var_name,op1->info.var_name) || result->kind != op1->kind){
        //     field_code->code.kind = ASSIGN;
        //     field_code->code.info.assign.left = result;
        //     field_code->code.info.assign.right = op1;
        //     intercodes.push_back(&intercodes,field_code);
        // }else{
        //     free(field_code);
        //     free(op1);
        // }
    }

    // if the type of field is BASIC
    // tn = *tn
    if((*ref_field)->type->kind == BASIC){
        // InterCodeListNodePtr field_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
        // op1 = (OperandPtr)malloc(sizeof(Operand));
        // op1->kind = REFERENCE;
        // if(offset)
        //     op1->info.var_name = result->info.var_name;
        // else
        //     op1->info.var_name = (*struct_hdr)->alias;

        // result = (OperandPtr)malloc(sizeof(Operand));
        // result->kind = VARIABLE;
        // result->info.var_name = (char*)malloc(sizeof(char)*10);
        // sprintf(result->info.var_name,"t%d",temp_cnt++);

        // field_code->code.kind = ASSIGN;
        // field_code->code.info.assign.left = result;
        // field_code->code.info.assign.right = op1;
        // intercodes.push_back(&intercodes,field_code);
        result = (OperandPtr)malloc(sizeof(Operand));
        result->kind = REFERENCE;
        if(offset)
            result->info.var_name = field_code->code.info.binop.result->info.var_name;
        else
            result->info.var_name = (*struct_hdr)->alias->info.var_name;
    }

    // return tn
    (*ref_field)->alias = result;
    //temp_cnt = old_cnt + 1;

    return 1;
}

int translate_assign(FieldListPtr lval,FieldListPtr rval){
    // generate code for assignment:
    // lval = rval
    // VARIABLE , REFERENCE
    if(lval->alias->kind == rval->alias->kind){
        if(!strcmp(lval->alias->info.var_name,rval->alias->info.var_name)){
            return 1;
        }
    }
    InterCodeListNodePtr assign_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    OperandPtr left,right;
    // left = (OperandPtr)malloc(sizeof(Operand));
    // right = (OperandPtr)malloc(sizeof(Operand));

    left = lval->alias;
    right = rval->alias;

    assign_code->code.kind = ASSIGN;
    assign_code->code.info.assign.left = left;
    assign_code->code.info.assign.right = right;
    intercodes.push_back(&intercodes,assign_code);

    return 1;
}

int translate_arithmetic(FieldListPtr val1,char operation,FieldListPtr val2){
    int old_cnt = temp_cnt++;
    // generate intercodes for arithmetic operation
    InterCodeListNodePtr binop_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    OperandPtr result,op1,op2;
    result = (OperandPtr)malloc(sizeof(Operand));
    op1 = (OperandPtr)malloc(sizeof(Operand));
    op2 = (OperandPtr)malloc(sizeof(Operand));

    // get the result
    result->kind = VARIABLE;
    result->info.var_name = (char*)malloc(sizeof(char)*10);
    sprintf(result->info.var_name,"t%d",old_cnt);

    // get the op1
    if(val1->alias->kind == CONSTANT_INT){
        op1->kind = CONSTANT_INT;
        op1->info.int_val = val1->alias->info.int_val;
    }else if(val1->alias->kind == CONSTANT_FLOAT){
        op1->kind = CONSTANT_FLOAT;
        op1->info.float_val = val1->alias->info.float_val;
    }else{
        op1->kind = val1->alias->kind;
        op1->info.var_name = val1->alias->info.var_name;

        // pass the left-val to the end
        if(val1->alias->info.var_name[0] != 'v' && val1->alias->info.var_name[0] != 'g'){
            free(result->info.var_name);
            result->info.var_name = val1->alias->info.var_name;
        }
    }
    // get the op2
    if(val2->alias->kind == CONSTANT_INT){
        op2->kind = CONSTANT_INT;
        op2->info.int_val = val2->alias->info.int_val;
    }else if(val2->alias->kind == CONSTANT_FLOAT){
        op2->kind = CONSTANT_FLOAT;
        op2->info.float_val = val2->alias->info.float_val;
    }else{
        op2->kind = val2->alias->kind;
        op2->info.var_name = val2->alias->info.var_name;
    }

    // generate intercodes:
    // tn = val1 op val2
    switch(operation){
        case '+': binop_code->code.kind = ADD; break;
        case '-': binop_code->code.kind = SUB; break;
        case '*': binop_code->code.kind = MUL; break;
        case '/': binop_code->code.kind = DIVISION; break;
        default: break;
    }
    binop_code->code.info.binop.result = result;
    binop_code->code.info.binop.op1 = op1;
    binop_code->code.info.binop.op2 = op2;
    intercodes.push_back(&intercodes,binop_code); 

    // return tn
    val1->alias = result;

    temp_cnt = old_cnt + 1;

    return 1;
}

int translate_func_call(FuncTablePtr func_def,FieldListPtr func_call){
    int old_cnt = temp_cnt++;
    OperandPtr x;
    InterCodeListNodePtr call_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));

    if(func_def->n_param == 0){
        // Exp -> ID LP RP
 
        x = (OperandPtr)malloc(sizeof(Operand));
        x->kind = VARIABLE;
        x->info.var_name = (char*)malloc(sizeof(char)*10);
        sprintf(x->info.var_name,"t%d",old_cnt);

        if(!strcmp(func_call->name,"read")){
            call_code->code.kind = READ;
            call_code->code.info.read.x = x;
            intercodes.push_back(&intercodes,call_code);
            // return tn = READ()
            func_call->alias = x;
        }else{
            call_code->code.kind = CALL;
            call_code->code.info.call_func.x = x;
            call_code->code.info.call_func.func_name = func_call->name;
            intercodes.push_back(&intercodes,call_code);
            // return tn = func()
            func_call->alias = x;
        }
    }else{
        // Exp -> ID LP Args RP
        if(!strcmp(func_call->name,"write")){
            FieldListPtr pargs = func_call->tail;
            x = (OperandPtr)malloc(sizeof(Operand));
            if(pargs->alias->kind == CONSTANT_INT){
                x->kind = CONSTANT_INT;
                x->info.int_val = pargs->alias->info.int_val;
            }else if(pargs->alias->kind == CONSTANT_FLOAT){
                x->kind = CONSTANT_FLOAT;
                x->info.float_val = pargs->alias->info.float_val;
            }else{
                x->kind = pargs->alias->kind;
                x->info.var_name = pargs->alias->info.var_name;
            }
            call_code->code.kind = WRITE;
            call_code->code.info.write.x = x;
            intercodes.push_back(&intercodes,call_code);
            // ret
            func_call->alias = x;
        }else{
            // generate codes for args
            FieldListPtr pargs = func_call->tail;
            InterCodeListNodePtr arg_code;
            InterCodeListNodePtr back = intercodes.trailer;

            for(;pargs;pargs = pargs->tail){
                arg_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
               
                x = (OperandPtr)malloc(sizeof(Operand));
                if(pargs->alias->kind == CONSTANT_INT){
                    x->kind = CONSTANT_INT;
                    x->info.int_val = pargs->alias->info.int_val;
                }else if(pargs->alias->kind == CONSTANT_FLOAT){
                    x->kind = CONSTANT_FLOAT;
                    x->info.float_val = pargs->alias->info.float_val;
                }else{
                    x->kind = pargs->alias->kind;
                    x->info.var_name = pargs->alias->info.var_name;
                }

                arg_code->code.kind = ARG;
                arg_code->code.info.arg.x = x;
                //reverse the args
                intercodes.ins_yx(&intercodes,back,arg_code);
                back = arg_code;
            }

            // generate codes for call of func
            x = (OperandPtr)malloc(sizeof(Operand));
            x->kind = VARIABLE;
            x->info.var_name = (char*)malloc(sizeof(char)*10);
            sprintf(x->info.var_name,"t%d",old_cnt);

            call_code->code.kind = CALL;
            call_code->code.info.call_func.x = x;
            call_code->code.info.call_func.func_name = func_call->name;
            intercodes.push_back(&intercodes,call_code);
            // return tn = func()
            func_call->alias = x;
        }
    }
    temp_cnt = old_cnt + 1;
    return 1;
}

// Stmt
int translate_return(FieldListPtr ret){
    InterCodeListNodePtr ret_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    ret_code->code.kind = RET;
    ret_code->code.info.ret.x = (OperandPtr)malloc(sizeof(Operand));

    if(ret->alias->kind == CONSTANT_INT){
        ret_code->code.info.ret.x->kind = CONSTANT_INT;
        ret_code->code.info.ret.x->info.int_val = *(int*)(ret->val_ptr);
    }else if(ret->alias->kind == CONSTANT_FLOAT){
        ret_code->code.info.ret.x->kind = CONSTANT_FLOAT;
        ret_code->code.info.ret.x->info.float_val = *(float*)(ret->val_ptr);
    }else{
        ret_code->code.info.ret.x->kind = ret->alias->kind;
        ret_code->code.info.ret.x->info.var_name = ret->alias->info.var_name;
    }
    intercodes.push_back(&intercodes,ret_code);
    return 1;
}

int translate_2op_cond(FieldListPtr op1, char* op_name,FieldListPtr op2,uint32_t true_label,uint32_t false_label){
    OperandPtr x,y;
    x = (OperandPtr)malloc(sizeof(Operand));
    y = (OperandPtr)malloc(sizeof(Operand));
    fill_operand(op1,x);
    fill_operand(op2,y);
    
    // generate codes:
    // if op1 op_name op2 goto label_true
    InterCodeListNodePtr cond_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    cond_code->code.kind = COND;
    cond_code->code.info.cond.x = x;
    cond_code->code.info.cond.y = y;
    cond_code->code.info.cond.relop = op_name;
    cond_code->code.info.cond.true_label = true_label;
    // push_back the cond_code
    intercodes.push_back(&intercodes,cond_code);

    // generate code : 
    // goto false_label
    InterCodeListNodePtr false_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    false_code->code.kind = GOTO;
    false_code->code.info.goto_here.to = false_label;
    intercodes.push_back(&intercodes,false_code);
    
    return 1;
}

int translate_logic(struct SyntaxTreeNode* Exp,FieldListPtr* ret){
    uint32_t true_label,false_label;
    true_label = label_cnt++;
    false_label = label_cnt++;
    // (*ret_val)->alias = #0 
    translate_ret_ass_num(ret,0);
    // generate intercodes 
    translate_cond(Exp->parent,ret,true_label,false_label);
    // genrate code for label
    translate_label('t',true_label);
    // (*ret_val)->alias = #1
    translate_ret_ass_num(ret,1);
    translate_label('f',false_label);
    return 1;
}

int translate_ret_ass_num(FieldListPtr* ret,int num){
    InterCodeListNodePtr res_num_cond = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    OperandPtr x,n;
    x = (OperandPtr)malloc(sizeof(Operand));
    n = (OperandPtr)malloc(sizeof(Operand));

    x->kind = VARIABLE;
    if(x->kind != VARIABLE || (*ret)->alias->info.var_name[0] != 't'){
        x->info.var_name = (char*)malloc(sizeof(char)*10);
        sprintf(x->info.var_name,"t%d",temp_cnt++);
        // return the result
        (*ret)->alias->info.var_name = x->info.var_name;
    }else{
        x->info.var_name = (*ret)->alias->info.var_name;
    }

    n->kind = CONSTANT_INT;
    n->info.int_val = num;

    res_num_cond->code.kind = ASSIGN;
    res_num_cond->code.info.assign.left = x;
    res_num_cond->code.info.assign.right = n;
    intercodes.push_back(&intercodes,res_num_cond); 
   
    return 1;
}

int translate_1op_logic(char* op_name,FieldListPtr op1){

    return 1;
}

int translate_cond(struct SyntaxTreeNode* Exp,FieldListPtr* ret_val,uint32_t true_label,uint32_t false_label){
    int and_cond = 0,or_cond = 0,relop_cond = 0;
    and_cond = !strcmp(Exp->children[1]->node_name,"AND");

    if(Exp->n_children == 2 && !strcmp(Exp->children[0]->node_name,"NOT")){
        return translate_cond(Exp->children[1],ret_val,false_label,true_label);
    }

    if(and_cond){
       // generate code for and
        FieldListPtr opa, opb;
        uint32_t new_true = label_cnt++;
        translate_cond(Exp->children[0],&opa,new_true,false_label);
        translate_label('t',new_true);
        translate_cond(Exp->children[2],&opb,true_label,false_label);
        // check
        if(opa->type->kind != BASIC || opa->type->info.basic == BASIC_INT){
            if(opa->name)
                printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,Exp->children[0]->lineno,opa->name);
        }
        if(opb->type->kind != BASIC || opb->type->info.basic == BASIC_INT){
            if(opb->name)
                printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,Exp->children[2]->lineno,opb->name);
        }
        // ret_val
        (*ret_val) = opa;
        return 1;
    }else{
        or_cond = !strcmp(Exp->children[1]->node_name,"OR");
        if(or_cond){
            // generate code for or
            FieldListPtr opa, opb;
            uint32_t new_false = label_cnt++;
            translate_cond(Exp->children[0],&opa,true_label,new_false);
            translate_label('f',new_false);
            translate_cond(Exp->children[2],&opb,true_label,false_label);

            // check
            if(opa->type->kind != BASIC || opa->type->info.basic == BASIC_INT){
                if(opa->name)
                    printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,Exp->children[0]->lineno,opa->name);
            }
            if(opb->type->kind != BASIC || opb->type->info.basic == BASIC_INT){
                if(opb->name)
                    printf("Error type %d at line %d: The expression \"%s\" is not a number \n",NOT_BASIC,Exp->children[2]->lineno,opb->name);
            }
            // ret_val
            (*ret_val) = opa;
            return 1;
        }else{
            relop_cond = !strcmp(Exp->children[1]->node_name,"RELOP");
            if(relop_cond){
                // generate code for relop
                FieldListPtr opa,opb;
                // restore the true label and false label
                ST_Exp(Exp->children[0],&opa);
                ST_Exp(Exp->children[2],&opb);
                if(opa->type->info.basic == opb->type->info.basic){    // Success
                    // generate intercodes for relation operation
                    translate_2op_cond(opa,Exp->children[1]->data.string_value,opb,true_label,false_label);
                    (*ret_val) = opa;
                    return 1;
                 }else{
                    if(opb->name)
                        printf("Error type %d at line %d: Type mismatiched for relation operator around \"%s\"\n",MISMATCHED_RELOP,Exp->children[2]->lineno,opb->name);
                    (*ret_val) = opa;
                    return 0;
                }
            }else{
                // restore the true label and false label
                ST_Exp(Exp,ret_val);
                if((*ret_val)->alias){
                    translate_other_cond(*ret_val,true_label,false_label);
                }
            }
        }
    }
    return 1;
}

int translate_other_cond(FieldListPtr ret,uint32_t true_label,uint32_t false_label){
    InterCodeListNodePtr cond_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    OperandPtr x,y;
    x = (OperandPtr)malloc(sizeof(Operand));
    y = (OperandPtr)malloc(sizeof(Operand));

    fill_operand(ret,x);
    y->kind = CONSTANT_INT;
    y->info.int_val = 0;

    // if x != 0 goto true_label
    cond_code->code.kind = COND;
    cond_code->code.info.cond.x = x;
    cond_code->code.info.cond.y = y;
    cond_code->code.info.cond.relop = "!=";
    cond_code->code.info.cond.true_label = true_label;
    intercodes.push_back(&intercodes,cond_code);

    // goto false_label
    InterCodeListNodePtr false_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    false_code->code.kind = GOTO;
    false_code->code.info.goto_here.to = false_label;
    intercodes.push_back(&intercodes,false_code);
    
    return 1;
}

int translate_goto(uint32_t to){
    InterCodeListNodePtr goto_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    goto_code->code.kind = GOTO;
    goto_code->code.info.goto_here.to = to;
    intercodes.push_back(&intercodes,goto_code);
    return 1;
}

int translate_label(char tag,uint32_t label){
    InterCodeListNodePtr label_code = (InterCodeListNodePtr)malloc(sizeof(InterCodeListNode));
    switch(tag){
        case 't':
            label_code->code.info.label.label = label;  //label_info.true_cnt
            break;
        case 'f':
            label_code->code.info.label.label = label;  //label_info.false_cnt
            break;
        case 'n':
            label_code->code.info.label.label = label;  //label_info.next_cnt
            break;
        default:
            break;
    }
    label_code->code.kind = LABEL;
    intercodes.push_back(&intercodes,label_code);
    return 1;
}

int translate_exchange_label(char a,char b){
    // uint32_t temp_label, *a_label, *b_label;
    // InterCodeListNodePtr temp_node, *a_node, *b_node;
    // switch(a){
    //     case 't': a_label = &label_info.true_cnt;  a_node = &label_info.true_node;  break;
    //     case 'f': a_label = &label_info.false_cnt; a_node = &label_info.false_node; break;
    //     case 'n': a_label = &label_info.next_cnt;  a_node = &label_info.next_node;  break;
    //     default:
    //         return 0;
    // }
    // switch(b){
    //     case 't': b_label = &label_info.true_cnt;  b_node = &label_info.true_node;  break;
    //     case 'f': b_label = &label_info.false_cnt; b_node = &label_info.false_node; break;
    //     case 'n': b_label = &label_info.next_cnt;  b_node = &label_info.next_node;  break;
    //     default:
    //         return 0;
    // }
    // // exchange the label tag
    // temp_label = *a_label;
    // *a_label = *b_label;
    // *b_label = temp_label;

    // // exchange the label node
    // temp_node = *a_node;
    // *a_node = *b_node;
    // *b_node = temp_node;
    return 1;
}

int fill_operand(FieldListPtr field,OperandPtr operand){
    switch(field->alias->kind){
        case CONSTANT_INT:
            operand->kind = CONSTANT_INT;
            operand->info.int_val = field->alias->info.int_val;
            break;
        case CONSTANT_FLOAT:
            operand->kind = CONSTANT_FLOAT;
            operand->info.float_val = field->alias->info.float_val;
            break;
        default:
            operand->kind = field->alias->kind;
            operand->info.var_name = field->alias->info.var_name;
            break;
    }
    return 1;
}


int print_intercodes(FILE* out){
    InterCodeListNodePtr iter = intercodes.header->succ;
    for(;iter != intercodes.trailer; iter = iter->succ){
        switch(iter->code.kind){
            case ASSIGN:
                print_intercodes_assign(out,iter);
                break;
            case ADD:
            case SUB:
            case MUL:
            case DIVISION:
                print_intercodes_binop(out,iter);
                break;
            case LABEL:
                fprintf(out,"LABEL label%d :\n",iter->code.info.label.label);
                break;
            case FUNCTION:
                fprintf(out,"FUNCTION %s :\n",iter->code.info.func.func_name);
                break;
            case GOTO:
                fprintf(out,"GOTO label%d\n",iter->code.info.goto_here.to);
                break;
            case COND:
                fprintf(out,"IF %s %s %s GOTO label%d\n",sprint_operand(iter->code.info.cond.x),iter->code.info.cond.relop,sprint_operand(iter->code.info.cond.y),iter->code.info.cond.true_label);
                break;
            case RET:
                fprintf(out,"RETURN %s\n",sprint_operand(iter->code.info.ret.x));
                break;
            case DEC:
                fprintf(out,"DEC %s %d\n",sprint_operand(iter->code.info.dec.x),iter->code.info.dec.size);
                break;
            case ARG:
                fprintf(out,"ARG %s\n",sprint_operand(iter->code.info.arg.x));
                break;
            case CALL:
                fprintf(out,"%s := CALL %s\n",sprint_operand(iter->code.info.call_func.x),iter->code.info.call_func.func_name);
                break;
            case PARAM:
                fprintf(out,"PARAM %s\n",iter->code.info.param.x);
                break;
            case READ:
                fprintf(out,"READ %s\n",sprint_operand(iter->code.info.read.x));
                break;
            case WRITE:
                fprintf(out,"WRITE %s\n",sprint_operand(iter->code.info.read.x));
                break;
            default:
                break;
        }
    }
}

int print_intercodes_assign(FILE* out,InterCodeListNodePtr code){
    OperandPtr left,right;
    left = code->code.info.assign.left;
    right = code->code.info.assign.right;
    switch(left->kind){
        case VARIABLE:
            fprintf(out,"%s := %s\n",left->info.var_name,sprint_operand(right));
            break;
        case REFERENCE:
            fprintf(out,"*%s := %s\n",left->info.var_name,sprint_operand(right));
            break;
        default:
            fprintf(out,"Error type for left-val: %d!\n",left->kind);
    }
    return 1;
}

int print_intercodes_binop(FILE* out,InterCodeListNodePtr code){
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
        case DIVISION:
            op_name = "/";
            break;
        default:
            op_name = " ";
            break;
    }

    switch(result->kind){
        case VARIABLE:
            fprintf(out,"%s := %s %s %s\n",result->info.var_name,sprint_operand(op1),op_name,sprint_operand(op2));
            break;
        case REFERENCE:
            fprintf(out,"*%s := %s %s %s\n",result->info.var_name,sprint_operand(op1),op_name,sprint_operand(op2));
            break;
        default:
            break;
    }
    return 1;
}


char* sprint_operand(OperandPtr op){
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


/**
 *  @file   IntermediateCode.c
 *  @brief  Optimization for intermediate codes
 *  @author jim
 *  @date   2018-6-23
 *  @version: v1.0
**/ 
int optimization_intercodes(){
    optimization_cond();
    optimization_label();
    optimization_arithmetic();
    optimization_func();
    return 1;
}

int optimization_label(){
    if(label_cnt == 0)
        return 1;
    LabelInfo labels[label_cnt];
    int label_index,last_index;
    InterCodeListNodePtr pcode = intercodes.header->succ;
    // record the label
    for(;pcode != intercodes.trailer; pcode = pcode->succ){
        
        if(pcode->code.kind == LABEL){
            // complete the entry
            label_index = pcode->code.info.label.label;
            labels[label_index].order = label_index;
            labels[label_index].node = pcode;
            labels[label_index].namesake = label_index;
            labels[label_index].ano_node = pcode;
            // hit the redundant label
            if(pcode->prev->code.kind == LABEL){
                last_index = pcode->prev->code.info.label.label;
                labels[label_index].namesake = labels[last_index].namesake;
                labels[label_index].ano_node = labels[last_index].ano_node;
            }
        }
    }
    // optimization for redundant label
    for(pcode = intercodes.header->succ;pcode != intercodes.trailer; pcode = pcode->succ){
        if(pcode->code.kind == GOTO){
            label_index = pcode->code.info.goto_here.to;
            pcode->code.info.goto_here.to = labels[label_index].namesake;
        }else if(pcode->code.kind == COND){
            label_index = pcode->code.info.cond.true_label;
            pcode->code.info.cond.true_label = labels[label_index].namesake;
        }
    }
    // release the redundant label
    for(label_index = 1;label_index < label_cnt;label_index++){
        if(labels[label_index].order != labels[label_index].namesake)
            intercodes.del(&intercodes,labels[label_index].node);
    }
    return 1;
}

int optimization_cond(){
    uint32_t cond_label,goto_label,label_label;
    char* relop;
    InterCodeListNodePtr pcode = intercodes.header->succ;
    // optimization for redundant goto
    for(;pcode != intercodes.trailer; pcode = pcode->succ){
        if(pcode->code.kind == COND){
            if(pcode->succ->code.kind == GOTO && pcode->succ->succ->code.kind == LABEL){
                
                cond_label = pcode->code.info.cond.true_label;
                goto_label = pcode->succ->code.info.goto_here.to;
                label_label = pcode->succ->succ->code.info.label.label;

                if(cond_label == label_label){
                    // translate the condtion to false
                    relop = pcode->code.info.cond.relop;
                    // get reverser relop
                    if(!strcmp(relop,">")){
                        pcode->code.info.cond.relop = "<=";
                    }else if(!strcmp(relop,"<=")){
                        pcode->code.info.cond.relop = ">";
                    }else if(!strcmp(relop,"<")){
                        pcode->code.info.cond.relop = ">=";
                    }else if(!strcmp(relop,">=")){
                        pcode->code.info.cond.relop = "<";
                    }else if(!strcmp(relop,"==")){
                        pcode->code.info.cond.relop = "!=";
                    }else{
                        pcode->code.info.cond.relop = "==";
                    }
                    // assign the false label to condition code
                    pcode->code.info.cond.true_label = goto_label;
                    // release the goto code
                    intercodes.del(&intercodes,pcode->succ);
                }
            }
        }
    }
    return 1;
}

int optimization_arithmetic(){
    InterCodeListNodePtr pcode = intercodes.header->succ;
    OperandPtr res,op1,op2;
    char operation;
    // optimization for two immediate number operation
    for(;pcode != intercodes.trailer; pcode = pcode->succ){
        if(pcode->code.kind >= ADD && pcode->code.kind <= DIVISION){
            res = pcode->code.info.binop.result;
            op1 = pcode->code.info.binop.op1;
            op2 = pcode->code.info.binop.op2;
            if(op1->kind != op2->kind){
                switch(pcode->code.kind){
                    case ADD: operation = '+';break;
                    case SUB: operation = '-';break;
                    case MUL: operation = '*';break;
                    case DIVISION: operation = '/';break;
                    default: break;
                }
                if(op1->kind == CONSTANT_INT){
                    optimization_imm_operand(pcode,op1,operation,op2);
                }else if(op2->kind == CONSTANT_INT){
                    optimization_imm_operand(pcode,op2,operation,op1);
                }
            }else if(op1->kind == op2->kind){
                if(op1->kind == CONSTANT_INT){
                    switch(pcode->code.kind){
                        case ADD:
                            op1->info.int_val += op2->info.int_val;
                            break;
                        case SUB:
                            op1->info.int_val -= op2->info.int_val;
                            break;
                        case MUL:
                            op1->info.int_val *= op2->info.int_val;
                            break;
                        case DIVISION:
                            op1->info.int_val /= op2->info.int_val;
                            break;
                        default: break;
                    }
                    pcode->code.kind = ASSIGN;
                    pcode->code.info.assign.left = res;
                    pcode->code.info.assign.right = op1;
                }else if(op1->kind == CONSTANT_FLOAT){
                    switch(pcode->code.kind){
                        case ADD:
                            op1->info.float_val += op2->info.float_val;
                            break;
                        case SUB:
                            op1->info.float_val -= op2->info.float_val;
                            break;
                        case MUL:
                            op1->info.float_val *= op2->info.float_val;
                            break;
                        case DIVISION:
                            op1->info.float_val /= op2->info.float_val;
                            break;
                        default: break;
                    }
                    pcode->code.kind = ASSIGN;
                    pcode->code.info.assign.left = res;
                    pcode->code.info.assign.right = op1;
                }else
                    continue;
            }else
                continue;
        }
    }
    return 1;
}

// only optimize the immediate that is interger
int optimization_imm_operand(InterCodeListNodePtr code,OperandPtr imm,char operation,OperandPtr op){
    OperandPtr res;
    res = code->code.info.binop.result;
    switch(operation){
        case '+':
            // res = 0 + op
            if(imm->info.int_val == 0){
                code->code.kind = ASSIGN;
                code->code.info.assign.left = res;
                code->code.info.assign.right = op;
            }
            break;
        case '-':
            // res = op - 0
            if(imm == code->code.info.binop.op2){
                code->code.kind = ASSIGN;
                code->code.info.assign.left = res;
                code->code.info.assign.right = op;
            }
            break;
        case '*':
            // res = 0 * op
            if(imm->info.int_val == 0){
                code->code.kind = ASSIGN;
                code->code.info.assign.left = res;
                code->code.info.assign.right = imm;
            // res = 1 * op
            }else if(imm->info.int_val == 1){
                code->code.kind = ASSIGN;
                code->code.info.assign.left = res;
                code->code.info.assign.right = op;
            }
            break;
        case '/':
            // res = op / 1
            if(imm == code->code.info.binop.op2){
                code->code.kind = ASSIGN;
                code->code.info.assign.left = res;
                code->code.info.assign.right = op;
            }
            break;
        default:break;
    }
    return 1;
}
// left-val:
// assign:  left = right            (left)
// binop:   res = op1 operation op2 (res)
// x = call func:                   (x)
// read:    x = read                (x)

// right-val:
// assign:  left = right            (right)
// binop:   res = op1 operation op2 (op1,op2)
// ret:     ret x                   (x)
// write:   write x                 (x)
int optimization_func(){
    // optimization for copy propagation
    InterCodeListNodePtr pcode = intercodes.header->succ;
    for(;pcode != intercodes.trailer; pcode = pcode->succ){
        // READ t
        // x = t
        // after optimization:
        // READ x
        if(pcode->code.kind == READ){
            if(pcode->succ->code.kind == ASSIGN){
                pcode->code.info.read.x = pcode->succ->code.info.assign.left;
                intercodes.del(&intercodes,pcode->succ);
            }
            // x = right
            // WRITE x
            // after optimization:
            // WRITE right
        }else if(pcode->code.kind == WRITE){
            if(pcode->prev->code.kind == ASSIGN){
                OperandPtr prev_left,write_x;
                prev_left = pcode->prev->code.info.assign.left;
                write_x = pcode->code.info.write.x;
                if(prev_left->kind == write_x->kind){
                    if(!strcmp(prev_left->info.var_name,write_x->info.var_name)){
                    pcode->code.info.write.x = pcode->prev->code.info.assign.right;
                    intercodes.del(&intercodes,pcode->prev);
                    }
                }
            }
            // t = call func
            // x = t
            // after optimization:
            // x = call func
        }else if(pcode->code.kind == CALL){
            if(pcode->succ->code.kind == ASSIGN){
                OperandPtr succ_right,call_left;
                call_left = pcode->code.info.call_func.x;
                succ_right = pcode->succ->code.info.assign.right;
                if(succ_right->kind == call_left->kind){
                    if(!strcmp(call_left->info.var_name,succ_right->info.var_name)){
                    pcode->code.info.call_func.x = pcode->succ->code.info.assign.left;
                    intercodes.del(&intercodes,pcode->succ);
                    }
                }
            }
            // x = right
            // ret x
            // after optimization:
            // ret right
        }else if(pcode->code.kind == RET){
            if(pcode->prev->code.kind == ASSIGN){
                OperandPtr ret_x,prev_left;
                prev_left = pcode->prev->code.info.assign.left;
                ret_x = pcode->code.info.ret.x;
                if(ret_x->kind == prev_left->kind){
                    if(!strcmp(prev_left->info.var_name,ret_x->info.var_name)){
                    pcode->code.info.ret.x = pcode->prev->code.info.assign.right;
                    intercodes.del(&intercodes,pcode->prev);
                    }
                }
            }
        }
    }
    return 1;
}