#ifndef __INTERMEDIATECODE_H_
#define __INTERMEDIATECODE_H_
#include "main.h"

typedef struct Operand_t Operand;
typedef struct InterCode_t InterCode;
typedef struct InterCodeListNode_t InterCodeListNode;


typedef Operand* OperandPtr;
typedef InterCode* InterCodePtr;
typedef InterCodeListNode* InterCodeListNodePtr;


struct Operand_t {
    enum { VARIABLE, CONSTANT_INT,CONSTANT_FLOAT, ADDRESS, REFERENCE} kind;
    union {
        char* var_name;
        union{
            int   int_val;
            float float_val;
        };
    }info;
};

struct InterCode_t{
    enum { ASSIGN, ADD, SUB, MUL,DIV,LABEL,FUNCTION,GOTO,COND,RET,DEC,ARG,CALL,PARAM,READ,WRITE} kind;
    union {
        struct { OperandPtr right, left; } assign;
        struct { OperandPtr result, op1, op2; } binop;
        struct {char* label_name;} label;
        struct {char* func_name;} func;
        struct {char* to;} goto_here; 
        struct { OperandPtr x,y; char* relop; char* to_z;} cond;
        struct {OperandPtr x;} ret;
        struct { OperandPtr x; int size;} dec;
        struct {OperandPtr x;} arg;
        struct {OperandPtr x; char* func_name;} call_func;
        struct {char* x;} param;
        struct {OperandPtr x;} read;
        struct {OperandPtr x;} write;
    } info;
};

struct InterCodeListNode_t{
    InterCode code;
    InterCodeListNode* prev;
    InterCodeListNode* succ;
};

extern struct List_t intercodes;
extern int local_cnt;
extern int temp_cnt;

//translate functions
int translate_func(FuncTablePtr func);
int translate_globalvar(char* var_name);
int translate_localvar(char* var_name);
int translate_Exp(struct SyntaxTreeNode* Exp, void* place);
int translate_arr(struct SyntaxTreeNode* ArrBase,FieldListPtr* arr,FieldListPtr* ref_arr_base);
int translate_structfield(FieldListPtr* struct_hdr,FieldListPtr* ref_field,int offset);

int print_intercodes();
char* print_operand(OperandPtr op);
int print_intercodes_assign(InterCodeListNodePtr code);
int print_intercodes_binop(InterCodeListNodePtr code);
#endif
