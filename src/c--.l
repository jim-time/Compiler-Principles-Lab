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
    #include"../include/SyntaxTree.h"
    #define SCANNER_OUTPUT 0

    int int_num;
    float float_num;
    union Node_Data node_data;
    int n_line;
    /*multiline comment*/
    #define NORMAL 0
    #define FOUND_SOC 1
    char c_comment;
    int comment_state = NORMAL;
    int quit_while = 0;

    /*yy's variable*/
    int yycolumn = 1;
    YYLTYPE yylloc;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    /*output lex to the scanneroutput.txt*/
    extern FILE *pscanner;  
    extern int error_hint;
%}
ID  [_a-zA-Z][_a-zA-Z0-9]*
INT_DEC ([1-9][0-9]*)|[0]
INT_OCT [0][0-7]+
INT_HEX [0][Xx]([0-9a-fA-F]+)
INT {INT_DEC}|{INT_OCT}|{INT_HEX}
FLOAT ([0-9]*\.[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?[fF]?
START_COMMENTS "/*"
END_COMMENTS "*/"
SINGLE_COMMENTS (\/\/.*)
SPACE [ \t]+
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
    error_hint = 1;
}
(0[xX]([0-9a-fA-F]*[g-zG-Z][0-9a-fA-F]*)+) {
    printf("Error type [A] at Line [%d]: Illegal hexadecimal number \'%s\'\n",yylineno,yytext);    
    error_hint = 1;
}
((\.([eE][+-]?[0-9]*[fF]?))|(([0-9]*\.?[0-9]+|[0-9]+\.)[eE][+-]?[fF]?)) {
    printf("Error type [A] at Line [%d]: Illegal floating point number \'%s\'\n",yylineno,yytext);    
    error_hint = 1;
}
{SPACE} {
}
{TYPE} {
#if SCANNER_OUTPUT
    fprintf(pscanner,"TYPE: %s\n",yytext);
#endif
    node_data.string_value = (char*)malloc(strlen(yytext)+1);
    strcpy(node_data.string_value,yytext);
    yylval.type_node = CreateNode(NULL,"TYPE",node_data,yylineno);
    return TYPE;
}
{MINUS} {
#if SCANNER_OUTPUT
    fprintf(pscanner,"MINUS\n");
#endif   
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"MINUS",node_data,yylineno);
    return MINUS;
}
{INT} {
    if(yytext[0] == '0'){   
        if(yyleng > 1){
            if(yytext[1] == 'x' || yytext[1] == 'X'){   /*recognize the hexadecimal format*/
                sscanf(yytext+2,"%x",&int_num);
            }else {     /*recognize the octal format*/
                sscanf(yytext+1,"%o",&int_num);
            }
        }else{
            /*recognize the zero*/
            sscanf(yytext,"%d",&int_num);  
        }
    }else{
        /*recognize the decimal format*/
        sscanf(yytext,"%d",&int_num);  
    }
#if SCANNER_OUTPUT
        fprintf(pscanner,"INT: %d\n",int_num);  
#endif
    node_data.int_value = int_num;
    yylval.type_node = CreateNode(NULL,"INT",node_data,yylineno);
    return INT;
}
\n {
    n_line++;
    yycolumn = 1;
}
{FLOAT} {
    sscanf(yytext,"%f",&float_num);
#if SCANNER_OUTPUT
    fprintf(pscanner,"FLOAT: %.6f\n",float_num);
#endif
    node_data.float_value = float_num;
    yylval.type_node = CreateNode(NULL,"FLOAT",node_data,yylineno);
    return FLOAT;
}
{SINGLE_COMMENTS} {
#if SCANNER_OUTPUT
        fprintf(pscanner,"SINGLE_COMMENTS @line [%d]\n",yylineno);
#endif 
}
{START_COMMENTS} {
    if(yytext[0] == '/' && yytext[1] == '*'){
        while((c_comment = input())!=0){
            switch(comment_state){
                case NORMAL:
                    if(c_comment == '*'){
                        c_comment = input();
                        if(c_comment == '/'){
                            #if SCANNER_OUTPUT
                            fprintf(pscanner,"End of comments @ line [%d]\n",yylineno);
                            #endif 
                            quit_while = 1;
                        }else
                        unput(c_comment);
                    }else if(c_comment == '/'){
                        c_comment = input();
                        if(c_comment == '*'){
                            comment_state = FOUND_SOC;
                        }else
                            unput(c_comment);
                    }
                    break;
                case FOUND_SOC:
                    if(c_comment == '*'){
                        c_comment = input();
                        if(c_comment == '/'){
                            #if SCANNER_OUTPUT
                            fprintf(pscanner,"End of cmments @ line [%d]\n",yylineno);
                            #endif 
                            quit_while = 1;
                        }else{
                            unput(c_comment);
                        }
                    }
                    break;
            }
            if(quit_while){
                quit_while = 0;
                break;
            } 
        }
    }
}
{END_COMMENTS} {
    switch(comment_state){
        case NORMAL:
            printf("Error type [B] at Line [%d]: Missing '/*'\n",yylineno);
            break;
        case FOUND_SOC:
            printf("Error type [B] at Line [%d]: Syntax error, unexpected nested comment\n",yylineno);
            comment_state = NORMAL;
            break;
    }
    error_hint = 1;
}
{SEMI} {
#if SCANNER_OUTPUT
    fprintf(pscanner,"SEMI\n");
#endif
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"SEMI",node_data,yylineno);
    return SEMI;
}
{COMMA} {
#if SCANNER_OUTPUT
    fprintf(pscanner,"COMMA\n");
#endif
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"COMMA",node_data,yylineno);
    return COMMA;
}
{ASSIGNOP} {
#if SCANNER_OUTPUT
    fprintf(pscanner,"ASSIGNOP\n");
#endif
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"ASSIGNOP",node_data,yylineno);
    return ASSIGNOP;
}
{RELOP} {
#if SCANNER_OUTPUT
    fprintf(pscanner,"RELOP\n");
#endif    
    node_data.string_value = (char*)malloc(strlen(yytext)+1);
    strcpy(node_data.string_value,yytext);
    yylval.type_node = CreateNode(NULL,"RELOP",node_data,yylineno);
    return RELOP;
}
{PLUS} {
#if SCANNER_OUTPUT   
    fprintf(pscanner,"PLUS\n");
#endif 
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"PLUS",node_data,yylineno);
    return PLUS;
}
{STAR} {
#if SCANNER_OUTPUT       
    fprintf(pscanner,"STAR\n");
#endif    
    node_data.int_value = 0; 
    yylval.type_node = CreateNode(NULL,"STAR",node_data,yylineno);
    return STAR;
}
{DIV} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"DIV\n");
#endif    
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"DIV",node_data,yylineno);    
    return DIV;
}
{AND} {
#if SCANNER_OUTPUT        
    fprintf(pscanner,"AND\n");
#endif     
    node_data.string_value = (char*)malloc(strlen(yytext)+1);
    strcpy(node_data.string_value,yytext);
    yylval.type_node = CreateNode(NULL,"AND",node_data,yylineno);
    return AND;
}
{OR} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"OR\n");
#endif     
    node_data.string_value = (char*)malloc(strlen(yytext)+1);
    strcpy(node_data.string_value,yytext);
    yylval.type_node = CreateNode(NULL,"OR",node_data,yylineno);
    return OR;
}
{DOT} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"DOT\n");
#endif   
    node_data.int_value = 0; 
    yylval.type_node = CreateNode(NULL,"DOT",node_data,yylineno);
    return DOT;
}
{NOT} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"NOT\n");
#endif     
    node_data.string_value = (char*)malloc(strlen(yytext)+1);
    strcpy(node_data.string_value,yytext);
    yylval.type_node = CreateNode(NULL,"NOT",node_data,yylineno);
    return NOT;
}
{LP} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"LP\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"LP",node_data,yylineno);
    return LP;
}
{RP} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"RP\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"RP",node_data,yylineno);
    return RP;
}
{LB} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"LB\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"LB",node_data,yylineno);
    return LB;
}
{RB} {
#if SCANNER_OUTPUT         
    fprintf(pscanner,"RB\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"RB",node_data,yylineno);
    return RB;
}
{LC} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"LC\n");
#endif 
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"LC",node_data,yylineno);    
    return LC;
}
{RC} {
#if SCANNER_OUTPUT     
    fprintf(pscanner,"RC\n");
#endif 
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"RC",node_data,yylineno);   
    return RC;
}
{STRUCT} {
#if SCANNER_OUTPUT         
    fprintf(pscanner,"STRUCT\n");
#endif
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"STRUCT",node_data,yylineno);
    return STRUCT;
}
{RETURN} {
#if SCANNER_OUTPUT        
    fprintf(pscanner,"RETURN\n");
#endif    
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"RETURN",node_data,yylineno);
    return RETURN;
}
{IF} {
#if SCANNER_OUTPUT         
    fprintf(pscanner,"IF\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"IF",node_data,yylineno);
    return IF;
}
{ELSE} {
#if SCANNER_OUTPUT         
    fprintf(pscanner,"ELSE\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"ELSE",node_data,yylineno);
    return ELSE;
}
{WHILE} {
#if SCANNER_OUTPUT         
    fprintf(pscanner,"WHILE\n");
#endif     
    node_data.int_value = 0;
    yylval.type_node = CreateNode(NULL,"WHILE",node_data,yylineno);
    return WHILE;
}
{ID} {
#if SCANNER_OUTPUT         
    fprintf(pscanner,"ID: %s\n",yytext);
#endif     

    node_data.string_value = (char*)malloc(strlen(yytext)+1);
    strcpy(node_data.string_value,yytext);
    yylval.type_node = CreateNode(NULL,"ID",node_data,yylineno);
    return ID;
}
.   {
    printf("Error type [A] at Line [%d]: Mysterious characters \'%s\'\n",yylineno,yytext);
    error_hint = 1;
}
%%
