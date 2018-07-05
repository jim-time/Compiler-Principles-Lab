#include "main.h"

#define __STR__(s)    #s
#define MACRO2STR(s) __STR__(s)

#define RW_CODE \
"read:\n\
li $v0, 4\n\
la $a0, _prompt\n\
syscall\n\
li $v0, 5\n\
syscall\n\
jr $ra\n\
\n\
write:\n\
li $v0, 1\n\
syscall\n\
li $v0, 4\n\
la $a0, _ret\n\
syscall\n\
move $v0, $0\n\
jr $ra\n"


char* mips_reg[NUM_MIPS_REG] = {"$r0","$at","$v0","$v1",
                                "$a0","$a1","$a2","$a3",
                                "$t0","$t1","$t2","$t3",
                                "$t4","$t5","$t6","$t7",
                                "$s0","$s1","$s2","$s3",
                                "$s4","$s5","$s6","$s7",
                                "$t8","$t9","$k0","$k1",
                                "$gp","$sp","$fp","$ra"
                                };

#define NUM_RESERVED 5
char* reserved_name[NUM_RESERVED] = {"add","sub","mul","div",
                                     "move"
                                    };

int CodeGenerator(FILE* out){
    InterCodeListNodePtr iter = intercodes.header->succ;
    CodeGen_Init(out);
    CreateBasicBlock();
    print_basicblock();
    for(;iter != intercodes.trailer; iter = iter->succ){
        switch(iter->code.kind){
            case ASSIGN:
                CodeGen_Assign(out,iter);
                break;
            case ADD:
            case SUB:
            case MUL:
            case DIVISION:
                CodeGen_BinOp(out,iter);
                break;
            case LABEL:
                fprintf(out,"L%d:\n",iter->code.info.label.label);
                break;
            case FUNCTION:
                CodeGen_DefFunc(out,iter);
                break;
            case GOTO:
                fprintf(out,"j L%d\n",iter->code.info.goto_here.to);
                break;
            case COND:
                CodeGen_Cond(out,iter);
                break;
            case RET:
                CodeGen_Ret(out,iter);
                break;
            case DEC:
                CodeGen_Dec(out,iter);
                break;
            case ARG:
                CodeGen_Args(out,iter);
                break;
            case CALL:
                CodeGen_CallFunc(out,iter);
                break;
            case PARAM:
                CodeGen_Param(out,iter);
                break;
            case READ:
                CodeGen_Read(out,iter);
                break;
            case WRITE:
                CodeGen_Write(out,iter);
                break;
            default:
                break;
        }
    }
    return 1;
}

int CodeGen_Init(FILE* out){
    // add write and read function codes
    fprintf(out,".data\n");
    fprintf(out,"_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(out,"_ret: .asciiz \"\\n\"\n");
    fprintf(out,".globl main\n");
    fprintf(out,".text\n");
    fprintf(out,RW_CODE);

    // reset the register descriptor and variable descriptor
    memset(reg,0,NUM_REG*sizeof(RegDT));
    int iter;
    for(iter = 0; iter < NUM_REG; iter++)
        reg[iter].valid = 1;
    return 1;
}

int CodeGen_Assign(FILE* out, InterCodeListNodePtr code){
    char* rd = NULL, *rs = NULL;
    OperandPtr left, right;
    left = code->code.info.assign.left;
    right = code->code.info.assign.right;
    if(right->kind == CONSTANT_INT){
        rd = getReg(out,left);
        fprintf(out,"li %s, %d\n",rd,right->info.int_val);
    }else if(right->kind == CONSTANT_FLOAT){

    }else if(right->kind == ADDRESS){
        //rigster the address of array or structure

    }else{
        if(left->kind == VARIABLE && right->kind == REFERENCE){
            rs = getReg(out,right);
            rd = getReg(out,left);
            fprintf(out,"lw %s, %s\n",rd,rs);
        }else if(left->kind == REFERENCE && right->kind == VARIABLE){
            rs = getReg(out,right);
            rd = getReg(out,left);
            fprintf(out,"sw %s, 0(%s)\n",rs,rd);
        }else{
            rs = getReg(out,right);
            rd = getReg(out,left);
            fprintf(out,"move %s, %s\n",rd,rs);
        }
    }

    if(!rd)
        free(rd);
    if(!rs)
        free(rs);
    return 1;
}

int CodeGen_BinOp(FILE* out, InterCodeListNodePtr code){
    char* rs = NULL, *rt = NULL, *rd = NULL;
    OperandPtr res,op1,op2;
    res = code->code.info.binop.result;
    op1 = code->code.info.binop.op1;
    op2 = code->code.info.binop.op2;

    switch(code->code.kind){
        case ADD:
            // immediate number
            if(op1->kind == CONSTANT_INT){
                rs = getReg(out,op2);
                rd = getReg(out,res);
                fprintf(out,"addi %s, %s, %d\n",rd,rs,op1->info.int_val);
            }else if(op2->kind == CONSTANT_INT){
                rd = getReg(out,res);
                rs = getReg(out,op1);
                fprintf(out,"addi %s, %s, %d\n",rd,rs,op2->info.int_val);
            }else{
                rs = getReg(out,op1);
                rt = getReg(out,op2);
                rd = getReg(out,res);
                fprintf(out,"add %s, %s, %s\n",rd,rs,rt);
            }
            break;
        case SUB:
            if(op1->kind == CONSTANT_INT){
                rs = getReg(out,op2);
                rd = getReg(out,res);
                fprintf(out,"addi %s, %s, -%d\n",rd,rs,op1->info.int_val);
            }else if(op2->kind == CONSTANT_INT){
                rs = getReg(out,op1);
                rd = getReg(out,res);
                fprintf(out,"addi %s, %s, -%d\n",rd,rs,op2->info.int_val);
            }else{
                rs = getReg(out,op1);
                rt = getReg(out,op2);
                rd = getReg(out,res);
                fprintf(out,"sub %s, %s, %s\n",rd,rs,rt);
            }
            break;
        case MUL:
            rs = getReg(out,op1);
            rt = getReg(out,op2);
            rd = getReg(out,res);
            fprintf(out,"mul %s, %s, %s\n",rd,rs,rt);
            break;
        case DIVISION:
        // LO = $rs / $rt
        // HI = $rs % $rt
        // move $rd, $LO
            rs = getReg(out,op1);
            rt = getReg(out,op2);
            rd = getReg(out,res);
            fprintf(out,"div %s, %s\n",rs,rt);
            fprintf(out,"mflo %s\n",rd);
            break;
        default :
            break;
    }

    // free memory
    if(!rs)
        free(rs);
    if(!rt)
        free(rt);
    if(!rd)
        free(rd);
    return 1;
}

int CodeGen_Cond(FILE* out, InterCodeListNodePtr code){
    char* relop = code->code.info.cond.relop;
    char* x = NULL, *y = NULL;
    x = getReg(out,code->code.info.cond.x);
    y = getReg(out,code->code.info.cond.y);

    // x != y
    switch(relop[0]){
        case '=':
            fprintf(out,"beq %s, %s, L%d\n",x,y,code->code.info.cond.true_label);
            break;
        case '!':
            fprintf(out,"bne %s, %s, L%d\n",x,y,code->code.info.cond.true_label);
            break;
        case '>':
            if(strlen(relop) == 1){
                fprintf(out,"bgt %s, %s, L%d\n",x,y,code->code.info.cond.true_label);
            }else{
                fprintf(out,"bge %s, %s, L%d\n",x,y,code->code.info.cond.true_label);
            }
            break;
        case '<':
            if(strlen(relop) == 1){
                fprintf(out,"blt %s, %s, L%d\n",x,y,code->code.info.cond.true_label);
            }else{
                fprintf(out,"ble %s, %s, L%d\n",x,y,code->code.info.cond.true_label);
            }
            break;
        default:
            break;
    }

    if(!x)
        free(x);
    if(!y)
        free(y);
    return 1;
}


char* CodeGen_CheckFunc(char* func_name){
    int iter = 0;
    char* ret_name = NULL;
    for(; iter < NUM_RESERVED; iter++){
        if(!strcmp(func_name,reserved_name[iter])){
            ret_name = (char*)malloc(sizeof(char)*TYPE_NAME_LEN);
            sprintf(ret_name,"_%s",func_name);
            break;
        }
    }
    if(!ret_name)
        ret_name = func_name;
    return ret_name;
}

int CodeGen_DefFunc(FILE* out, InterCodeListNodePtr code){
    fprintf(out,"\n%s:\n",CodeGen_CheckFunc(code->code.info.func.func_name));
    // save register
    PUSH(out,"$fp");
    // mov $fp, $sp
    fprintf(out,"move $fp, $sp\n");
    return 1;
}

int CodeGen_Param(FILE* out, InterCodeListNodePtr code){
    //fprintf(out,"PARAM %s\n",code->code.info.param.x);
    // register the parameter


    return 1;
}

int CodeGen_Dec(FILE* out, InterCodeListNodePtr code){
    // register the array and structure
    // register(code->code.info.dec.x)
    fprintf(out,"addi $sp, $sp, -%d\n",code->code.info.dec.size);
    // register($sp)
    return 1;
}

int CodeGen_Args(FILE* out, InterCodeListNodePtr code){
    static int arg_cnt = 0;
    char* arg_reg = getReg(out,code->code.info.arg.x);
    if(code->prev->code.kind != ARG){
        // write back active variable among t0 - t9
        arg_cnt = 0;
    }
    if(arg_cnt < 4){
        fprintf(out,"move %s, %s\n",mips_reg[4+arg_cnt],arg_reg);
    }else{
        PUSH(out,arg_reg);
    }
    arg_cnt++;
    return 1;
}

int CodeGen_CallFunc(FILE* out, InterCodeListNodePtr code){
    // push the return addr
    PUSH(out,"$ra");
    // call function
    fprintf(out,"jal %s\n",CodeGen_CheckFunc(code->code.info.call_func.func_name));
    // restore the return addr
    POP(out,"$ra");
    // get the return value
    if(code->code.info.call_func.x){
        char* ret_name = getReg(out,code->code.info.call_func.x);
        fprintf(out,"move %s, $v0\n",ret_name);
        if(!ret_name)
            free(ret_name);
    }
    return 1;
}

int CodeGen_Ret(FILE* out, InterCodeListNodePtr code){
    char* ret_reg = getReg(out,code->code.info.ret.x);
    // restore the register
    fprintf(out,"move $sp, $fp\n");
    POP(out,"$fp");
    fprintf(out,"move $v0, %s\njr $ra\n",ret_reg);
    if(!ret_reg)
        free(ret_reg);
    return 1;
}

int CodeGen_Read(FILE* out, InterCodeListNodePtr code){
    OperandPtr x = code->code.info.read.x;
    code->code.kind = CALL;
    code->code.info.call_func.x = x;
    code->code.info.call_func.func_name = "read";
    CodeGen_CallFunc(out,code);
    return 1;
}

int CodeGen_Write(FILE* out, InterCodeListNodePtr code){
    OperandPtr x = code->code.info.write.x;
    code->code.kind = ARG;
    code->code.info.arg.x = x; 
    CodeGen_Args(out,code);
    code->code.kind = CALL;
    code->code.info.call_func.func_name = "write";
    code->code.info.call_func.x = NULL;
    CodeGen_CallFunc(out,code);
    return 1;
}

