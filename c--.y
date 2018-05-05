/**bison -d syntax.y
 *  @file   c--.y
 *  @brief  
 *  @author jim
 *  @date   2018-5-5
 *  @version: v1.0
**/
%{
    #define YYERROR_VERBOSE
    #include "stdio.h"
    extern int yylex (void);
    extern int yylineno;

    int yyerror(const char* msg);
    int print_syntax(char* syntax);
%}
%code requires {
  #define YYLTYPE YYLTYPE
  typedef struct YYLTYPE
  {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    char *filename;
  } YYLTYPE;
}

%union {
    int type_int;
    float type_float;
    double type_double;
    char   *type_string;
}
 /* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token TYPE
%token PLUS MINUS STAR DIV  
%token COMMA SEMI DOT
%token RELOP
%token ASSIGNOP
%token AND OR NOT 
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF ELSE WHILE
%token <type_string>ID
%token COMMENTS
/* declared non-terminals */
//%type Program ExtDefList ExtDef ExtDecList Specifire StructSpecifire OptTag Tag VarDec FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args
/*declared priority*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%%
Program : ExtDefList
    ;
ExtDefList : ExtDef ExtDefList
    | /* empty */
    ;
ExtDef : Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | Specifier ExtDecList error    /*uncertain 0*/
    ;
ExtDecList : VarDec
    | VarDec COMMA ExtDecList
    ;
Specifier : TYPE
    | StructSpecifier
    ;
StructSpecifier : STRUCT OptTag LC DefList RC
    | STRUCT Tag
    | STRUCT OptTag LC DefList error    /*uncertain 1*/
    ;
OptTag : ID
    | /* empty */
    ;
Tag : ID
    ;
VarDec : ID
    | VarDec LB INT RB
    | VarDec LB error RB
    ;
FunDec : ID LP VarList RP
    | ID LP RP
    ;
VarList : ParamDec COMMA VarList
    | ParamDec
    ;
ParamDec : Specifier VarDec
    ;
CompSt : LC DefList StmtList RC
    | LC DefList StmtList error
    ;
StmtList : Stmt StmtList
    | /* empty */
    ;
Stmt : Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    | error SEMI
    ;

DefList : Def DefList
    | /* empty */
    ;
Def : Specifier DecList SEMI
    | Specifier error SEMI          /*uncertain*/
    ;
DecList : Dec
    | Dec COMMA DecList
    ;
Dec : VarDec
    | VarDec ASSIGNOP Exp
    ;
Exp : Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp {}
    | Exp PLUS Exp  //{ $$ = $1 + $3; }
    | Exp MINUS Exp //{ $$ = $1 - $3; }
    | Exp STAR Exp  //{ $$ = $1 * $3; }
    | Exp DIV Exp   //{ $$ = $1 / $3; }
    | LP Exp RP
    | MINUS Exp     //{ $$ = -$2; }
    | NOT Exp       //{ $$ = !$2; }
    | ID LP Args RP
    | ID LP RP      
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    | error RP
    ;
Args : Exp COMMA Args
    | Exp
    ;
%%
int yyerror(const char* msg)
 {
    printf("Error type [B] at Line [%d]: %s\n", yylineno,msg);
    return 0;
 }
int print_syntax(char* syntax){
    printf("%s (%d)\n",syntax,yylineno);
    return 0;
 }

