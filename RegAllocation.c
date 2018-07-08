#include "main.h"
#include "time.h"
#include "sys/time.h"
#include "Bitmap.h"


#define OP_NAME_LEN 16
struct OperandDT_t *ops = NULL;
int esp_val = 0;


/**
 *  @file   RegAllocation.c
 *  @brief  Analysize the live variable
 *  @author jim
 *  @date   2018-7-6
 *  @version: v1.0
**/ 
int nr_blocks = 0;
typedef struct LabelBlock_t LabelBlock;
struct LabelBlock_t{
    int order;
    BasicBlockNodePtr node;
};

LabelBlock *labels;
int CreateBasicBlock(){
    //LabelBlock labels[label_cnt];
    labels = (LabelBlock*)malloc(sizeof(LabelBlock)*label_cnt);
    memset(labels,0,sizeof(LabelBlock)*label_cnt);
    int label,lines;

    InterCodeListNodePtr pcode = intercodes.header->succ;
    BasicBlockNodePtr node = NULL,new_node = NULL;
    BasicBlockPtr blocks;
    for(;pcode != intercodes.trailer;pcode = pcode->succ){
        switch(pcode->code.kind){
            case FUNCTION:
                // save the old info
                if(node){
                    node->lines = lines;
                    node->next = NULL;
                }
                lines = 1;
                
                blocks = (BasicBlockPtr)malloc(sizeof(BasicBlock));
                memset(blocks,0,sizeof(BasicBlock));
                BasicBlocks[nr_blocks++] = blocks;
                node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                memset(node,0,sizeof(BasicBlockNode));

                blocks->header = blocks->trailer = node;
                node->code_begin = node->code_end = pcode;
                break;
            case LABEL:
                // save the old info
                if(node)
                    node->lines = lines;
                lines = 1;
                
                label = pcode->code.info.label.label;
                if(!labels[label].node){
                    labels[label].order = label;
                    new_node = labels[label].node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                    memset(new_node,0,sizeof(BasicBlockNode));
                }else{
                    // before the label, its node has been register
                    new_node = labels[label].node;
                }
                
                node->next = new_node;
                new_node->prev = node;

                // fill the succ
                if(pcode->prev->code.kind != GOTO)
                    node->succ = new_node;

                new_node->code_begin = new_node->code_end = pcode;
                node = new_node;
                //new_node = NULL;
                blocks->trailer = node;
                break;
            case COND:
                label = pcode->code.info.cond.true_label;

                new_node = NULL;
                if(pcode->succ->code.kind == LABEL){
                    if(labels[pcode->succ->code.info.label.label].node)
                        new_node = labels[pcode->succ->code.info.label.label].node;
                    else{
                        new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                        labels[pcode->succ->code.info.label.label].node = new_node;
                        memset(new_node,0,sizeof(BasicBlockNode));
                    }
                }else{
                    new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                    memset(new_node,0,sizeof(BasicBlockNode));
                }

                node->next = new_node;
                new_node->prev = node;
                // fill the old node
                if(!node->code_begin)
                    node->code_begin = pcode;
                node->code_end = pcode;
                
                //fill the branch
                if(!labels[label].node){
                    labels[label].order = label;
                    labels[label].node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                    memset(labels[label].node,0,sizeof(BasicBlockNode));
                }
                node->branch = labels[label].node;
                // fill the succ
                node->succ = new_node;
                node->lines = ++lines;
                
                // fill the new node
                new_node->code_begin = new_node->code_end = pcode->succ;
                if(pcode->succ->code.kind != LABEL){
                    node = new_node;
                    lines = 0;
                }
                new_node = NULL;
                blocks->trailer = node;

                break;
            case GOTO:
                label = pcode->code.info.goto_here.to;

                new_node = NULL;
                if(pcode->succ->code.kind == LABEL){
                    if(labels[pcode->succ->code.info.label.label].node)
                        new_node = labels[pcode->succ->code.info.label.label].node;
                    else{
                        new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                        labels[pcode->succ->code.info.label.label].node = new_node;
                        memset(new_node,0,sizeof(BasicBlockNode));
                    }
                }else{
                    new_node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                    memset(new_node,0,sizeof(BasicBlockNode));
                }
                
                // fill the old node
                if(!node->code_begin)
                    node->code_begin = pcode;
                node->code_end = pcode;
                node->next = new_node;
                new_node->prev = node;

                // branch
                if(!labels[label].node){
                    labels[label].order = label;
                    labels[label].node = (BasicBlockNodePtr)malloc(sizeof(BasicBlockNode));
                    memset(labels[label].node,0,sizeof(BasicBlockNode));
                }
                node->branch = labels[label].node;
                node->succ = NULL;
                node->lines = ++lines;

                // fill the new node
                new_node->code_begin = new_node->code_end = pcode->succ;
                if(pcode->succ->code.kind != LABEL){
                    lines = 0;
                    node = new_node;
                }
                
                blocks->trailer = node;
                break;
            default:
                node->lines = ++lines;
                node->code_end = pcode;
                break;
        }
    }
    if(node)
        node->next = NULL;
    return 1;
}

int getLiveVarInfo(){
    int iterBlock;
    BasicBlockNodePtr pblock;
    for(iterBlock = 0; iterBlock < nr_blocks; iterBlock++){
        pblock = BasicBlocks[iterBlock]->trailer;
        BasicBlock_LiveVariable(pblock);
    }
    return 1;
}

int print_basicblock(){
    int iterBlock,iterCode;
    InterCodeListNodePtr pcode;
    BasicBlockNodePtr pblock;
    for(iterBlock = 0; iterBlock < nr_blocks; iterBlock++){
        for(pblock = BasicBlocks[iterBlock]->header; pblock != NULL ;pblock = pblock->next){
            printf("\n%d\n",pblock->lines);
            // printf("current addr:\t%#x\n",pblock);
            // printf("next addr:\t%#x\n",pblock->next);
            // printf("prev addr:\t%#x\n",pblock->prev);
            // printf("succ addr:\t%#x\n",pblock->succ);
            // printf("branch addr:\t%#x\n",pblock->branch);
            for(pcode = pblock->code_begin,iterCode = 0; pcode->prev != pblock->code_end;pcode = pcode->succ){
                print_singlecode(stdout, pcode);
                if(pblock->out)
                    Bitmap_printMember(pblock->out[iterCode++],stdout);
            }
        }
    }
    return 1;
}

int BasicBlock_LiveVariable(BasicBlockNodePtr block_trailer){
    BitmapPtr def,use,in,temp,tempdef;
    BitmapPtr *old_out,*out;
    int iterCode,label,kind;
    InterCodeListNodePtr pcode;
    BasicBlockNodePtr pblock;

    //initialize in def use set
    in = (BitmapPtr)malloc(sizeof(Bitmap));
    def = (BitmapPtr)malloc(sizeof(Bitmap));
    use = (BitmapPtr)malloc(sizeof(Bitmap));
    // local variable + temp variable
    Bitmap_Create(in,local_cnt+temp_cnt-1);
    Bitmap_Create(def,local_cnt+temp_cnt-1);
    Bitmap_Create(use,local_cnt+temp_cnt-1);

    // clear the out and old_out
    // initialize the out and old_out and in sets
    for(pblock = block_trailer;pblock != NULL; pblock = pblock->prev){
        pblock->out = (BitmapPtr*)malloc(sizeof(BitmapPtr)*pblock->lines);
        memset(pblock->out,0,sizeof(BitmapPtr)*pblock->lines);
        pblock->old_out = (BitmapPtr*)malloc(sizeof(BitmapPtr)*pblock->lines);
        memset(pblock->old_out,0,sizeof(BitmapPtr)*pblock->lines);
        pblock->in = (BitmapPtr)malloc(sizeof(Bitmap));
        Bitmap_Create(pblock->in,local_cnt+temp_cnt-1);
        if(pblock->code_begin->code.kind == LABEL){
            label = pblock->code_begin->code.info.label.label;
            labels[label].node->in = pblock->in;
        }
    }

    int update_flag = 1;
   // int u_cnt = 0;
    while(update_flag){
        update_flag = 0;
        //printf("update %d!\n",u_cnt++);
        for(pblock = block_trailer;pblock != NULL; pblock = pblock->prev){
            if(pblock && pblock->lines){
                out = pblock->out;
                old_out = pblock->old_out;

                for(iterCode = pblock->lines - 1, pcode = pblock->code_end; pcode->succ != pblock->code_begin ; iterCode--,pcode = pcode->prev){
                    if(!old_out[iterCode]){
                        old_out[iterCode] = (BitmapPtr)malloc(sizeof(Bitmap));
                        Bitmap_Create(old_out[iterCode],local_cnt+temp_cnt-1);
                    }
                    if(!out[iterCode]){
                        out[iterCode] = (BitmapPtr)malloc(sizeof(Bitmap));
                        Bitmap_Create(out[iterCode],local_cnt+temp_cnt-1);
                    }
                    // copy out to old_out
                    Bitmap_Copy(old_out[iterCode],out[iterCode]);

                    // compute the set of out
                    switch(pcode->code.kind){
                        case COND:
                            label = pcode->code.info.cond.true_label;
                            if(!labels[label].node->in){
                                labels[label].node->in = (BitmapPtr)malloc(sizeof(Bitmap));
                                Bitmap_Create(labels[label].node->in,local_cnt+temp_cnt-1);
                            }
                            temp = Bitmap_unionWith(in,labels[label].node->in);
                            Bitmap_Copy(out[iterCode],temp);
                            free(temp);
                            // some bugs
                            Bitmap_Copy(in,out[iterCode]);
                            //in = Bitmap_unionWith(in,out[iterCode]);
                            break;
                        case GOTO:
                            label = pcode->code.info.goto_here.to;
                            if(!labels[label].node->in){
                                labels[label].node->in = (BitmapPtr)malloc(sizeof(Bitmap));
                                Bitmap_Create(labels[label].node->in,local_cnt+temp_cnt-1);
                            }
                            Bitmap_Copy(out[iterCode],labels[label].node->in);
                            // some bugs 
                            Bitmap_Copy(in,out[iterCode]);
                            //in = Bitmap_unionWith(in,out[iterCode]);
                            break;
                        case RET:
                            Bitmap_MakeEmpty(in);
                            Bitmap_Copy(out[iterCode],in);
                            break;
                        default:
                            Bitmap_Copy(out[iterCode],in);
                            break;
                    }
                    
                    // compute the set of def and use
                    Bitmap_MakeEmpty(def);
                    Bitmap_MakeEmpty(use);
                    kind = getUseDef(pcode,def,use);
                    // compute the set of in
                    if(kind){
                        //maybe a bug
                        // if(iterCode + 1 < pblock->lines){
                        //     tempdef = def;
                        //     def = Bitmap_differenceFrom(def,out[iterCode+1]);
                        //     free(tempdef);
                        // }
                        
                        free(in);
                        temp = Bitmap_differenceFrom(out[iterCode],def);
                        in = Bitmap_unionWith(use,temp);
                        free(temp);
                    }
                    // update the set of in
                    if(pcode->code.kind == LABEL)
                        Bitmap_Copy(pblock->in,in);
                }
            }
        }
        // check the update info
        for(pblock = block_trailer;pblock != NULL; pblock = pblock->prev){
            for(iterCode = pblock->lines - 1; iterCode>=0 ; iterCode--){
                if(!Bitmap_isEqual(pblock->out[iterCode],pblock->old_out[iterCode])){
                    update_flag |= 0x1;
                }
            }
        }
    }
    return 1;
}

int getUseDef(InterCodeListNodePtr pcode,BitmapPtr def,BitmapPtr use){
    OperandPtr res,op1,op2;
    OperandPtr left, right;
    switch(pcode->code.kind){
        case ASSIGN:
            left = pcode->code.info.assign.left;
            right = pcode->code.info.assign.right;
            if(left->kind == VARIABLE){
                Bitmap_putMember(def,getOperandBitInfo(left),1);
            }else if(left->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(left),1);
            }
            if(right->kind == VARIABLE || right->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(right),1);
            }
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIVISION:
            res = pcode->code.info.binop.result;
            op1 = pcode->code.info.binop.op1;
            op2 = pcode->code.info.binop.op2;
            if(res->kind == VARIABLE){
                Bitmap_putMember(def,getOperandBitInfo(res),1);
            }else if(res->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(res),1);
            }
            if(op1->kind == VARIABLE || op1->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op1),1);
            }
            if(op2->kind == VARIABLE || op2->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op2),1);
            }
            break;
        case COND:
            op1 = pcode->code.info.cond.x;
            op2 = pcode->code.info.cond.y;
            if(op1->kind == VARIABLE || op1->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op1),1);
            }
            if(op2->kind == VARIABLE || op2->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op2),1);
            }
            break;
        case RET:
            op1 = pcode->code.info.ret.x;
            if(op1->kind == VARIABLE || op1->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op1),1);
            }
            break;
        case ARG:
            op1 = pcode->code.info.arg.x;
            if(op1->kind == VARIABLE || op1->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op1),1);
            }
            break;
        case CALL:
            left = pcode->code.info.call_func.x;
            if(left->kind == VARIABLE){
                Bitmap_putMember(def,getOperandBitInfo(left),1);
            }else if(left->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(left),1);
            }
            break;
        case READ:
            left = pcode->code.info.read.x;
            if(left->kind == VARIABLE){
                Bitmap_putMember(def,getOperandBitInfo(left),1);
            }else if(left->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(left),1);
            }
            break;
        case WRITE:
            op1 = pcode->code.info.write.x;
            if(op1->kind == VARIABLE || op1->kind == REFERENCE){
                Bitmap_putMember(use,getOperandBitInfo(op1),1);
            }
            break;
        default:
            return 0;
            break;
    }
    return 1;
}

// +----+--------------+-------------+
// |Bit0|local_variable|temp_variable|
// +----+--------------+-------------+
int getOperandBitInfo(OperandPtr op){
    int id;
    char* idstr;
    switch(op->kind){
        case VARIABLE:
        case REFERENCE:
            sscanf(op->info.var_name+1,"%d",&id);
            if(op->info.var_name[0] == 't'){
                return local_cnt-1+id;
            }else if(op->info.var_name[0] == 'v'){
                return id;
            }
            break;
        default:
            break;
    }   
    return 0;
}


/**
 *  @file   RegAllocation.c
 *  @brief  allocate the register by the live-variable info
 *  @author jim
 *  @date   2018-7-6
 *  @version: v1.0
**/ 
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
    entry->dirty = 0;
    entry->isVar = 1;
    (*ret) = entry;
    HASH_ADD_KEYPTR(hh,ops,entry->name,strlen(entry->name),entry);
    return 1;
}

int reg_cnt = 0;
int RegAllocate(FILE* out,OperandDTPtr x,BitmapPtr liveVar){
    int old_cnt = reg_cnt;
    int iterReg = (reg_cnt+1)%18 + 8;
    int var_id = 0,end_signal = -1;
    // Plan A
    for(;reg[iterReg].valid == 0;iterReg = (++reg_cnt)%18 + 8){        
        if(iterReg == old_cnt + 8){
            // spilling !
            break;
        }
    }
    // Plan B
    if(iterReg == old_cnt + 8){
        // spilling
        end_signal = (reg_cnt)%18 + 8;
        for(iterReg = (reg_cnt+1)%18 + 8;iterReg < end_signal;iterReg = (++reg_cnt)%18 + 8){
            var_id = getOperandBitInfo(reg[iterReg].var->op);
            if(!Bitmap_getMember(liveVar,var_id)){
                // release the variable
                reg[iterReg].valid = 1;
                if(var_id){
                    // write back to the stack
                    reg[iterReg].var->in_reg = 0;
                    OperandDTPtr pOperand;
                    for(pOperand = ops; pOperand != NULL; pOperand = (OperandDTPtr)(pOperand->hh.next)){
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
                               
                            // if the var is vn
                            }else{
                                if(pOperand->offset == 0){
                                    PUSH(out,mips_reg[iterReg]);
                                    esp_val -=4;
                                    pOperand->offset = esp_val;
                                }else
                                    fprintf(out,"sw %s, %d(%s)\n",mips_reg[iterReg],pOperand->offset,mips_reg[pOperand->base_reg]);
                            }
                            pOperand->dirty = 0;
                            pOperand->in_reg = 0;
                            pOperand->in_memory = 1;
                        }
                    }
                }
                break;
            }
        }        
    }
    // Plan C
    if(iterReg == end_signal){
        // pick a register at random
        iterReg = getRandomNum()%18 + 8;
        //write back the variable
        OperandDTPtr pOperand;
        for(pOperand = ops; pOperand != NULL; pOperand = (OperandDTPtr)(pOperand->hh.next)){
            if(pOperand->in_reg == 1 && pOperand->reg == iterReg){
                if(pOperand->name[0] == 'v'){
                    if(pOperand->offset == 0){
                        PUSH(out,mips_reg[iterReg]);
                        esp_val -=4;
                        pOperand->offset = esp_val;
                    }else
                        fprintf(out,"sw %s, %d(%s)\n",mips_reg[iterReg],pOperand->offset,mips_reg[pOperand->base_reg]);
                    pOperand->dirty = 0;
                    pOperand->in_reg = 0;
                    pOperand->in_memory = 1;
                }else{
                    pOperand->dirty = 0;
                    pOperand->in_reg = 0;
                    pOperand->in_memory = 0;
                }
            }
        }
    }
    reg[iterReg].valid = 0;
    reg[iterReg].var = x;
    x->in_reg = 1;
    x->reg = iterReg;
    return iterReg;
}


int getReg(FILE* out, OperandPtr var,int side,BitmapPtr liveVar){
    struct OperandDT_t* entry;
    struct OperandDT_t* base_entry;
    char* op_name;
    int reg_cnt = 0;

    int base,rd,rs;

    // run the algorithm of register allocation
    switch(var->kind){
        case VARIABLE:
            // run the register allocation
            addOperand(var,&entry);
            if(entry->in_reg){
                reg_cnt = entry->reg;
            }else{
                if(entry->name[0] == 't'){
                    reg_cnt = RegAllocate(out,entry,liveVar);
                    if(entry->offset != 0){
                        fprintf(out,"lw %s, %d(%s)\n",mips_reg[reg_cnt],entry->offset,mips_reg[entry->base_reg]);
                    }
                }else if(entry->name[0] == 'v'){
                       // local variable which is not the array or structure
                    if(entry->isVar){
                        reg_cnt = RegAllocate(out,entry,liveVar);
                        if(entry->offset != 0)
                            fprintf(out,"lw %s, %d(%s)\n",mips_reg[reg_cnt],entry->offset,mips_reg[entry->base_reg]);
                        else{
                            // PUSH(out,mips_reg[iterReg]);
                            // esp_val -=4;
                        }
                    }else{
                        // array or structure type
                        // get the address of header
                        reg_cnt = RegAllocate(out,entry,liveVar);
                        fprintf(out,"addi %s, $fp, %d\n",mips_reg[reg_cnt],entry->offset);
                    }
                }
            }
            break;
        case REFERENCE:
            //get the base
            var->kind = VARIABLE;
            addOperand(var,&base_entry);
            base = getReg(out,var,side,liveVar);
            if(var->info.var_name[0] == 't'){
                // loda *t
                var->kind = REFERENCE;
                addOperand(var,&entry);
                rs = RegAllocate(out,entry,liveVar);
                fprintf(out,"lw %s, 0(%s)\n",mips_reg[rs],mips_reg[base]);
            }else{
                // load *v
                var->kind = REFERENCE;
                addOperand(var,&entry);
                rs = RegAllocate(out,entry,liveVar);
                if(base_entry->isVar){
                    fprintf(out,"lw %s, 0(%s)\n",mips_reg[rs],mips_reg[base]);
                }else
                    fprintf(out,"lw %s, %d(%s)\n",mips_reg[rs],base_entry->offset,mips_reg[base_entry->base_reg]);
            }
            reg_cnt = rs;
            break;
        case CONSTANT_INT:
            // li $reg, #num
            addOperand(var,&entry);
            if(entry->in_reg){
                reg_cnt = entry->reg;
            }else{
                reg_cnt = RegAllocate(out,entry,liveVar);
                fprintf(out,"li %s, %d\n", mips_reg[reg_cnt], var->info.int_val);
            }
            break;
        case CONSTANT_FLOAT:

            break;
        case ADDRESS:
            break;
        default:
            reg_cnt = 0;
            break;
    }
    return reg_cnt;
}

int backupCnt = 0;
int backupReg(FILE* out,BitmapPtr liveVar){
    OperandPtr op;
    // backup paramter register: a0-a3
    int iterParam;
    for(iterParam = 0;iterParam < 4; iterParam++){
        if(iterParam < param_index){
            PUSH(out,mips_reg[4+iterParam]);
            esp_val -= 4;
        }else
            break;
    }
    // push other register
    backupCnt = 0;
    int iterReg;
    for(iterReg = 8; iterReg < 26; iterReg++){
        // if(reg[iterReg].var){
        //     if(reg[iterReg].valid == 0){
        //         op = reg[iterReg].var->op;
        //         if(Bitmap_getMember(liveVar,getOperandBitInfo(op))){
        //             PUSH(out,mips_reg[iterReg]);
        //             esp_val -= 4;
        //             backupCnt++;
        //         }
        //     }
        // }
        if(reg[iterReg].var){
            if(reg[iterReg].valid == 0){
                op = reg[iterReg].var->op;
                if(op->kind == VARIABLE){
                    if(op->info.var_name[0] == 'v'){
                        //if(reg[iterReg].var->isVar){
                            PUSH(out,mips_reg[iterReg]);
                            esp_val -= 4;
                            backupCnt++;
                        //}
                    }else if(Bitmap_getMember(liveVar,getOperandBitInfo(op))){
                        PUSH(out,mips_reg[iterReg]);
                        esp_val -= 4;
                        backupCnt++;
                    }
                }
            }
        }
    }
    return 1;
}

int restoreReg(FILE* out, BitmapPtr liveVar){
    int iterReg;
    int iterParam;
    OperandPtr op;
    for(iterReg = 25; iterReg >= 8; iterReg--){
        // if(reg[iterReg].var){
        //     if(reg[iterReg].valid == 0){
        //         op = reg[iterReg].var->op;
        //         if(Bitmap_getMember(liveVar,getOperandBitInfo(op))){
        //             POP(out,mips_reg[iterReg]);
        //             esp_val += 4;
        //         }else{
        //             freeVar(out,op,liveVar);
        //         }
        //     }
        // }
        if(reg[iterReg].var){
            if(reg[iterReg].valid == 0){
                op = reg[iterReg].var->op;
                if(op->kind == VARIABLE){
                    if(op->info.var_name[0] == 'v'){
                        //if(reg[iterReg].var->isVar){
                            POP(out,mips_reg[iterReg]);
                            esp_val += 4;
                        //}
                    }else if(Bitmap_getMember(liveVar,getOperandBitInfo(op))){
                        POP(out,mips_reg[iterReg]);
                        esp_val += 4;
                    }
                }
                freeVar(out,op,liveVar);
            }
        }
    }
    // restore the paramter register: a0-a3
    // 1 <= param_index <= 4
    if(param_index > 4)
        iterParam = 4;
    else
        iterParam = param_index;
    for(;iterParam > 0; iterParam--){
        POP(out,mips_reg[3+iterParam]);
        esp_val += 4;
    }
    return 1;
}

int freeVar(FILE* out,OperandPtr op,BitmapPtr liveVar){
    struct OperandDT_t * entry, *base_entry;
    Operand base;
    addOperand(op,&entry);
    if(entry->in_reg){
        if(entry->reg){
            if(op->kind == VARIABLE){
                // only release the temp variables
                if(op->info.var_name[0] != 'v'){
                    if(!Bitmap_getMember(liveVar,getOperandBitInfo(op))){
                        reg[entry->reg].valid = 1;
                        reg[entry->reg].var = NULL;
                        entry->in_reg = 0;
                        entry->in_memory = 0;
                    }       
                }
            }else if(op->kind == REFERENCE){
                if(op->info.var_name[0] == 't'){
                    base.kind = VARIABLE;
                    base.info.var_name = op->info.var_name;
                    addOperand(&base,&base_entry);
                    if(!Bitmap_getMember(liveVar,getOperandBitInfo(&base))){
                        reg[base_entry->reg].valid = 1;
                        reg[base_entry->reg].var = NULL;
                        base_entry->in_reg = 0;
                        base_entry->in_memory = 0;
                    }      
                }else{

                }
            }else{
                reg[entry->reg].valid = 1;
                reg[entry->reg].var = NULL;
                entry->in_reg = 0;
                entry->in_memory = 0;
            }
            // if(entry->dirty){
            //     //write back
            //     entry->dirty = 0;
            //     if(entry->op->kind == VARIABLE){
            //         //if(entry->op->info.var_name[0] == 'v'){
            //             entry->in_memory = 1;
            //             if(entry->offset == 0){
            //                 PUSH(out,mips_reg[entry->reg]);
            //                 esp_val -=4;
            //                 entry->base_reg = 30;
            //                 entry->offset = esp_val;
            //             }else
            //                 fprintf(out,"sw %s, %d(%s)\n",mips_reg[entry->reg],entry->offset,mips_reg[entry->base_reg]);
            //         //}
            //     }else if(entry->op->kind == REFERENCE){

            //     }
            // }
        }
    }
    return 1;
}

int getRandomNum(){
    struct timeval time;
    gettimeofday(&time,NULL);
    srand(time.tv_usec);
    return rand();
}
