#include "main.h"
#include "time.h"
#include "sys/time.h"
#include "Bitmap.h"


#define OP_NAME_LEN 16
struct OperandDT_t *ops = NULL;
int esp_val = 0;
int nr_blocks = 0;

typedef struct LabelBlock_t LabelBlock;
struct LabelBlock_t{
    int order;
    BasicBlockNodePtr node;
};

int CreateBasicBlock(){
    LabelBlock labels[label_cnt];
    memset(labels,0,sizeof(LabelBlock)*label_cnt);
    int label;

    InterCodeListNodePtr pcode = intercodes.header->succ;
    BasicBlockNodePtr node,new_node;
    BasicBlockPtr blocks;
    for(;pcode != intercodes.trailer;pcode = pcode->succ){
        switch(pcode->code.kind){
            case FUNCTION:
                blocks = (BasicBlockPtr)malloc(sizeof(BasicBlock));
                BasicBlocks[nr_blocks++] = blocks;
                node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                memset(node,0,sizeof(BasicBlockNode));
                blocks->header = blocks->trailer = node;
                node->code_begin = node->code_end = pcode;
                
                break;
            case LABEL:
                label = pcode->code.info.label.label;
                if(!labels[label].node){
                    labels[label].node = new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                    memset(new_node,0,sizeof(BasicBlockNode));
                }else{
                    new_node = labels[label].node;
                }
                node->next = new_node;
                node->branch = NULL;
                new_node->code_begin = new_node->code_end = pcode;
                node = new_node;

                labels[label].order = label;
                labels[label].node = new_node;

                blocks->trailer = node;
                break;
            case COND:
                label = pcode->code.info.cond.true_label;

                new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                memset(new_node,0,sizeof(BasicBlockNode));
                if(pcode->succ->code.kind == LABEL){
                    labels[pcode->succ->code.info.label.label].node = new_node;
                }
                // fill the old node
                if(!node->code_begin)
                    node->code_begin = pcode;
                node->code_end = pcode;
                node->next = new_node;
                if(!labels[label].node){
                    labels[label].node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                }
                node->branch = labels[label].node;
                // fill the new node
                new_node->code_begin = new_node->code_end = pcode->succ;
                node = new_node;
                
                blocks->trailer = node;
                break;
            case GOTO:
                label = pcode->code.info.goto_here.to;

                new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                memset(new_node,0,sizeof(BasicBlockNode));
                if(pcode->succ->code.kind == LABEL){
                    labels[pcode->succ->code.info.label.label].node = new_node;
                }
                // fill the old node
                if(!node->code_begin)
                    node->code_begin = pcode;
                node->code_end = pcode;
                node->next = new_node;
                // branch
                if(!labels[label].node){
                    labels[label].node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                }
                node->branch = labels[label].node;
                // fill the new node
                new_node->code_begin = new_node->code_end = pcode->succ;
                node = new_node;
                
                blocks->trailer = node;
                break;
            default:
                node->code_end = pcode;
                break;
        }
    }
    return 1;
}

int print_basicblock(){
    int iterBlock;
    InterCodeListNodePtr pcode;
    BasicBlockNodePtr pblock;
    for(iterBlock = 0; iterBlock < nr_blocks; iterBlock++){
        printf("Blocks%c\n",'A'+iterBlock);
        pblock = BasicBlocks[iterBlock]->header;
        for(;pblock != BasicBlocks[iterBlock]->trailer;pblock = pblock->next){
            printf("\n");
            for(pcode = pblock->code_begin; pcode->prev != pblock->code_end;pcode = pcode->succ){
                print_singlecode(stdout, pcode);
            }
        }
        printf("\n");
        // printf basic block node
        for(pcode = pblock->code_begin; pcode->prev != pblock->code_end;pcode = pcode->succ){
                print_singlecode(stdout, pcode);
        }
    }
    return 1;
}

char* getOpName(OperandPtr op){
    char* op_name;
    switch(op->kind){
        case VARIABLE:
            op_name = op->info.var_name;
            break;
        case REFERENCE:
            op_name = (char*)malloc(sizeof(char)*OP_NAME_LEN);
            sprintf(op_name,"*%s",op->info.var_name);
            break;
        case CONSTANT_INT:
            op_name = (char*)malloc(sizeof(char)*OP_NAME_LEN);
            sprintf(op_name,"%d",op->info.int_val);
            break;
        case CONSTANT_FLOAT:
            op_name = (char*)malloc(sizeof(char)*OP_NAME_LEN);
            sprintf(op_name,"%f",op->info.float_val);
            break;
        case ADDRESS:
            op_name = (char*)malloc(sizeof(char)*OP_NAME_LEN);
            sprintf(op_name,"&%s",op->info.var_name);
            break;
        default:
            break;
    }
    return op_name;
}


int addOperand(OperandPtr op,OperandDTPtr* ret){
    struct OperandDT_t* entry;
    char* name = getOpName(op);
    HASH_FIND_STR(ops,name,entry);
    if(entry){
        (*ret) = entry;
        return 1;
    }
        
    //add a new entry to hash table
    entry = (struct OperandDT_t*)malloc(sizeof(struct OperandDT_t));
    entry->name = name;
    entry->op = op;
    entry->reg = 0;
    entry->base_reg = 30;//$fp
    entry->offset = 0;

    entry->in_reg = 0;
    entry->in_memory = 1;
    entry->swap = 0;
    (*ret) = entry;
    HASH_ADD_KEYPTR(hh,ops,entry->name,strlen(entry->name),entry);
    return 1;
}

int RegAllocate(FILE* out,OperandDTPtr x){
    static int reg_cnt = 0;
    int old_cnt = reg_cnt;
    int iterReg = (reg_cnt+1)%18 + 8;
    for(;reg[iterReg].valid == 0;iterReg = (++reg_cnt)%18 + 8){
        if(iterReg == old_cnt + 8){
            // spilling !
            break;
        }
    }
    if(iterReg == old_cnt + 8){
        // spilling
        iterReg = (reg_cnt+1)%18 + 8;
        reg[iterReg].var->in_reg = 0;
        // write back the variable
        OperandDTPtr pOperand;
        for(pOperand = ops; pOperand != NULL; pOperand = (struct FuncTable_t*)(pOperand->hh.next)){
            if(pOperand->in_reg == 1 && pOperand->reg == iterReg){
                // if the var is tn
                if(pOperand->name[0] == 't'){
                    if(pOperand->offset == 0){
                        PUSH(out,mips_reg[iterReg]);
                        esp_val -=4;
                        pOperand->offset = esp_val;
                    }else{
                        fprintf(out,"sw %s, %d(%s)\n",mips_reg[iterReg],pOperand->offset,mips_reg[pOperand->base_reg]);
                    }
                    pOperand->swap = 1;
                // if the var is vn
                }else{
                    fprintf(out,"sw %s, %d(%s)\n",mips_reg[iterReg],pOperand->offset,mips_reg[pOperand->base_reg]);
                }
                pOperand->in_reg = 0;
                pOperand->in_memory = 1;
            }
        }
    }else{
        reg[iterReg].valid = 0;
        reg[iterReg].var = x;
        x->in_reg = 1;
        x->reg = iterReg;
        
    }
    return iterReg;
}


char* getReg(FILE* out, OperandPtr var){
    struct OperandDT_t* entry;
    char* op_name;

    char* reg_name = (char*)malloc(sizeof(char)*REG_NAME_LEN);
    int reg_cnt = 0;

    // run the algorithm of register allocation
    switch(var->kind){
        case VARIABLE:
            // run the register allocation
            addOperand(var,&entry);
            if(entry->in_reg){
                sprintf(reg_name,"%s",mips_reg[entry->reg]);
            }else{
                reg_cnt = RegAllocate(out,entry);
                if(entry->name[0] == 't'){
                    if(entry->offset!=0){
                        fprintf(out,"lw %s, %d(%s)\n",mips_reg[reg_cnt],entry->offset,mips_reg[entry->base_reg]);
                    }
                }else if(entry->name[0] == 'v'){
                    fprintf(out,"lw %s, %d(%s)\n",mips_reg[reg_cnt],entry->offset,mips_reg[entry->base_reg]);
                }
                sprintf(reg_name,"%s",mips_reg[reg_cnt]);
            }
           
            break;
        case REFERENCE:
            addOperand(var,&entry);
            if(entry->in_reg){
                sprintf(reg_name,"%s",mips_reg[entry->reg]);
            }else{
                reg_cnt = RegAllocate(out,entry);
                sprintf(reg_name,"%s",mips_reg[reg_cnt]);
            }
            break;
        case CONSTANT_INT:
            // li $reg, #num
            sprintf(reg_name,"$%d",reg_cnt);
            fprintf(out,"li %s, %d\n", reg_name, var->info.int_val);
            break;
        case CONSTANT_FLOAT:

            break;
        case ADDRESS:

            break;
        default:
            reg_cnt = 0;
            break;
    }

    return reg_name;
}


int getRandomNum(){
    struct timeval time;
    gettimeofday(&time,NULL);
    srand(time.tv_usec);
    return rand();
}
