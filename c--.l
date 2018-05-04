/**
 *  @file   c--.l
 *  @brief  
 *  @author jim
 *  @date   2018-5-3
 *  @version: v1.1
**/

%option yylineno
%option noyywrap    
%{
    #include"stdio.h"
    int int_num;
    float float_num;
    char c_comment;
    int n_line;
    /*void comments();*/
%}
INT_DEC ([1-9][0-9]*)|[0]
INT_OCT [0][0-7]+
INT_HEX [0][Xx]([0-9a-fA-F]+)
INT {INT_DEC}|{INT_OCT}|{INT_HEX}
FLOAT ([0-9]*\.[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?[fF]?
COMMENTS "/*"|(\/\/.*)
WHITESPACE [ \t]+
SEMI ;
COMMA ,
ASSIGNOP =
RELOP \>|\<|\>=|\<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
ID [_a-zA-Z][_a-zA-z0-9]*
%%
{WHITESPACE} {
}
{TYPE} {
    printf("TYPE: %s\n",yytext);
}
{MINUS} {
    printf("MINUS\n");
}
{INT} {
    if(yytext[0] == '0'){   
        if(yyleng > 1){
            if(yytext[1] == 'x' || yytext[1] == 'X'){   /*recognize the hexadecimal format*/
                sscanf(yytext+2,"%x",&int_num);
                printf("INT: %d\n",int_num);
            }else {     /*recognize the octal format*/
                sscanf(yytext+1,"%o",&int_num);
                printf("INT: %d\n",int_num);
            }
        }else
            /*recognize the zero*/
             printf("INT: %d\n",atoi(yytext));   
    }else
        /*recognize the decimal format*/
        printf("INT: %d\n",atoi(yytext));  
}
(0([0-7]*[89][0-7]*)+) {
    printf("Error type [A] at Line [%d]: Illegal octal number \'%s\'\n",yylineno,yytext);
}
(0[xX]([0-9a-fA-F]*[g-zG-Z][0-9a-fA-F]*)+) {
    printf("Error type [A] at Line [%d]: Illegal hexadecimal number \'%s\'\n",yylineno,yytext);    
}
\n {
    n_line++;
}
{FLOAT} {
    sscanf(yytext,"%f",&float_num);
    printf("FLOAT: %.6f\n",float_num);
}
{COMMENTS} {
    if(yytext[0] == '/' && yytext[1] == '*'){
        while((c_comment = input())!=0){
            if(c_comment == '*'){
                c_comment = input();
                if(c_comment == '/'){
                    printf("End of comments @ line [%d]\n",yylineno);
                    break;
                }else
                    unput(c_comment);
            }
        }
    }else
        printf("COMMENTS @line [%d]\n",yylineno);
}
{SEMI} {
    printf("SEMI\n");
}
{COMMA} {
    printf("COMMA\n");
}
{ASSIGNOP} {
    printf("ASSIGNOP\n");
}
{RELOP} {
    printf("RELOP: %s\n",yytext);
}
{PLUS} {
    printf("PLUS\n");
}
{STAR} {
    printf("STAR\n");
}
{DIV} {
    printf("DIV\n");
}
{AND} {
    printf("AND\n");
}
{OR} {
    printf("OR\n");
}
{DOT} {
    printf("DOT\n");
}
{NOT} {
    printf("NOT\n");
}
{LP} {
    printf("LP\n");
}
{RP} {
    printf("RP\n");
}
{LB} {
    printf("LB\n");
}
{RB} {
    printf("RB\n");
}
{LC} {
    printf("LC\n");
}
{RC} {
    printf("RC\n");
}
{STRUCT} {
    printf("STRUCT\n");
}
{RETURN} {
    printf("RETURN\n");
}
{IF} {
    printf("IF\n");
}
{ELSE} {
    printf("ELSE\n");
}
{WHILE} {
    printf("WHILE\n");
}
{ID} {
    printf("ID: %s\n",yytext);
}
.   {
    printf("Error type [A] at Line [%d]:Mysterious characters\"%s\"\n",yylineno,yytext);
}
%%
void comments(){
    char c;
    input();
    input();
    while((c = input())!=0){
        if(c == '*'){
            if((c = input())=='/'){
                printf("End of comments @ line [%d]\n",yylineno);
                break;
            }else
                unput(c);
        }
    }
}
