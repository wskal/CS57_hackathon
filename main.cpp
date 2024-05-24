#include "inclass.h"
#include "semantic_analysis.h"
#include "ast.h"
#include <cstdio>
#include <stack>
#include <vector>

extern FILE *yyin;             // Declare yyin as an external variable
extern int yyparse();          // Declare yyparse as an external function
extern void yylex_destroy();   // Declare yylex_destroy as an external function
extern astNode *rootNode;      // Declare rootNode as an external variable

using SymbolTable = std::vector<char *>;

int main(int argc, char* argv[]){
    astNode *progNode1 = NULL;
    astNode *progNode2 = NULL;

    if (argc >= 2) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
            fprintf(stderr, "File open error\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
        return 1;
    }

    #ifdef YYDEBUG
    yydebug = 1;
    #endif

    yyparse();

    if (rootNode == NULL) {
        fprintf(stderr, "root is null\n");
        return 1;
    }
    progNode1 = rootNode;

    std::stack<SymbolTable> symbolTableStack;
    if (rootNode != NULL) {
        if (!visitNode(rootNode, symbolTableStack)) {
            fprintf(stderr, "didn't visit root node\n");
            freeNode(rootNode);
            return 1;
        } else {
            freeNode(rootNode);
        }
    }  

    fclose(yyin);
    yylex_destroy();

    // second file 
    if (argc == 3) {
        yyin = fopen(argv[2], "r");
        if (yyin == NULL) {
            fprintf(stderr, "File open error\n");
            return 1;
        }

        #ifdef YYDEBUG
        yydebug = 1;
        #endif

        yyparse();

        if (rootNode == NULL) {
            fprintf(stderr, "root is null\n");
            return 1;
        }
        progNode2 = rootNode;

        symbolTableStack = std::stack<SymbolTable>();
        if (rootNode != NULL) {
            if (!visitNode(rootNode, symbolTableStack)) {
                fprintf(stderr, "didn't visit root node\n");
                freeNode(rootNode);
                return 1;
            } else {
                freeNode(rootNode);
            }
        }  

        fclose(yyin);
        yylex_destroy();
    }

    int score = compareTrees(progNode1, progNode2);
    printf("Differential score is: %d\n", score);
    free(progNode1);
    free(progNode2);

    return 0;
}
