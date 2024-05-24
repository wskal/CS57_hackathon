# Define the compiler
CC = g++
CFLAGS = -Wall -g
INCLUDES = -I.

# Source and Object files
SRCS = inclass.cpp main.cpp yacc.tab.c lex.yy.c ast.c semantic_analysis.c
OBJS = inclass.o main.o yacc.tab.o lex.yy.o ast.o semantic_analysis.o

EXEC = inClassOut

# Flex and Bison files
FLEX = flex
BISON = bison

all: $(EXEC)

# Rule for linking the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(EXEC) $(OBJS)

# Rule for compiling .cpp source files
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Rule for compiling .c source files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Rule for compiling Bison files
yacc.tab.c yacc.tab.h: yacc.y
	$(BISON) -d yacc.y -o yacc.tab.c

# Rule for compiling Flex files
lex.yy.c: lex.l yacc.tab.h
	$(FLEX) -o lex.yy.c lex.l

# Test file 
test: $(EXEC)
	./$(EXEC) < parser_tests/p_bad.c

# Clean up
clean:
	rm -f $(OBJS) $(EXEC) yacc.tab.c yacc.tab.h lex.yy.c

# Phony targets
.PHONY: all clean test
