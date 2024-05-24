/*
*	MiniC Compiler - Lexical Analyzer
*
* 	Purpose: This lexical file helps define and recognize the set of tokens used in the input file to be 
* 	parsed in the Bison file. 
*
* 	Author: Carly Retterer
* 	Date: 4/16/2024
*/

%{
	#include <stdio.h>
	#include "ast.h"
	#include "yacc.tab.h"
	#include <string.h>
%}

%%
"extern" {return EXTERN;}
"void" {return VOID;}
"int"  {return INT;}
"while" {return WHILE;}
"if" {return IF;}
"else" {return ELSE;}
"return" {return RETURN;}
"read" {return READ;}
"print" {return PRINT;}


">"									  {return GT;}
"<"									  {return LT;}
"=="									{return EQ;}
">="									{return GTE;}
"<="								  {return LTE;}

"+" {return PLUS;}
"-" {return MINUS;}
"*" {return MULT;}
"/" {return DIV;}


"=" {return EQUALS;}

[a-zA-Z][a-zA-Z0-9_]*	{ yylval.sname = strdup(yytext);
													return ID;}
[0-9]*					{ yylval.ival = atoi(yytext);
													return NUM;}

[ \t\n]
.										{return yytext[0];}
%%

int yywrap(){
	return 1;
}

