/**bison -d syntax.y    bison -d -v -t c--.y
 *  @file   c--.y
 *  @brief  
 *  @author jim
 *  @date   2018-5-5
 *  @version: v1.0
**/
%{
    #define YYERROR_VERBOSE
    #include "stdio.h"
    #include "../include/SyntaxTree.h"
    #include "../include/SymbolTable.h"
    extern int yylex (void);
    extern int yylineno;
    extern struct SyntaxTreeNode* root;
    extern int error_hint;

    int yyerror(const char* msg);
    int print_error(const char* msg);
    int print_syntax(char* syntax);
    union Node_Data node_data = {0};
%}
%union {
    struct SyntaxTreeNode* type_node;
}
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
/* declared tokens */
%token <type_node> INT
%token <type_node> FLOAT
%token <type_node>TYPE
%token <type_node>PLUS MINUS STAR DIV  
%token <type_node>COMMA SEMI DOT
%token <type_node>RELOP
%token <type_node>ASSIGNOP
%token <type_node>AND OR NOT 
%token <type_node>LP RP LB RB LC RC
%token <type_node>STRUCT
%token <type_node>RETURN
%token <type_node>IF ELSE WHILE
%token <type_node>ID
/* declared non-terminals */
%type <type_node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
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
Program : ExtDefList                {root = CreateNode(NULL,"Program",node_data,0);InsertNode(root,$1);/*PreOrderTraverse($$,0);*/}
    ;
ExtDefList : ExtDef ExtDefList      {$$ = CreateNode(NULL,"ExtDefList",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}
    | /* empty */                   {$$ = CreateNode(NULL,"ExtDefList",node_data,0);}
    ;
ExtDef : Specifier ExtDecList SEMI  {$$ = CreateNode(NULL,"ExtDef",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Specifier SEMI                {$$ = CreateNode(NULL,"ExtDef",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}
    | Specifier FunDec SEMI         {$$ = CreateNode(NULL,"ExtDef",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3); }
    | Specifier FunDec CompSt       {$$ = CreateNode(NULL,"ExtDef",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    ;
ExtDecList : VarDec                 {$$ = CreateNode(NULL,"ExtDecList",node_data,0);InsertNode($$,$1);}
    | VarDec COMMA ExtDecList       {$$ = CreateNode(NULL,"ExtDecList",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    ;
Specifier : TYPE                    {$$ = CreateNode(NULL,"Specifier",node_data,0);InsertNode($$,$1);}
    | StructSpecifier               {$$ = CreateNode(NULL,"Specifier",node_data,0);InsertNode($$,$1);}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   {$$ = CreateNode(NULL,"StructSpecifier",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);InsertNode($$,$5);}
    | STRUCT Tag                    {$$ = CreateNode(NULL,"StructSpecifier",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}
    | STRUCT OptTag LC error RC   /*error 1*/   
    ;
OptTag : ID                         {$$ = CreateNode(NULL,"OptTag",node_data,0);InsertNode($$,$1);}
    | /* empty */                   {$$ = CreateNode(NULL,"OptTag",node_data,0);}
    ;
Tag : ID                            {$$ = CreateNode(NULL,"Tag",node_data,0);InsertNode($$,$1);}
    ;
VarDec : ID                         {$$ = CreateNode(NULL,"VarDec",node_data,0);InsertNode($$,$1);}
    | VarDec LB INT RB              {$$ = CreateNode(NULL,"VarDec",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);}
    | VarDec LB error RB    /*error */      
    ;
FunDec : ID LP VarList RP           {$$ = CreateNode(NULL,"FunDec",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);}
    | ID LP RP                      {$$ = CreateNode(NULL,"FunDec",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | ID LP error RP        /*error 2*/
    ;
VarList : ParamDec COMMA VarList    {$$ = CreateNode(NULL,"VarList",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | ParamDec                      {$$ = CreateNode(NULL,"VarList",node_data,0);InsertNode($$,$1);}  
    ;
ParamDec : Specifier VarDec         {$$ = CreateNode(NULL,"ParamDec",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}  
    ;
CompSt : LC DefList StmtList RC     {$$ = CreateNode(NULL,"CompSt",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);}  
    | LC DefList error RC   /*error */
    ;
StmtList : Stmt StmtList            {$$ = CreateNode(NULL,"StmtList",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}  
    | /* empty */                   {$$ = CreateNode(NULL,"StmtList",node_data,0);}
    ;
Stmt : Exp SEMI                     {$$ = CreateNode(NULL,"Stmt",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}  
    | CompSt                        {$$ = CreateNode(NULL,"Stmt",node_data,0);InsertNode($$,$1);}  
    | RETURN Exp SEMI               {$$ = CreateNode(NULL,"Stmt",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}  
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   {$$ = CreateNode(NULL,"Stmt",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);InsertNode($$,$5);}  
    | IF LP Exp RP Stmt ELSE Stmt   {$$ = CreateNode(NULL,"Stmt",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);InsertNode($$,$5);InsertNode($$,$6);InsertNode($$,$7);}  
    | WHILE LP Exp RP Stmt          {$$ = CreateNode(NULL,"Stmt",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);InsertNode($$,$5);}  
    | error SEMI            /*error */{print_error("Missing \";\"");}
    ;

DefList : Def DefList               {$$ = CreateNode(NULL,"DefList",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}  
    | /* empty */                   {$$ = CreateNode(NULL,"DefList",node_data,0);}
    ;
Def : Specifier DecList SEMI        {$$ = CreateNode(NULL,"Def",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}  
    | Specifier error SEMI   /*error */ {print_error("Missing \";\"");} 
    ;
DecList : Dec                       {$$ = CreateNode(NULL,"DecList",node_data,0);InsertNode($$,$1);}   
    | Dec COMMA DecList             {$$ = CreateNode(NULL,"DecList",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}   
    ;
Dec : VarDec                        {$$ = CreateNode(NULL,"Dec",node_data,0);InsertNode($$,$1);} 
    | VarDec ASSIGNOP Exp           {$$ = CreateNode(NULL,"Dec",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}   
    | VarDec error Exp       /*error 3*/
    ;
Exp : Exp ASSIGNOP Exp              {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}   
    | Exp AND Exp                   {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp OR Exp                    {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp RELOP Exp                 {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp PLUS Exp                  {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp MINUS Exp                 {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp STAR Exp                  {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp DIV Exp                   {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | LP Exp RP                     {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | MINUS Exp                     {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}
    | NOT Exp                       {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);}
    | ID LP Args RP                 {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);}
    | ID LP RP                      {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp LB Exp RB                 {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);InsertNode($$,$4);}
    | Exp DOT ID                    {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | ID                            {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);}
    | INT                           {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);}
    | FLOAT                         {$$ = CreateNode(NULL,"Exp",node_data,0);InsertNode($$,$1);}
    | LP error RP          /*error */
    | Exp LB error RB      /*error */ //{yyerror("Missing \"]\"");}
    ;
Args : Exp COMMA Args               {$$ = CreateNode(NULL,"Args",node_data,0);InsertNode($$,$1);InsertNode($$,$2);InsertNode($$,$3);}
    | Exp                           {$$ = CreateNode(NULL,"Args",node_data,0);InsertNode($$,$1);}
    ;
%%
int yyerror(const char* msg)
 {
    printf("Error type [B] at Line [%d]: %s\n", yylineno,msg);
    error_hint = 1;
    //DestructTree($$);
    return 0;
 }
int print_error(const char* msg){
    //printf("Error type [B] at Line [%d]: %s\n", yylineno,msg);
    return 0;    
}

