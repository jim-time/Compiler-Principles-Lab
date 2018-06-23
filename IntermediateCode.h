#ifndef __INTERMEDIATECODE_H_
#define __INTERMEDIATECODE_H_
#include "main.h"

typedef struct Operand_t Operand;
typedef struct InterCode_t InterCode;
typedef struct InterCodeListNode_t InterCodeListNode;
typedef struct LabelInfo_t LabelInfo;

typedef Operand* OperandPtr;
typedef InterCode* InterCodePtr;
typedef InterCodeListNode* InterCodeListNodePtr;
typedef LabelInfo* LabelInfoPtr;

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
        struct {uint32_t label;} label;
        struct {char* func_name;} func;
        struct {uint32_t to;} goto_here; 
        struct { OperandPtr x,y; char* relop; uint32_t true_label;} cond;
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

// conditon judgment
struct LabelInfo_t{
    uint32_t true_cnt;
    InterCodeListNodePtr true_node;
    uint32_t false_cnt;
    InterCodeListNodePtr false_node;
    uint32_t next_cnt;
    InterCodeListNodePtr next_node;
};

extern int label_cnt;
extern int condition_flag;
extern LabelInfo label_info;
//translate functions
int translate_func_dec(FuncTablePtr func);
int translate_globalvar(char* var_name);
int translate_localvar(char* var_name);
int translate_arr(struct SyntaxTreeNode* ArrBase,FieldListPtr* arr,FieldListPtr* ref_arr_base);
int translate_structfield(FieldListPtr* struct_hdr,FieldListPtr* ref_field,int offset);
int translate_assign(FieldListPtr lval,FieldListPtr rval);
int translate_arithmetic(FieldListPtr val1, char operation, FieldListPtr val2);

int translate_goto(uint32_t to);
int translate_label(char tag,uint32_t label);
int translate_exchange_label(char a,char b);
int translate_cond(struct SyntaxTreeNode* Exp,FieldListPtr* ret_val,uint32_t true_label,uint32_t false_label);
int translate_other_cond(FieldListPtr ret,uint32_t true_label,uint32_t false_label);

int translate_2op_cond(FieldListPtr op1, char* op_name,FieldListPtr op2,uint32_t true_label,uint32_t false_label);
int translate_logic(struct SyntaxTreeNode* Exp,FieldListPtr* ret);
int translate_1op_logic(char* op_name,FieldListPtr op1);

int translate_func_call(FuncTablePtr func_def,FieldListPtr func_call);
int translate_return(FieldListPtr ret);

int translate_ret_ass_num(FieldListPtr* ret,int num);
int fill_operand(FieldListPtr field,OperandPtr operand);

int print_intercodes(FILE* out);

int print_intercodes_assign(FILE* out, InterCodeListNodePtr code);
int print_intercodes_binop(FILE* out, InterCodeListNodePtr code);
char* sprint_operand(OperandPtr op);
#endif
