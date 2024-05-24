// inclass.h
#ifndef INCLASS_H
#define INCLASS_H

#include "ast.h"
#include <cstdio>



// Function declarations
int compareTrees(astNode *rootnode1, astNode *rootnode2);
int compare(astNode *node1, astNode *node2, int score);

#endif // INCLASS_H
