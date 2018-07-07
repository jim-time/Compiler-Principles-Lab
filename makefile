OUTDIR := ./Output
OBJECTS := SyntaxTree.o main.o TypeCheck.o SymbolTable.o SemanticAnalysis.o IntermediateCode.o CodeGenerator.o RegAllocation.o my_vector.o my_list.o Bitmap.o lex.yy.c c--.tab.c
TARGET := parser
CFLAGS := -c -g
$(TARGET): $(OBJECTS)
	cc -g $(OBJECTS)  -Wall -lfl -ly -o $(TARGET)

SyntaxTree.o: SyntaxTree.c SyntaxTree.h
	cc $(CFLAGS) $<

main.o:	main.c lex.yy.c c--.tab.c SyntaxTree.h
	cc $(CFLAGS) $^

my_vector.o: my_vector.c my_vector.h
	cc $(CFLAGS) $<

my_list.o: my_list.c my_list.h
	cc $(CFLAGS) $<

Bitmap.o: Bitmap.c Bitmap.h
	cc $(CFLAGS) $<

SymbolTable.o: SymbolTable.c SymbolTable.h my_vector.h
	cc $(CFLAGS) $^

SemanticAnalysis.o: SemanticAnalysis.c SemanticAnalysis.h SyntaxTree.h
	cc $(CFLAGS) $<

IntermediateCode.o: IntermediateCode.c IntermediateCode.h
	cc $(CFLAGS) $<

CodeGenerator.o: CodeGenerator.c CodeGenerator.h
	cc $(CFLAGS) $<

RegAllocation.o: RegAllocation.c RegAllocation.h
	cc $(CFLAGS) $<

lex.yy.c : c--.tab.h c--.tab.c c--.l SyntaxTree.h
	flex c--.l

c--.tab.h :
c--.tab.c : c--.y c--.l SyntaxTree.h 
	bison -d -v -t c--.y

.PHONY:	clean
clean:
	rm  *.o *.output lex.yy.c *.tab.* *.gch $(TARGET)