.PHONY:clean
CC := gcc
AR := ar
LD := ld
CFLAGS = -Wall -o 
LDFLAGS := -lfl -ly

PWD := $(shell pwd)
INC_DIR  := $(PWD)/include
INCLUDE_DIRS := $(PWD)/include $(PWD)/include/uthash/include
INC_DIRS := $(addprefix -I, $(INCLUDE_DIRS))
SRC_DIR := $(PWD)/src
BIN_DIR := $(PWD)/build/bin
OBJ_DIR := $(PWD)/build/obj
TEST_DIR := $(PWD)/tests

RM = -rm
MAKE = make

OBJECTS := $(OBJ_DIR)/SyntaxTree.o $(OBJ_DIR)/main.o $(OBJ_DIR)/TypeCheck.o $(OBJ_DIR)/SymbolTable.o \
$(OBJ_DIR)/SemanticAnalysis.o $(OBJ_DIR)/IntermediateCode.o $(OBJ_DIR)/CodeGenerator.o $(OBJ_DIR)/RegAllocation.o\
$(OBJ_DIR)/my_vector.o $(OBJ_DIR)/my_list.o $(OBJ_DIR)/Bitmap.o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/c--.tab.c

TARGET := parser
CFLAGS := -c -g
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	cc $(INC_DIRS) $(OBJECTS)  $(LDFLAGS) -Wall -o $@

$(OBJ_DIR)/SyntaxTree.o: $(SRC_DIR)/SyntaxTree.c $(INC_DIR)/SyntaxTree.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/my_vector.o: $(SRC_DIR)/my_vector.c $(INC_DIR)/my_vector.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/my_list.o: $(SRC_DIR)/my_list.c $(INC_DIR)/my_list.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/Bitmap.o: $(SRC_DIR)/Bitmap.c $(INC_DIR)/Bitmap.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/SymbolTable.o: $(SRC_DIR)/SymbolTable.c $(INC_DIR)/SymbolTable.h $(INC_DIR)/my_vector.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $^

$(OBJ_DIR)/SemanticAnalysis.o: $(SRC_DIR)/SemanticAnalysis.c $(INC_DIR)/SemanticAnalysis.h $(INC_DIR)/SyntaxTree.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/IntermediateCode.o: $(SRC_DIR)/IntermediateCode.c $(INC_DIR)/IntermediateCode.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/TypeCheck.o: $(SRC_DIR)/TypeCheck.c
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/CodeGenerator.o: $(SRC_DIR)/CodeGenerator.c $(INC_DIR)/CodeGenerator.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/RegAllocation.o: $(SRC_DIR)/RegAllocation.c $(INC_DIR)/RegAllocation.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $<

$(OBJ_DIR)/main.o:	$(SRC_DIR)/main.c $(SRC_DIR)/lex.yy.c $(SRC_DIR)/c--.tab.c $(INC_DIR)/SyntaxTree.h
	cd $(OBJ_DIR) && cc $(INC_DIRS) $(CFLAGS) $^

$(SRC_DIR)/lex.yy.c : $(SRC_DIR)/c--.tab.h $(SRC_DIR)/c--.tab.c $(SRC_DIR)/c--.l $(INC_DIR)/SyntaxTree.h
	cd $(SRC_DIR) && flex $(SRC_DIR)/c--.l

$(SRC_DIR)/c--.tab.h :
$(SRC_DIR)/c--.tab.c : $(SRC_DIR)/c--.y $(SRC_DIR)/c--.l $(INC_DIR)/SyntaxTree.h 
	cd $(SRC_DIR) && bison -d $(SRC_DIR)/c--.y

test:
	$(shell $(BIN_DIR)/$(TARGET) $(TEST_DIR)/*)
	@echo "Done! Leave the code.s and out.ir in current directory"
	@echo "Try the code.s on qtspim!"
.PHONY:	clean
clean:
	$(RM)  $(OBJ_DIR)/*.o $(SRC_DIR)/*.tab.*  $(SRC_DIR)/lex.yy.c $(BIN_DIR)/$(TARGET) *.s *.ir
