/* 
*   MiniC Compiler - Semantic Analysis
*
*   Purpose: This file is the implementation of the Semantic Analysis for the MiniC compiler. It runs through the AST and
*   to perform semantic checks and constuct a stack of symbol tables. The purpose of the semantic checks is to ensure proper scope
*   and declaration of variables. In other words, it ensures that the input file abides by two rules: a variable is declared before it is used
*   and there is only one declaration of the variable in a scope. 
*
*   Author: Carly Retterer
*   Date: 4/16/2024
*/

#include <stdio.h>
#include <vector>
#include <stack>
#include "ast.h"
#include "semantic_analysis.h"
#include <algorithm>
#include <string>
#include <cstring>

using SymbolTable = vector<char *>;  // symbol table data structure 

bool visitNode(astNode* node, stack<SymbolTable>& symbolTableStack);

bool visitNode(astNode* node, stack<SymbolTable>& symbolTableStack){
    //checking if node passed is a null pointer
    if(node == nullptr){
        fprintf(stderr, "Error: node is null.\n");
        return false;
    }
    // if the node is a block statement node and is body of a function: 
    // get the top symbol table from the stack and use it as the curr_sym_table 
    // visit all nodes in the statement list of block statement 
    // pop top of the stack
    if (node->type == ast_func) {
            astNode* blockNode = node->func.body;
            SymbolTable curr_sym_table;
            if (node->func.param != nullptr) {
                curr_sym_table.push_back(node->func.param->var.name);
                printf("Size of : %ld\n", curr_sym_table.size());
            }
            symbolTableStack.push(curr_sym_table);
            if (blockNode->stmt.block.stmt_list != nullptr) {
                for (astNode* stmt : *(blockNode->stmt.block.stmt_list)) {
                    // handling possible errors where the stmt is a null pointer
                    if (stmt == nullptr) {
                        fprintf(stderr, "Statement node is null\n");
                        continue;
                    }
                    visitNode(stmt, symbolTableStack);
                }
            }
            symbolTableStack.pop();
    }


    // if the node is a block statement node: 
    // create a new symbol table curr_sym_table and push it to the symbol table stack
    // visit all nodes in the statement list of block statement 
    // pop top of the stack
    if (node->type == ast_stmt && node->stmt.type == ast_block){
        SymbolTable curr_sym_table;
        symbolTableStack.push(curr_sym_table);
        if(node->stmt.block.stmt_list != NULL){
            for (astNode* stmt : *(node->stmt.block.stmt_list)) {
                // handling possible errors where the stmt is a null pointer
                if(stmt == nullptr){
                    fprintf(stderr, "Statement node is null.\n");
                    return false;
                }
                visitNode(stmt, symbolTableStack);
            }
        }
        symbolTableStack.pop();
    }



    // if the node is a declaration statement, check if the variable is in the symbol table 
    // at the top of the stack. If it does, then emit an error message. 
    // Otherwise, add the variable to the symbol table at the top of the stack.
    if(node->type == ast_stmt && node->stmt.type == ast_decl){
        if (!symbolTableStack.empty()) {
            //find symbol table at top of stack
            SymbolTable& curr_table = symbolTableStack.top();
            printf("size of current table : %ld %ld\n", symbolTableStack.size(), curr_table.size());
            char* declName = node->stmt.decl.name;
            // making sure declName is not null
            if(declName == NULL){
                fprintf(stderr, "Declaration name is null.\n");
                return false;
            }
            //iterate through symbol table
            for (auto it : curr_table) {
                if (strcmp(it, declName)==0) {
                    printf("Error: Variable has already been declared: '%s'\n", declName);
                    return false;
                }
            }
            curr_table.push_back(declName);
            //printf("size of current table : %ld %ld\n", symbolTableStack.size(), curr_table.size());
        }
    }

    // if the node is a variable node, check if it appears in one of the symbol tables on the stack. 
    // If it does not, then emit an error message with name of the variable.
    if(node->type == ast_var) {
        char* varName = node->var.name;
        // checking to make sure variable name isn't null
        if(varName == NULL){
            fprintf(stderr, "varName is null\n");
            return false;
        }
        bool ifFound = false;  // bool to keep track of whether or not variable name has been foumd
        stack<SymbolTable> curr_stack = symbolTableStack;  //initializing empty stack and copying current stack to it
        while(!curr_stack.empty()) {
            SymbolTable& tempTable = curr_stack.top();
            // iterating through every symbol table in stack
            for (auto it : tempTable) {
                if (strcmp(it, varName) == 0) {
                    ifFound = true;
                    break;
                }
            }
            curr_stack.pop();
        }  
        if (!ifFound) {
                printf("Error: Variable has not been declared. '%s'\n", varName);
                return false;
        }
    }

    //for all other node types, visit all the child nodes of the current node
    else {
            // program nodes
        if (node->type == ast_prog) {
            // children
            visitNode(node->prog.ext1, symbolTableStack);
            visitNode(node->prog.ext2, symbolTableStack);
            visitNode(node->prog.func, symbolTableStack);
        }
        // statement nodes
        else if (node->type == ast_stmt) {
            switch (node->stmt.type) {
                case ast_call:
                    if(node->stmt.call.param !=nullptr){
                        visitNode(node->stmt.call.param, symbolTableStack);
                        break;
                    }
                case ast_ret:
                    visitNode(node->stmt.ret.expr, symbolTableStack);
                    break;
                case ast_while:
                    visitNode(node->stmt.whilen.cond, symbolTableStack);
                    visitNode(node->stmt.whilen.body, symbolTableStack);
                    break;
                case ast_if:
                    visitNode(node->stmt.ifn.cond, symbolTableStack);
                    visitNode(node->stmt.ifn.if_body, symbolTableStack);
                    if (node->stmt.ifn.else_body != NULL) {
                        visitNode(node->stmt.ifn.else_body, symbolTableStack);
                    }
                    break;
                case ast_asgn:
                    visitNode(node->stmt.asgn.rhs, symbolTableStack);
                    visitNode(node->stmt.asgn.lhs, symbolTableStack);
                    break;
            }
        }
        // expr nodes
        else if (node->type == ast_rexpr) {
            visitNode(node->rexpr.lhs, symbolTableStack);
            visitNode(node->rexpr.rhs, symbolTableStack);
        }
        else if (node->type == ast_bexpr) {
            visitNode(node->bexpr.lhs, symbolTableStack);
            visitNode(node->bexpr.rhs, symbolTableStack);
        }
        else if (node->type == ast_uexpr) {
            visitNode(node->uexpr.expr, symbolTableStack);
        }
    }
    return true;
}