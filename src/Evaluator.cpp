#include <cmath>
#include <string>

#include "Parser.cpp"

using namespace std;

/*
=====================
ErrorType Enum
=====================

Represents the possible error states that may occur.

- NO_ERROR: Evaluated successfully
- DIV_ZERO: Attempted division or modulo by zero
- SYNTAX_ERROR: Invalid expression tree
- UNKNOWN_TOKEN: Unrecognized token
*/
enum ErrorType {
    NO_ERROR = 0,
    DIV_ZERO = 1,
    SYNTAX_ERROR = 2,
    UNKNOWN_TOKEN = 3
};

/*
=====================
EvalResult Struct
=====================

Stores the...
- computed value of the expression only if errorType is 0.
- errorType using the following error codes:
    + 0 = no error
    + 1 = division by zero
    + 2 = syntax error
    + 3 = unknown token
*/
struct EvalResult {
    float value;
    ErrorType errorType;
};

/*
=====================================
EVALUATOR CLASS
=====================================

---------------------------------------------------------------------------
PURPOSE:
This class evaluates an expression tree (AST) and computes the 
proper result.
---------------------------------------------------------------------------

------------------------------------------------------------------------------
PRECONDITIONS:
1. The input TreeNode pointer is a valid expression tree root
2. Tokens are produced by a previous module tokenizer
3. The Tokens are order properly according to PEMDAS evaluation
4. Each node represents either a number, unary operator, or binary operator
5. Parentheses have already been handled by the parser and implemented in the AST

POSTCONDITIONS:
1. Result is returned in the EvalResult struct
2. If evaluation was successful the errorType is 0
3. If an error occurs the proper errorType is included
4. If the value is undefined, its set to 0
5. Input tree is not modified
------------------------------------------------------------------------------
*/
class Evaluator {
    public:
        EvalResult evaluate(TreeNode* root);
    
    private:
        EvalResult evalNode(TreeNode* root);
};

/*
evaluate:
    Description:
        Entry point for evaluating the expression by initializing evaluation and 
        to start the recursive process.
    
    Parameters: 
        root (TreeNode*): Pointer to the root of the tree
    
    Returns:
        EvalResult: showing the computed result or the error information
*/
EvalResult Evaluator::evaluate(TreeNode* root) {

    return evalNode(root);
}

/*
- evalNode:
    Description:
        Recursively evaluates a node using a post-order traversal.
    
    Parameters: 
        node (TreeNode*): Pointer to the current node in the tree
    
    Returns:
        EvalResult: showing the computed result or the error information
*/
EvalResult Evaluator::evalNode(TreeNode* node) {

    // If the node does not exist, return a syntax error
    if (node == nullptr) {
        return {0, SYNTAX_ERROR};
    }

    // UNARY OPERATOR
    // If the node's value represents a '-' unary, then negate its single child
    if (node->value == "neg") {

        // Confirm no error with child of '-' unary operator
        if (node->left == nullptr) {
            return {0, SYNTAX_ERROR};
        }

        EvalResult child = evalNode(node->left);

        // Confirm the child has no error before applying operator
        if (child.errorType != NO_ERROR) {
            return child;
        }

        return {-child.value, child.errorType}; // negate the value
    }

    // NUMBER
    // If the node is a leaf (no children), then the node represents a number
    if (node->left == nullptr && node->right == nullptr) {
        return {stof(node->value), NO_ERROR}; // Convert the node's value to a float
    }

    // BINARY OPERATORS
    // Evaluate the left subtree first
    EvalResult left = evalNode(node->left);

    // Confirm no error type present in the left subtree
    if (left.errorType != NO_ERROR) {
        return left;
    }

    // Evaluate the right subtree
    EvalResult right = evalNode(node->right);

    // Confirm no error type present in the right subtree
    if (right.errorType != NO_ERROR) {
        return right;
    }

    // Get the operation in string format
    const string& op = node->value;

    // Evaluate based on the operator
    if (op == "+") {
        return {left.value + right.value, NO_ERROR};
    } 
    else if (op == "-") {
        return {left.value - right.value, NO_ERROR};
    }
    else if (op == "*") {
        return {left.value * right.value, NO_ERROR};
    }
    else if (op == "/") {
        if (right.value == 0) { // check for division by zero
            return {0, DIV_ZERO};
        }
        return {left.value / right.value, NO_ERROR};
    }
    else if (op == "%") {
        if (right.value == 0) { // check for division by zero
            return {0, DIV_ZERO};
        }
        return {left.value - right.value * floor(left.value / right.value), NO_ERROR};
    }
    else if (op == "**") {
        return {pow(left.value, right.value), NO_ERROR};
    }
    else {
        return {0, UNKNOWN_TOKEN}; // unknown operator
    }
    
}
