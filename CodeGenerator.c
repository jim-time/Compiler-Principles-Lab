#include "main.h"
#include "Bitmap.h"

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
    CodeGen_Init(out);
    CreateBasicBlock();
    getLiveVarInfo();
    print_basicblock();
    //CodeGen_Start(out);
    return 1;
}

int CodeGen_Start(FILE* out){
    int iterBlock,iterCode;
    InterCodeListNodePtr pcode;
    BasicBlockNodePtr pblock;
    for(iterBlock = 0; iterBlock < nr_blocks; iterBlock++){
        pblock = BasicBlocks[iterBlock]->header;
        for(;pblock != NULL ;pblock = pblock->next){
            for(pcode = pblock->code_begin,iterCode = 0; pcode->prev != pblock->code_end;pcode = pcode->succ,iterCode++){
                switch(pcode->code.kind){
                    case ASSIGN:
                        CodeGen_Assign(out,pcode,pblock->out[iterCode]);
                        break;
                    case ADD:
                    case SUB:
                    case MUL:
                    case DIVISION:
                        CodeGen_BinOp(out,pcode,pblock->out[iterCode]);
                        break;
                    case LABEL:
                        fprintf(out,"L%d:\n",pcode->code.info.label.label);
                        break;
                    case FUNCTION:
                        CodeGen_DefFunc(out,pcode,pblock->out[iterCode]);
                        break;
                    case GOTO:
                        fprintf(out,"j L%d\n",pcode->code.info.goto_here.to);
                        break;
                    case COND:
                        CodeGen_Cond(out,pcode,pblock->out[iterCode]);
                        break;
                    case RET:
                        CodeGen_Ret(out,pcode,pblock->out[iterCode]);
                        break;
                    case DEC:
                        CodeGen_Dec(out,pcode,pblock->out[iterCode]);
                        break;
                    case ARG:
                        CodeGen_Args(out,pcode,pblock->out[iterCode]);
                        break;
                    case CALL:
                        CodeGen_CallFunc(out,pcode,pblock->out[iterCode]);
                        break;
                    case PARAM:
                        CodeGen_Param(out,pcode,pblock->out[iterCode]);
                        break;
                    case READ:
                        CodeGen_Read(out,pcode,pblock->out[iterCode]);
                        break;
                    case WRITE:
                        CodeGen_Write(out,pcode,pblock->out[iterCode]);
                        break;
                    default:
                        break;
                }
            }
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

int CodeGen_Assign(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    struct OperandDT_t *left_entry, *right_entry;
    int rd = 0, rs = 0, base = 0;
    OperandPtr left, right;
    left = code->code.info.assign.left;
    right = code->code.info.assign.right;
    if(right->kind == CONSTANT_INT){
        // li x, #k
        if(left->kind == VARIABLE){
            rd = getReg(out,left,LEFT_VAL,liveVar);
            addOperand(left,&left_entry);
            fprintf(out,"li %s, %d\n", mips_reg[rd], right->info.int_val);
        }else if(left->kind == REFERENCE){
            // li y, #k
            rs = getReg(out,right,RIGHT_VAL,liveVar);
            // sw y, *x 
            left->kind = VARIABLE;
            addOperand(left,&left_entry);
            rd = getReg(out,left,LEFT_VAL,liveVar);
            if(left->info.var_name[0] == 't')
                fprintf(out,"sw %s,0(%s)\n",mips_reg[rs],mips_reg[rd]);
            else    
                fprintf(out,"sw %s,%d(%s)\n",mips_reg[rs],left_entry->offset,mips_reg[left_entry->base_reg]);
            // restore the info
            left->kind = REFERENCE;
        }
        // free left
        left_entry->dirty = 1;
        freeVar(out,left,liveVar);
        // free right
        if(rs){
            //release the reg manually
            reg[rs].valid = 1;
        }
    }else if(right->kind == CONSTANT_FLOAT){
        // li reg(left), #k

    }else if(right->kind == ADDRESS){
        //rigster the address of array or structure
        // vn = &_vn
        // addOperand(right,&right_entry);
        // right_entry->op->info.var_name = left->info.var_name;
        // right_entry->name = left->info.var_name;
    }else{
        if(left->kind == VARIABLE && right->kind == REFERENCE){
            // x = *y
            // get base (tn/*vn)
            right->kind = VARIABLE;
            base = getReg(out,right,RIGHT_VAL,liveVar);

            // get x
            rd = getReg(out,left,LEFT_VAL,liveVar);
            // *y -> *tn
            if(right->info.var_name[0] == 't'){
                fprintf(out,"lw %s, 0(%s)\n",mips_reg[rd],mips_reg[base]);
            }else{
                // *yn -> *vn
                addOperand(right,&right_entry);
                fprintf(out,"lw %s, %d(%s)\n",mips_reg[rd],right_entry->offset,mips_reg[right_entry->base_reg]);
            }         

            // free *y
            freeVar(out,right,liveVar);
            // restore the intercodes info
            right->kind = REFERENCE;
            // free x
            addOperand(left,&left_entry);
            left_entry->dirty = 1;
            freeVar(out,left,liveVar);
        }else if(left->kind == REFERENCE && right->kind == VARIABLE){
            // *x = y
            // get y
            rs = getReg(out,right,RIGHT_VAL,liveVar);
            // get base
            left->kind = VARIABLE;
            addOperand(left,&left_entry);
            base = getReg(out,left,LEFT_VAL,liveVar);
            // store *t
            if(left->info.var_name[0] == 't'){
                fprintf(out,"sw %s, 0(%s)\n",mips_reg[rs],mips_reg[base]);
            }else{
                // store *v
                fprintf(out,"sw %s, %d(%s)\n",mips_reg[rs],left_entry->offset,mips_reg[left_entry->base_reg]);
            }
           
            // free *x
            freeVar(out,left,liveVar);
            // restore the intercodes info
            left->kind = REFERENCE;
            addOperand(left,&left_entry);
            left_entry->dirty = 1;
            // free y
            freeVar(out,right,liveVar);
        }else{
            // x = y
            rs = getReg(out,right,RIGHT_VAL,liveVar);
            rd = getReg(out,left,LEFT_VAL,liveVar);
            fprintf(out,"move %s, %s\n",mips_reg[rd],mips_reg[rs]);
            // free x 
            addOperand(left,&left_entry);
            left_entry->dirty = 1;
            freeVar(out,left,liveVar);
            // free y
            freeVar(out,right,liveVar);
        }
    }
    return 1;
}

int CodeGen_BinOp(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    struct OperandDT_t *left_entry, *right_entry;
    int base;
    int rs = 0, rt = 0, rd = 0;
    OperandPtr res,op1,op2;
    res = code->code.info.binop.result;
    op1 = code->code.info.binop.op1;
    op2 = code->code.info.binop.op2;

    switch(code->code.kind){
        case ADD:
            // immediate number
            if(op1->kind == CONSTANT_INT){
                rs = getReg(out,op2,RIGHT_VAL,liveVar);
                rd = getReg(out,res,LEFT_VAL,liveVar);
                fprintf(out,"addi %s, %s, %d\n",mips_reg[rd],mips_reg[rs],op1->info.int_val);

                // free op2
                freeVar(out,op2,liveVar);
            }else if(op2->kind == CONSTANT_INT){
                rd = getReg(out,res,LEFT_VAL,liveVar);
                rs = getReg(out,op1,RIGHT_VAL,liveVar);
                fprintf(out,"addi %s, %s, %d\n",mips_reg[rd],mips_reg[rs],op2->info.int_val);

                // free op1
                freeVar(out,op1,liveVar);
            }else{
                rs = getReg(out,op1,RIGHT_VAL,liveVar);
                rt = getReg(out,op2,RIGHT_VAL,liveVar);
                rd = getReg(out,res,LEFT_VAL,liveVar);
                fprintf(out,"add %s, %s, %s\n",mips_reg[rd],mips_reg[rs],mips_reg[rt]);

                // free op1
                freeVar(out,op1,liveVar);
                // free op2
                freeVar(out,op2,liveVar);
            }
            // free res
            //check the res
            if(res->kind == REFERENCE){
                //write back
                // get the base reg
                if(res->info.var_name[0] == 't'){
                    res->kind = VARIABLE;
                    addOperand(res,&left_entry);
                    base = getReg(out,res,LEFT_VAL,liveVar);
                    fprintf(out,"sw %s, 0(%s)\n",mips_reg[rd],mips_reg[base]);
                    res->kind = REFERENCE;                    
                }else{
                    res->kind = VARIABLE;
                    addOperand(res,&left_entry);
                    fprintf(out,"sw %s, %d(%s)\n",mips_reg[rd],left_entry->offset,mips_reg[left_entry->base_reg]);
                    res->kind = REFERENCE;
                }
                // release reg manually
                //reg[rd].valid = 1;
            }
            addOperand(res,&left_entry);
            left_entry->dirty = 1;
            freeVar(out,res,liveVar);
            break;
        case SUB:
            if(op1->kind == CONSTANT_INT){
                rs = getReg(out,op2,RIGHT_VAL,liveVar);
                rd = getReg(out,res,LEFT_VAL,liveVar);
                fprintf(out,"addi %s, %s, -%d\n",mips_reg[rd],mips_reg[rs],op1->info.int_val);

                // free op2
                freeVar(out,op2,liveVar);
            }else if(op2->kind == CONSTANT_INT){
                rs = getReg(out,op1,RIGHT_VAL,liveVar);
                rd = getReg(out,res,LEFT_VAL,liveVar);
                fprintf(out,"addi %s, %s, -%d\n",mips_reg[rd],mips_reg[rs],op2->info.int_val);

                // free op1
                freeVar(out,op1,liveVar);
            }else{  
                rs = getReg(out,op1,RIGHT_VAL,liveVar);
                rt = getReg(out,op2,RIGHT_VAL,liveVar);
                rd = getReg(out,res,LEFT_VAL,liveVar);
                fprintf(out,"sub %s, %s, %s\n",mips_reg[rd],mips_reg[rs],mips_reg[rt]);

                // free op1
                freeVar(out,op1,liveVar);
                // free op2
                freeVar(out,op2,liveVar);
            }

            // free res
            //check the res
            if(res->kind == REFERENCE){
                //write back
                // get the base reg
                if(res->info.var_name[0] == 't'){
                    res->kind = VARIABLE;
                    addOperand(res,&left_entry);
                    base = getReg(out,res,LEFT_VAL,liveVar);
                    fprintf(out,"sw %s, 0(%s)\n",mips_reg[rd],mips_reg[base]);
                    res->kind = REFERENCE;
                }else{
                    res->kind = VARIABLE;
                    addOperand(res,&left_entry);
                    fprintf(out,"sw %s, %d(%s)\n",mips_reg[rd],left_entry->offset,mips_reg[left_entry->base_reg]);
                    res->kind = REFERENCE;
                }
                // release reg manually
                //reg[rd].valid = 1;
            }
            addOperand(res,&left_entry);
            left_entry->dirty = 1;
            freeVar(out,res,liveVar);
            break;
        case MUL:
            rs = getReg(out,op1,RIGHT_VAL,liveVar);
            rt = getReg(out,op2,RIGHT_VAL,liveVar);
            rd = getReg(out,res,LEFT_VAL,liveVar);
            fprintf(out,"mul %s, %s, %s\n",mips_reg[rd],mips_reg[rs],mips_reg[rt]);

            addOperand(res,&left_entry);
            left_entry->dirty = 1;
            freeVar(out,op1,liveVar);
            freeVar(out,op2,liveVar);
            freeVar(out,res,liveVar);
            break;
        case DIVISION:
        // LO = $rs / $rt
        // HI = $rs % $rt
        // move $rd, $LO
            rs = getReg(out,op1,RIGHT_VAL,liveVar);
            rt = getReg(out,op2,RIGHT_VAL,liveVar);
            rd = getReg(out,res,LEFT_VAL,liveVar);
            fprintf(out,"div %s, %s\n",mips_reg[rs],mips_reg[rt]);
            fprintf(out,"mflo %s\n",mips_reg[rd]);

            addOperand(res,&left_entry);
            left_entry->dirty = 1;
            freeVar(out,op1,liveVar);
            freeVar(out,op2,liveVar);
            freeVar(out,res,liveVar);
            break;
        default :
            break;
    }
    return 1;
}

int CodeGen_Cond(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    char* relop = code->code.info.cond.relop;
    OperandPtr opx, opy;
    opx = code->code.info.cond.x;
    opy = code->code.info.cond.y;
    int x = 0, y = 0;
    x = getReg(out,opx,RIGHT_VAL,liveVar);
    y = getReg(out,opy,RIGHT_VAL,liveVar);

    // x != y
    switch(relop[0]){
        case '=':
            fprintf(out,"beq %s, %s, L%d\n",mips_reg[x],mips_reg[y],code->code.info.cond.true_label);
            break;
        case '!':
            fprintf(out,"bne %s, %s, L%d\n",mips_reg[x],mips_reg[y],code->code.info.cond.true_label);
            break;
        case '>':
            if(strlen(relop) == 1){
                fprintf(out,"bgt %s, %s, L%d\n",mips_reg[x],mips_reg[y],code->code.info.cond.true_label);
            }else{
                fprintf(out,"bge %s, %s, L%d\n",mips_reg[x],mips_reg[y],code->code.info.cond.true_label);
            }
            break;
        case '<':
            if(strlen(relop) == 1){
                fprintf(out,"blt %s, %s, L%d\n",mips_reg[x],mips_reg[y],code->code.info.cond.true_label);
            }else{
                fprintf(out,"ble %s, %s, L%d\n",mips_reg[x],mips_reg[y],code->code.info.cond.true_label);
            }
            break;
        default:
            break;
    }

    freeVar(out,opx,liveVar);
    freeVar(out,opy,liveVar);
    return 1;
}



int CodeGen_DefFunc(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    // clear the arg_cnt
    if(code->succ->code.kind != PARAM)
        param_index = 0;
    // reset the reg_cnt
    reg_cnt = -1;
    // clear all the reg
    int iterReg = 0;
    for(;iterReg < 32;iterReg++){
        if(reg[iterReg].var){
            reg[iterReg].var->in_reg = 0;
            reg[iterReg].var->dirty = 0;
        }
        reg[iterReg].valid = 1;
    }
    fprintf(out,"\n%s:\n",CodeGen_CheckFunc(code->code.info.func.func_name));
    // save register
    PUSH(out,"$fp");
    // mov $fp, $sp
    fprintf(out,"move $fp, $sp\n");
    esp_val = 0;
    return 1;
}

int param_index = 0;
int CodeGen_Param(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    
    OperandDTPtr entry;
    OperandPtr op = (OperandPtr)malloc(sizeof(Operand));
    op->kind = VARIABLE;
    op->info.var_name = code->code.info.param.x;

    // register the parameter
    param_index++;
    if(param_index <= 4){
        // 1 <= param_index <= 4
        addOperand(op,&entry);
        entry->in_reg = 1;
        entry->reg = 3 + param_index;
    }else{
        // param_index >= 5
        entry->isVar = 0;
        entry->in_reg = 0;
        entry->base_reg = 30;
        entry->offset = (param_index-4)*4 + 4;
    }
    return 1;
}

int CodeGen_Dec(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    
    // register the array and structure
    // register(code->code.info.dec.x)
    OperandDTPtr entry;
    OperandPtr op,x;
    x = code->code.info.dec.x;
    op = (OperandPtr)malloc(sizeof(Operand));
    op->kind = VARIABLE;
    op->info.var_name = x->info.var_name + 1;

    addOperand(op,&entry);
    esp_val -= code->code.info.dec.size;
    fprintf(out,"addi $sp, $sp, -%d\n",code->code.info.dec.size);
    entry->offset = esp_val;
    entry->isVar = 0;
    return 1;
}

int arg_cnt = 0;
int CodeGen_Args(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    int arg_reg = getReg(out,code->code.info.arg.x,RIGHT_VAL,liveVar);
    if(code->prev->code.kind != ARG){
        // backup active variable among t0 - t9
        backupReg(out,liveVar);
        arg_cnt = 0;
    }
    if(arg_cnt < 4){
        fprintf(out,"move %s, %s\n",mips_reg[4+arg_cnt],mips_reg[arg_reg]);
    }else{
        PUSH(out,mips_reg[arg_reg]);
    }
    arg_cnt++;
    return 1;
}

int CodeGen_CallFunc(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){   
    OperandDTPtr entry;
    // push the return addr
    PUSH(out,"$ra");
    // call function
    fprintf(out,"jal %s\n",CodeGen_CheckFunc(code->code.info.call_func.func_name));
    // restore the return addr
    POP(out,"$ra");
    // restore the esp value
    fprintf(out,"addi $sp,$sp,%d\n",(arg_cnt+1)*4);
    esp_val -= (arg_cnt+1)*4;
    // restore the backup regsiter
    restoreReg(out,liveVar);
    // get the return value
    OperandPtr x = code->code.info.call_func.x;
    if(x){
        addOperand(x,&entry);
        entry->dirty = 1;
        int ret_reg = getReg(out,x,LEFT_VAL,liveVar);
        fprintf(out,"move %s, $v0\n",mips_reg[ret_reg]);
        freeVar(out,x,liveVar);
    }
    return 1;
}

int CodeGen_Ret(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    OperandPtr x = code->code.info.ret.x;
    int ret_reg = getReg(out,x,RIGHT_VAL,liveVar);
    // restore the register, such as s0-s7
    fprintf(out,"move $sp, $fp\n");
    POP(out,"$fp");
    fprintf(out,"move $v0, %s\njr $ra\n",mips_reg[ret_reg]);
    freeVar(out,x,liveVar);
    return 1;
}

int CodeGen_Read(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    OperandPtr x = code->code.info.read.x;
    code->code.kind = CALL;
    code->code.info.call_func.x = x;
    code->code.info.call_func.func_name = "read";
    CodeGen_CallFunc(out,code,liveVar);
    return 1;
}

int CodeGen_Write(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar){
    OperandPtr x = code->code.info.write.x;
    code->code.kind = ARG;
    code->code.info.arg.x = x; 
    CodeGen_Args(out,code,liveVar);
    code->code.kind = CALL;
    code->code.info.call_func.func_name = "write";
    code->code.info.call_func.x = NULL;
    CodeGen_CallFunc(out,code,liveVar);
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
