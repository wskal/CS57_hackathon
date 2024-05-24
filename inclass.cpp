#include "ast.h"
#include "cstdlib"
#include "inclass.h"
#include "semantic_analysis.h"

// BEGIN DEDUCTOR DEFINITIONS // 
const int NULL_NODE_DEDUCTOR = 2;
const int TYPE_MISMATCH_DEDUCTOR = 4;
const int LENGTH_MISMATCH_DEDUCTOR = 2;
const int PARAM_MISMATCH_DEDUCTOR = 3;
const int NAME_MISMATCH_DEDUCTOR = 2;
const int DECL_NAME_MISMATCH_DEDUCTOR = 3;
// END DEDUCTOR DEFINITIONS // 

// Helper compare function which will do the meat of the work 
int compare(astNode *node1, astNode *node2, int score) {
    // Base case 
    // return existing score as it is if both are null
    if (node1 == NULL and node2 == NULL) {
        return score;
    }
    // If one node is null then decrement by 2 (arbitrarily for now)
    if (node1 == NULL or node2 == NULL) {
        return score - NULL_NODE_DEDUCTOR;   
    }
    // if you the nodes you are currently comparing are of different types
    if (node1->type != node2->type) {
        return score - TYPE_MISMATCH_DEDUCTOR;
    }
    // if they are both functions
    if (node1->type == ast_func and node2->type == ast_func) {
        // Compare params if they dont match up then decrement score by 3
        if (node1->func.param != node2->func.param) {
            score -= PARAM_MISMATCH_DEDUCTOR;
        }
        // if both function bodies have block statements
        if (node1->func.body->type == ast_stmt and node2->func.body->type == ast_stmt and node1->func.body->stmt.type == ast_block and node2->func.body->stmt.type == ast_block) {
            // if they have different number of statements
            if (node1->func.body->stmt.block.stmt_list->size() != node2->func.body->stmt.block.stmt_list->size()) {
                int lengthdif = (node1->func.body->stmt.block.stmt_list->size() - node2->func.body->stmt.block.stmt_list->size());
                score -= LENGTH_MISMATCH_DEDUCTOR * abs(lengthdif);
            }
            // compare each statement in the block while it is within the bounds of the shortest list
            for (int i = 0; i < min(node1->func.body->stmt.block.stmt_list->size(), node2->func.body->stmt.block.stmt_list->size()); i++) {
                score = compare(node1->func.body->stmt.block.stmt_list->at(i), node2->func.body->stmt.block.stmt_list->at(i), score);
                return score;
            }
        } else {
            score = compare(node1->func.body, node2->func.body, score);
            return score;
        }
    }
    // if they are both block statements
    if (node1->type == ast_stmt and node2->type == ast_stmt and node1->stmt.type == ast_block and node2->stmt.type == ast_block) {
        // if they have different number of statements
        if (node1->stmt.block.stmt_list->size() != node2->stmt.block.stmt_list->size()) {
            int lengthdif = (node1->stmt.block.stmt_list->size() - node2->stmt.block.stmt_list->size());
            score -= LENGTH_MISMATCH_DEDUCTOR * abs(lengthdif);
        }
        // compare each statement in the block while it is within the bounds of the shortest list
        for (int i = 0; i < min(node1->stmt.block.stmt_list->size(), node2->stmt.block.stmt_list->size()); i++) {
            score = compare(node1->stmt.block.stmt_list->at(i), node2->stmt.block.stmt_list->at(i), score);
            return score;
        }
    }

    // if they are both declaration statements
    if (node1->type == ast_stmt and node2->type == ast_stmt and node1->stmt.type == ast_decl and node2->stmt.type == ast_decl) {
        if (node1->stmt.decl.name != node2->stmt.decl.name) {
            score -= DECL_NAME_MISMATCH_DEDUCTOR;
            return score;
        }
    }

    // if they are both variable nodes
    if (node1->type == ast_var and node2->type == ast_var) {
        if (node1->var.name != node2->var.name) {
            score -= NAME_MISMATCH_DEDUCTOR;
            return score;
        }
    }

    // if they are both extern nodes, ignore them
    if (node1->type == ast_extern and node2->type == ast_extern) {
        return score;
    }

    // if none of the above cases have been met, then we compare the children of the current node
    if (node1->type == ast_stmt and node2->type == ast_stmt) {
        if (node1->type == ast_call and node2->type == ast_call) {
            score = compare(node1->stmt.call.param, node2->stmt.call.param, score);
        } else if (node1->type == ast_ret and node2->type == ast_ret) {
            score = compare(node1->stmt.ret.expr, node2->stmt.ret.expr, score);
        } else if (node1->type == ast_while and node2->type == ast_while) {
            score = compare(node1->stmt.whilen.cond, node2->stmt.whilen.cond, score);
            score = compare(node1->stmt.whilen.body, node2->stmt.whilen.body, score);
        } else if (node1->type == ast_if and node2->type == ast_if) {
            score = compare(node1->stmt.ifn.cond, node2->stmt.ifn.cond, score);
            score = compare(node1->stmt.ifn.if_body, node2->stmt.ifn.if_body, score);
            if (node1->stmt.ifn.else_body != NULL) {
                score = compare(node1->stmt.ifn.else_body, node2->stmt.ifn.else_body, score);
            }
        } else if (node1->type == ast_asgn and node2->type == ast_asgn) {
            score = compare(node1->stmt.asgn.rhs, node2->stmt.asgn.rhs, score);
            score = compare(node1->stmt.asgn.lhs, node2->stmt.asgn.lhs, score);
        }
        return score;
    }
    
    // if they are both expression nodes
    if (node1->type == ast_rexpr and node2->type == ast_rexpr) {
        score = compare(node1->rexpr.lhs, node2->rexpr.lhs, score);
        score = compare(node1->rexpr.rhs, node2->rexpr.rhs, score);
        return score;
    }
    if (node1->type == ast_bexpr and node2->type == ast_bexpr) {
        score = compare(node1->bexpr.lhs, node2->bexpr.lhs, score);
        score = compare(node1->bexpr.rhs, node2->bexpr.rhs, score);
        return score;
    }
    if (node1->type == ast_uexpr and node2->type == ast_uexpr) {
        score = compare(node1->uexpr.expr, node2->uexpr.expr, score);
        return score;
    }

    printf("Error: No matching case found\n");
    return score;

}
            

// Main entry point function which we will use to compare two given bits of code as per the spec,
// starting with their root nodes leveraging semantic analysis like we did in part 1
int compareTrees(astNode *rootnode1, astNode *rootnode2) {
    int score = 100;
    return compare(rootnode1, rootnode2, score);
}