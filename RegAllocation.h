#ifndef __REGALLOCATION_H_
#define __REGALLOCATION_H_
#include "main.h"

#define NUM_BLOCKS  10
#define NUM_REG 32

typedef struct BasicBlockNode_t BasicBlockNode;
typedef BasicBlockNode* BasicBlockNodePtr;
typedef struct BasicBlock_t BasicBlock;
typedef BasicBlock* BasicBlockPtr;

typedef struct RegDT_t RegDT;
typedef RegDT* RegDTPtr;
typedef struct OperandDT_t OperandDT;
typedef OperandDT* OperandDTPtr;

struct BasicBlockNode_t{
    InterCodeListNodePtr code_begin;
    InterCodeListNodePtr code_end;
    BasicBlockNodePtr next;
    BasicBlockNodePtr branch;
};

struct BasicBlock_t{
    BasicBlockNodePtr header;
    BasicBlockNodePtr trailer;
};

BasicBlockPtr BasicBlocks[NUM_BLOCKS];
int nr_blocks;

struct RegDT_t{
    char* name;
    OperandDTPtr var;
    struct{
        uint8_t valid:1;
        uint8_t isNum:1;
        uint8_t dummy:6;
    };
}reg[NUM_REG];

struct OperandDT_t{
    char* name;
    OperandPtr op;
    // reg index
    int reg;
    // memory addr
    int base_reg;
    int offset;
    struct{
        uint8_t in_reg:1;
        uint8_t in_memory:1;
        uint8_t swap:1;
        uint8_t dummy:5;
    };
    UT_hash_handle hh;
};

//operands table
extern struct OperandDT_t *ops;
extern int esp_val;

int CreateBasicBlock();
int print_basicblock();

int RegAllocate(FILE* out,OperandDTPtr x);
int addOperand(OperandPtr op,OperandDTPtr* ret);
char* getOpName(OperandPtr op);
char* getReg(FILE* out, OperandPtr var);


int getRandomNum();
#endif
