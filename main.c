/*
 *  @file   main.c
 *  @brief  
 *  @author jim
 *  @date   2018-5-4
 *  @version v1.0
**/

#include <stdio.h>

extern FILE *yyin;
extern int yylex (void);
extern void yyrestart(FILE * input_file );
extern int yyparse (void);

FILE *pscanner;
int main(int argc, char** argv) {
    FILE *pfile;
    if (argc > 1) {
        if (!(pfile = fopen(argv[1], "r+"))) {
            perror(argv[1]);
        return 1;
        }
    }
    pscanner = fopen("scanneroutput.txt","r+");
    yyrestart(pfile);
    yyparse();
   // yylex();
    fclose(pscanner);
    return 0;
}

