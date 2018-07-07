#ifndef __REGALLOCATION_H_
#define __REGALLOCATION_H_
#include "main.h"
#include "Bitmap.h"

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
    BasicBlockNodePtr prev;
    BasicBlockNodePtr branch;
    // live-variable analysize
    int lines;
    BitmapPtr in;
    BitmapPtr* out;
    BitmapPtr* old_out;
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
        uint8_t dummy:7;
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
        uint8_t dirty:1;
        uint8_t isVar:1;
        uint8_t dummy:4;
    };
    UT_hash_handle hh;
};

//operands table
extern struct OperandDT_t *ops;
extern int esp_val;
extern int backupCnt;
extern int reg_cnt;

// live-variable analysize
int CreateBasicBlock();
int getLiveVarInfo();
int print_basicblock();
int getUseDef(InterCodeListNodePtr pcode,BitmapPtr def,BitmapPtr use);
int getOperandBitInfo(OperandPtr op);
int BasicBlock_LiveVariable(BasicBlockNodePtr block_trailer);

// algorithm to the allocation for register
int RegAllocate(FILE* out,OperandDTPtr x,BitmapPtr liveVar);
int addOperand(OperandPtr op,OperandDTPtr* ret);
char* getOpName(OperandPtr op);
int getReg(FILE* out, OperandPtr var,int side,BitmapPtr liveVar);
int freeVar(FILE* out,OperandPtr op,BitmapPtr liveVar);
int getRandomNum();

// activation records
int backupReg(FILE* out,BitmapPtr liveVar);
int restoreReg(FILE* out, BitmapPtr liveVar);
#endif
