#ifndef __SEMANTICANALYSIS_H_
#define __SEMANTICANALYSIS_H_
#include "SyntaxTree.h"
#include "TypeCheck.h"
#include "SymbolTable.h"
#include "my_vector.h"

extern int hierarchy;

//syntax tree analyze
int ST_Program(struct SyntaxTreeNode* Program);
int ST_ExtDefList(struct SyntaxTreeNode* ExtDefList);
int ST_ExtDef(struct SyntaxTreeNode* ExtDef);
int ST_ExtDecList(struct SyntaxTreeNode* ExtDecList, TypePtr field_type);

int ST_Specifier(struct SyntaxTreeNode* Specifier,TypePtr* ret);
int ST_StructSpecifier(struct SyntaxTreeNode* StructSpecifier,TypePtr* ret);
int ST_OptTag(struct SyntaxTreeNode* OptTag,const char** ret_id);
int ST_Tag(struct SyntaxTreeNode* Tag, const char** ret_id);

int ST_VarDec(struct SyntaxTreeNode* VarDec,TypePtr field_type, FieldListPtr* ret_field);
int ST_FuncDec(struct SyntaxTreeNode* FuncDec,FuncTablePtr func);
int ST_VarList(struct SyntaxTreeNode* VarList,FuncTablePtr func);
int ST_ParamDec(struct SyntaxTreeNode* ParamDec, FieldListPtr* para);

int ST_CompSt(struct SyntaxTreeNode* CompSt);
int ST_StmtList(struct SyntaxTreeNode* StmtList);
int ST_Stmt(struct SyntaxTreeNode* Stmt);
int ST_DefList(struct SyntaxTreeNode* DefList,FieldListPtr* ret_field);
int ST_Def(struct SyntaxTreeNode* Def,FieldListPtr* ret_field);

int ST_DecList(struct SyntaxTreeNode* DecList,TypePtr field_type,FieldListPtr* ret_field);
int ST_Dec(struct SyntaxTreeNode* Dec,TypePtr field_type,FieldListPtr* ret_field);

int ST_Exp(struct SyntaxTreeNode* Exp);
int ST_Args(struct SyntaxTreeNode* Args);

#endif
