#ifndef __CODEGENERATOR_H_
#define __CODEGENERATOR_H_
#include "main.h"
#include "Bitmap.h"

// external defintion
// IntermediateCode.h
typedef struct Operand_t Operand;
typedef struct InterCode_t InterCode;
typedef struct InterCodeListNode_t InterCodeListNode;
typedef struct LabelInfo_t LabelInfo;

typedef Operand* OperandPtr;
typedef InterCode* InterCodePtr;
typedef InterCodeListNode* InterCodeListNodePtr;
typedef LabelInfo* LabelInfoPtr;

// CodeGenerator.h
#define PUSH(out,x)                                 \
        do{                                         \
            fprintf(out,"addi $sp, $sp, -4\n");     \
            fprintf(out,"sw %s, 0($sp)\n",x);       \
        }while(0)

#define POP(out,x)                                  \
        do{                                         \
            fprintf(out,"lw %s, 0($sp)\n",x);       \
            fprintf(out,"addi $sp, $sp, 4\n");      \
        }while(0)

#define REG_NAME_LEN 16
#define NUM_MIPS_REG 32
#define MAX_FUNC 50
extern char* mips_reg[NUM_MIPS_REG];
extern int param_index;
extern int localvars[MAX_FUNC];
extern int funcCnt;
extern int arg_index;

#define LEFT_VAL 0x1
#define RIGHT_VAL 0x2

int CodeGenerator(FILE* out);
int CodeGen_Start(FILE* out);
int CodeGen_Init(FILE* out);
int CodeGen_Assign(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_BinOp(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Cond(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);

int CodeGen_DefFunc(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Param(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Dec(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Args(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_CallFunc(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Ret(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Read(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
int CodeGen_Write(FILE* out, InterCodeListNodePtr code,BitmapPtr liveVar);
char* CodeGen_CheckFunc(char* func_name);

#endif
