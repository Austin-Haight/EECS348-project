#include <cmath>
#include "Token.hpp"

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
- errorIndex shows the index in the token array where the error occured (-1 = no error)
*/
struct EvalResult {
    float value;
    int errorType;
    int errorIndex;
};

/*
=====================================
EVALUATOR CLASS
=====================================

---------------------------------------------------------------------------
PURPOSE:
This class evaluates a tokenized arithmetic expression and computes the 
proper result.
---------------------------------------------------------------------------

------------------------------------------------------------------------------
PRECONDITIONS:
1. Input token array is valid
2. Tokens are produced by a previous module tokenizer
3. The Tokens are order properly in PEMDAS evalution
4. size correctly reflects the number of tokens

POSTCONDITIONS:
1. Result is returned in the EvalResult struct.
2. If evalutation was successful the errorType is 0 and the errorIndex is -1
3. If an error occurs the proper errorType is included
4. If the value is undefined, its set to 0
5. Token array is not modified
------------------------------------------------------------------------------
*/
class Evaluator {
    public:
        EvalResult evaluate(Token* tokens, int size);
    
    private:
        EvalResult evalToken(Token* tokens, int& i, int size);
        EvalResult handleUnary(Token* tokens, int& i, int size);
};

/*
evaluate:
    Description:
        Entry point for evaluating the expression by initializing evaluation and 
        to start the recursive process.
    
    Parameters: 
        tokens: pointer to an array of Token objects
        size: number of tokens in the array
    
    Returns:
        EvalResult showing the computed result or the error information
*/
EvalResult Evaluator::evaluate(Token* tokens, int size) {

    int i = 0; // Start from beginning of array
    return evalToken(tokens, i, size);
}

/*
- evalToken:
    Description:
        Recursively evaluates a token or sub-expression for the proper index.
        This handles all expression types, such as parentheses and unary/binary operators.
    
    Parameters: 
        tokens: pointer to an array of Token objects
        i: reference to the current index
        size: number of tokens in the array
    
    Returns:
        EvalResult showing the computed result or the error information
*/
EvalResult Evaluator::evalToken(Token* tokens, int& i, int size) {

    // If i is outside the valid range, return a syntax error
    if (i >= size) {
        return {0, 2, i};
    }

    Token t = tokens[i]; // Initialize t to be a token

    if (t.isInteger()) {
        return {t.getInt(), 0, -1}; // Valid integer value
    }

    if (t.isParentheses()) {
        // Recursively evaluate the sub-expression inside the parentheses
        return evaluate(t.getInnerTokens(), t.innerSize());
    }

    if (t.isUnary()) {
        // Call the helper function for unary
        return handleUnary(tokens, i, size);
    }

    if (t.isOperator()) {
        char op = t.getOp();

        // Evaluate the left side of the tree data structure
        i++;
        EvalResult left = evalToken(tokens, i, size);
        // If the error is a nonzero number (there is an error), return the error
        if (left.errorType) {
            return left;
        }

        // Evaluate the right side of the tree data structure
        i++;
        EvalResult right = evalToken(tokens, i, size);
        // If the error is a nonzero number (there is an error), return the error
        if (right.errorType) {
            return right;
        }

        // Evaluate the expression based on each possible operator
        switch (op) {

            case '+':
                return {left.value + right.value, 0, -1};

            case '-':
                return {left.value - right.value, 0, -1};
            
            case '*':
                return {left.value * right.value, 0, -1};
            
            case '/':
                // Check for division by zero error
                if (right.value == 0) {
                    return {0, 1, i};
                }
                return {left.value / right.value, 0, -1};
            
            case '%':
                // Check for division by zero error
                if (right.value == 0) {
                    return {0, 1, i};
                }
                return {left.value - right.value * std::floor(left.value / right.value), 0, -1};
            
            case '**':
                return {std::pow(left.value, right.value), 0, -1};
            
            default:
                // Unknown operator
                return {0, 2, i};
        }

    }

    return {0, 2, i}; // Unknown token
    
}

/*
- handleUnary:
    Description:
        Process the unary operators (+ or -) by applying the sign to the expression
        it relates to.
    
    Parameters: 
        tokens: pointer to an array of Token objects
        i: reference to the current index
        size: number of tokens in the array
    
    Returns:
        EvalResult showing the computed result or the error information
*/
EvalResult Evaluator::handleUnary(Token* tokens, int& i, int size) {
    
    Token t = tokens[i];

    // If the Operation is '-', the sign is -1, otherwise it is just 1
    int sign = (t.getOp() == '-') ? -1 : 1;

    // Move past the unary operator
    i++;

    // Evaluate the next full expression
    EvalResult result = evalToken(tokens, i, size);

    // If there is an error (nonzero for errorType), immediately return
    if (result.errorType != 0) {
        return result;
    }

    // Return the value of the number with its unary operator included
    return {sign * result.value, 0, -1};
}
