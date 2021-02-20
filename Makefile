CC = g++

CONFIG = llvm-config
LLVM_FLAGS = --cxxflags --ldflags --libs
LIB_FLAG = -pthread

OBJS = main.o node.o token.o parser.o CodeGen.o

compile: $(OBJS)
	$(CC) $(LIB_FLAG) -g $(OBJS) `$(CONFIG) $(LLVM_FLAGS)` -ldl -o compile

main.o : main.cpp node.h parser.hpp CodeGen.h
	$(CC) $(LIB_FLAG) -g main.cpp `$(CONFIG) $(LLVM_FLAGS)` -ldl -c -o main.o

node.o : node.cpp node.h
	$(CC) $(LIB_FLAG) -g node.cpp `$(CONFIG) $(LLVM_FLAGS)` -ldl -c -o node.o

token.o : token.cpp token.hpp parser.hpp
	$(CC) $(LIB_FLAG) -g token.cpp `$(CONFIG) $(LLVM_FLAGS)` -ldl -c -o token.o

parser.o : parser.cpp parser.hpp
	$(CC) $(LIB_FLAG) -g parser.cpp -DYYERROR_VERBOSE `$(CONFIG) $(LLVM_FLAGS)` -ldl -c -o parser.o

CodeGen.o : CodeGen.cpp CodeGen.h node.h
	$(CC) $(LIB_FLAG) -g CodeGen.cpp `$(CONFIG) $(LLVM_FLAGS)` -ldl -c -o CodeGen.o


parser.cpp: parser.y node.h Symbol_Table.h
	bison -d -o parser.cpp parser.y

parser.hpp: parser.cpp

token.cpp: token.l parser.hpp
	lex -o token.cpp token.l

token.hpp: token.cpp



clean:
	rm $(OBJS) compile

run:
	./compile ./myc_src_bin/test.dc ./test.ll
