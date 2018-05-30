OUTDIR := ./Output
objects := SyntaxTree.o main.o TypeCheck.o SymbolTable.o SemanticAnalysis.o my_vector.o lex.yy.c c--.tab.c
CFLAGS := -c -g
parser: $(objects)
	cc -g $(objects)  -Wall -lfl -ly -o parser

SyntaxTree.o: SyntaxTree.c SyntaxTree.h
	cc $(CFLAGS) $<

main.o:	main.c lex.yy.c c--.tab.c SyntaxTree.h
	cc $(CFLAGS) $^

my_vector.o: my_vector.c my_vector.h
	cc $(CFLAGS) $<

SymbolTable.o: SymbolTable.c SymbolTable.h my_vector.h
	cc $(CFLAGS) $^

SemanticAnalysis.o: SemanticAnalysis.c SemanticAnalysis.h SyntaxTree.h
	cc $(CFLAGS) $<

lex.yy.c : c--.tab.h c--.tab.c c--.l SyntaxTree.h
	flex c--.l

c--.tab.h :
c--.tab.c : c--.y c--.l SyntaxTree.h 
	bison -d -v -t c--.y

.PHONY:	clean
clean:
	rm  *.o *.output lex.yy.c *.tab.* *.gch