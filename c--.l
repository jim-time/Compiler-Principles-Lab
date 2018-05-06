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
    #include"stdlib.h"
    #include"string.h"
    #include"c--.tab.h" 
    int int_num;
    float float_num;
    char c_comment;
    int n_line;
    
    int yycolumn = 1;
    YYLTYPE yylloc;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    /*output lex to the scanneroutput.txt*/
    extern FILE *pscanner;  
%}
ID  [_a-zA-Z][_a-zA-Z0-9]*
INT_DEC ([1-9][0-9]*)|[0]
INT_OCT [0][0-7]+
INT_HEX [0][Xx]([0-9a-fA-F]+)
INT {INT_DEC}|{INT_OCT}|{INT_HEX}
FLOAT ([0-9]*\.[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?[fF]?
COMMENTS "/*"|(\/\/.*)
WHITESPACE [ \t]+
EOL \n
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
LB "["
RB "]"
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
%%
(0([0-7]*[89][0-7]*)+) {
    printf("Error type [A] at Line [%d]: Illegal octal number \'%s\'\n",yylineno,yytext);
}
(0[xX]([0-9a-fA-F]*[g-zG-Z][0-9a-fA-F]*)+) {
    printf("Error type [A] at Line [%d]: Illegal hexadecimal number \'%s\'\n",yylineno,yytext);    
}
((\.([eE][+-]?[0-9]*[fF]?))|(([0-9]*\.?[0-9]+|[0-9]+\.)[eE][+-]?[fF]?)) {
    printf("Error type [A] at Line [%d]: Illegal floating point number \'%s\'\n",yylineno,yytext);    
}
{WHITESPACE} {
}
{TYPE} {
    fprintf(pscanner,"TYPE: %s\n",yytext);
    return TYPE;
}
{MINUS} {
    fprintf(pscanner,"MINUS\n");
    return MINUS;
}
{INT} {
    if(yytext[0] == '0'){   
        if(yyleng > 1){
            if(yytext[1] == 'x' || yytext[1] == 'X'){   /*recognize the hexadecimal format*/
                sscanf(yytext+2,"%x",&int_num);
                fprintf(pscanner,"INT: %d\n",int_num);
            }else {     /*recognize the octal format*/
                sscanf(yytext+1,"%o",&int_num);
                fprintf(pscanner,"INT: %d\n",int_num);
            }
        }else
            /*recognize the zero*/
             fprintf(pscanner,"INT: %d\n",atoi(yytext));   
    }else
        /*recognize the decimal format*/
        fprintf(pscanner,"INT: %d\n",atoi(yytext));  
    yylval.type_int = int_num;
    return INT;
}
\n {
    n_line++;
    yycolumn = 1;
}
{FLOAT} {
    sscanf(yytext,"%f",&float_num);
    fprintf(pscanner,"FLOAT: %.6f\n",float_num);
    yylval.type_float = float_num;
    return FLOAT;
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
    fprintf(pscanner,"SEMI\n");
    return SEMI;
}
{COMMA} {
    fprintf(pscanner,"COMMA\n");
    return COMMA;
}
{ASSIGNOP} {
    fprintf(pscanner,"ASSIGNOP\n");
    return ASSIGNOP;
}
{RELOP} {
    fprintf(pscanner,"RELOP\n");
    return RELOP;
}
{PLUS} {
    fprintf(pscanner,"PLUS\n");
    return PLUS;
}
{STAR} {
    fprintf(pscanner,"STAR\n");
    return STAR;
}
{DIV} {
    fprintf(pscanner,"DIV\n");
    return DIV;
}
{AND} {
    fprintf(pscanner,"AND\n");
    return AND;
}
{OR} {
    fprintf(pscanner,"OR\n");
    return OR;
}
{DOT} {
    fprintf(pscanner,"DOT\n");
    return DOT;
}
{NOT} {
    fprintf(pscanner,"NOT\n");
    return NOT;
}
{LP} {
    fprintf(pscanner,"LP\n");
    return LP;
}
{RP} {
    fprintf(pscanner,"RP\n");
    return RP;
}
{LB} {
    fprintf(pscanner,"LB\n");
    return LB;
}
{RB} {
    fprintf(pscanner,"RB\n");
    return RB;
}
{LC} {
    fprintf(pscanner,"LC\n");
    return LC;
}
{RC} {
    fprintf(pscanner,"RC\n");
    return RC;
}
{STRUCT} {
    fprintf(pscanner,"STRUCT\n");
    return STRUCT;
}
{RETURN} {
    fprintf(pscanner,"RETURN\n");
    return RETURN;
}
{IF} {
    fprintf(pscanner,"IF\n");
    return IF;
}
{ELSE} {
    fprintf(pscanner,"ELSE\n");
    return ELSE;
}
{WHILE} {
    fprintf(pscanner,"WHILE\n");
    return WHILE;
}
{ID} {
    fprintf(pscanner,"ID: %s\n",yytext);
    yylval.type_string = (char*)malloc(strlen(yytext)+1);
    strcpy(yylval.type_string,yytext);
    return ID;
}
.   {
    printf("Error type [A] at Line [%d]: Mysterious characters \'%s\'\n",yylineno,yytext);
}
%%
void comments(){
    char c;
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
