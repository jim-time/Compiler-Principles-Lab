objects = main.c lex.yy.c c--.tab.c SyntaxTree.c
parser: $(objects)
	cc $(objects)  -Wall -lfl -ly -o parser

SyntaxTree.o: SyntaxTree.c SyntaxTree.h
	cc -o SyntaxTree.o SyntaxTree.c
lex.yy.c : c--.tab.h c--.l
	flex c--.l
c--.tab.c : c--.y 
	bison -d -v -t c--.y

clean:
	rm parser $(objects)