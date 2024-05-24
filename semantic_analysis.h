/*
* h file for semantic_analysis.c
*
* @author: Will Kalikman
*/

#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include <string>
#include <vector>
#include <stack>
#include "ast.h"

using SymbolTable = std::vector<char *>;

/**
 * This function recursively traverses AST nodes by starting with the root node and 
 * performs semantic analysis. Implements two rules per the spec:
 * -A variable is declared before it is used
 * -There is only one declaration of the variable in a scope
 * @param node is a pointer to the AST node being visited (initially root, then recursively all other nodes).
 * @param symbolTableStack is a reference to the symbol table stack.
 * returns: true if function succesfully traversed AST and performed semantic analysis
 * false otherwise
 */
bool visitNode(astNode* node, stack<SymbolTable>& symbolTableStack);


#endif