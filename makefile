objects := SyntaxTree.c main.c lex.yy.c c--.tab.c 
parser: $(objects)
	cc $(objects)  -Wall -lfl -ly -o ./Output/parser

SyntaxTree.o: SyntaxTree.c SyntaxTree.h
	cc -o $@ $<

main.o:	main.c lex.yy.c c--.tab.c SyntaxTree.h
	cc -o $@ $^

lex.yy.c : c--.tab.h c--.tab.c c--.l SyntaxTree.h
	flex c--.l

c--.tab.h :
c--.tab.c : c--.y c--.l SyntaxTree.h 
	bison -d -v -t c--.y

.PHONY:	clean
clean:
	rm ./Output/parser *.o lex.yy.c c--.tab.c c--.tab.h