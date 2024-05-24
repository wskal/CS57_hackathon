/*
*	MiniC Compiler - Bison Grammar File
*
*   Purpose: In this Bison file, we handle grammar rules and include a main function where we call yyparse(); to parse our
*   input file. We then build the AST and conduct semantic analysis on the tree in the semantic_analysis.c file, using an algorithm given by
*   Vasanta. We also free all memory allocated to the tree using Vasanta's ast library. 
* 
* 	Author: Carly Retterer
* 	Date: 4/16/2024
*/

%{
#include <stdio.h>
#include "ast.h"
#include <vector>
#include <stack>
#include "semantic_analysis.h"
#include <string>

extern int yylex();
extern int yylex_destroy();
extern int yywrap();
int yyerror(const char *);
extern FILE * yyin;
using SymbolTable = std::vector<std::string>;
astNode* rootNode = NULL;

%}

%union{
    int ival;
    char *sname;
    astNode *nptr;
    vector<astNode*> *svec_ptr;
}

%token <ival> NUM
%token <sname> ID
%token PLUS MINUS MULT DIV INT
%token EXTERN VOID IF ELSE WHILE RETURN READ PRINT
%token EQ GT LT GTE LTE EQUALS

%nonassoc THEN
%nonassoc ELSE
%nonassoc EQ GT LT GTE LTE EQUALS
%left PLUS MINUS
%left MULT DIV

%type <svec_ptr> stmts var_decls
%type <nptr> stmt expr term block_stmt decl func cond_expr prog extern_list print  //non-terminals
%start prog


%%

// function node: followed by func name, possible param, and block stmt
func : INT ID '(' ')' block_stmt {
    $$ = createFunc($2, NULL, $5);
    if ($$ == NULL) {
        yyerror("Failed to create function node due to memory allocation failure.");
        YYABORT;
    }
    printf("non-parametric function created\n");
    free($2);
	}

     | INT ID '(' INT ID ')' block_stmt {
    $$ = createFunc($2, createVar($5), $7);
    if ($$ == NULL || $7 == NULL) {
        yyerror("Failed to create function node with parameters due to memory allocation failure.");
        YYABORT;
    }
    printf("Function with parameters created\n");
    free($2);
    free($5);
}

// program node : can be followed by extern read and extern print
prog : extern_list extern_list func {
    printf("Creating program node...\n");
    $$ = createProg($1, $2, $3);
    printf("createProg returned: %p\n", $$);  // Assuming $$ is a pointer
    if ($$ == NULL) {
        yyerror("Failed to create program node due to memory allocation failure.");
        YYABORT;
    }
    rootNode = $$;
    if (rootNode == NULL) {
        fprintf(stderr, "Error: rootNode wasn't initialized at the beginning\n");
    }
}

// extern_list non terminal followed by extern print node or possible extern read node
extern_list : EXTERN VOID PRINT '(' INT ')' ';' {$$ = createExtern("print");}
            | EXTERN INT READ '(' ')' ';' {$$ = createExtern("read");} 

// block stmt code taken from ex given by Vasanta, with modifications for debugging purposes and errors checks
block_stmt : '{' var_decls stmts '}' {
    vector<astNode*>* new_vec = new (nothrow) vector<astNode*>();
    if (!new_vec) {
        yyerror("Failed to allocate memory for block statement.");
        YYABORT;
    }
    new_vec->insert(new_vec->end(), $2->begin(), $2->end());
    new_vec->insert(new_vec->end(), $3->begin(), $3->end());
    $$ = createBlock(new_vec);
    if ($$ == NULL) {
        delete new_vec;  // Clean up vector if block creation fails
        yyerror("Failed to create block node due to memory allocation failure.");
        YYABORT;
    }
    printNode($$);
    delete $2;
    delete $3;
    printf("block created\n");
}
            | '{' stmts '}' {
    $$ = createBlock($2);
    if ($$ == NULL) {
        yyerror("Failed to create block node due to memory allocation failure.");
        YYABORT;
    }
    printNode($$);
    printf("Simple block created\n");
}

// var declarations, with code given by Vasanta
var_decls	 : var_decls decl {$$ = $1;
														 $$->push_back($2);}
					 | decl {$$ = new vector<astNode*>();
									 $$->push_back($1);}

// decl nodes with debugging checks
decl : INT ID ';' {
    $$ = createDecl($2);
    if ($$ == NULL) {
        yyerror("Failed to create declaration node due to memory allocation failure.");
        YYABORT;
    }
    free($2);
}

// statement nodes with code given by Vasanta, modified for debugging purposes
stmts : stmts stmt {
    $1->push_back($2);
    $$ = $1;
}
       | stmt {
    $$ = new (nothrow) vector<astNode*>();
    if (!$$) {
        yyerror("Failed to allocate memory for statements.");
        YYABORT;
    }
    $$->push_back($1);
}

//print non terminal					 
print : PRINT '(' expr ')' ';' {$$ = createCall("print", $3);}

// possible stmts 
stmt 			: IF '(' cond_expr ')' stmt %prec THEN {$$ = createIf($3, $5);}
     				| IF '(' cond_expr ')' stmt ELSE stmt {$$ = createIf($3, $5, $7);}
     				| WHILE '(' cond_expr ')' block_stmt {$$ = createWhile($3, $5);}
     				| RETURN ';' {$$ = createRet(NULL);}
     				| RETURN '(' expr ')' ';' {$$ = createRet($3);}
                    | RETURN expr ';' {$$ = createRet($2);}
     				| block_stmt {$$ = $1;}
     				| ID EQUALS expr ';' {astNode* tnptr = createVar($1); $$ = createAsgn(tnptr, $3); free($1);}
					| print {$$ = $1;}
     				;

cond_expr 		: expr EQ expr {$$ = createRExpr($1, $3, eq);}
          			| expr GT expr {$$ = createRExpr($1, $3, gt);}
          			| expr LT expr {$$ = createRExpr($1, $3, lt);}
          			| expr GTE expr {$$ = createRExpr($1, $3, ge);}
          			| expr LTE expr {$$ = createRExpr($1, $3, le);}
          			;

expr			 : term PLUS term {$$ = createBExpr($1, $3, add);}
					 | term MINUS term {$$ = createBExpr($1, $3, sub);}
					 | term MULT term {$$ = createBExpr($1, $3, mul);}
					 | term DIV term {$$ = createBExpr($1, $3, divide);}
                     | READ '(' ')' {$$ = createCall("read", NULL);}
					 | term {$$ = $1;}
                     ;

term			 : NUM {$$ = createCnst($1);}
					 | ID {$$ = createVar($1); free($1);}
					 | MINUS term {$$ = createUExpr($2, uminus);}

%%
int yyerror(const char *s){
	fprintf(stderr,"%s\n", s);
	return 0;
}