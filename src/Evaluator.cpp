#include <cmath>
#include <string>
#include <list>

#include "Parser.cpp"

using namespace std;

/*
=====================
ErrorType Enum
=====================

Represents the possible error states that may occur.

- NO_ERROR: Evaluated successfully
- DIV_ZERO: Attempted division or modulo by zero
- SYNTAX_ERROR: Invalid prefix expression
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

PURPOSE:
This class evaluates a prefix-ordered token stream and computes
the proper result.

PRECONDITIONS:
1. The input prefix-ordered token list is valid
2. Tokens are produced by a previous module tokenizer
4. Each token represents either a number, unary operator, or binary operator
5. Parentheses have already been handled by the parser and implemented in the AST

POSTCONDITIONS:
1. Result is returned in the EvalResult struct
2. If evaluation was successful the errorType is 0
3. If an error occurs the proper errorType is included
4. If the value is undefined, its set to 0
5. Input token list is not modified
*/
class Evaluator {
    public:
        EvalResult evaluate(list<unique_ptr<Token>>& tokens);
    
    private:
        EvalResult evalPrefix(list<unique_ptr<Token>>& tokens, list<unique_ptr<Token>>::iterator& it);
};

/*
=======================
EVALUATE METHOD
=======================
Description:
    Entry point for evaluating the expression by initializing evaluation and 
    to start the recursive process.
    
Parameters: 
    tokens (list<Token*>): list of all tokens from expression in prefix order.
    
Returns:
    EvalResult: showing the computed result or the error information
*/
EvalResult Evaluator::evaluate(list<unique_ptr<Token>>& tokens) {
    list<unique_ptr<Token>>::iterator it = tokens.begin();
    return evalPrefix(tokens, it);
}

/*
=======================
EVALPREFIX METHOD
=======================
Description:
    Recursively evaluates a prefix-ordered token stream by consuming tokens in order, 
    where each operator triggers recursive evaluation of its operands before computing
    the result.
    
Parameters: 
    tokens (list<Token*>): list of all tokens from expression in prefix order.
    it (list<Token*>::iterator&): reference to the current position in the token list.
    
Returns:
    EvalResult: showing the computed result or the error information
*/
EvalResult Evaluator::evalPrefix(list<unique_ptr<Token>>& tokens, list<unique_ptr<Token>>::iterator& it) {

    // If no tokens are left, return a syntax error
    if (it == tokens.end()) {
        return {0, SYNTAX_ERROR};
    }

    unique_ptr<Token>& t = *it; // the token 'it' is pointing to

    ++it; // point to the next token

    // ----------------------
    // UNARY MINUS
    //-----------------------

    if (dynamic_cast<NegationToken*>(t.get())) {

        EvalResult subExpr = evalPrefix(tokens, it);

        // Confirm the subexpression has no error before applying operator
        if (subExpr.errorType != NO_ERROR) {
            return subExpr;
        }

        return {-subExpr.value, NO_ERROR}; // negate the value
    }

    // -------------------
    // NUMBER
    // -------------------

    if (IntToken* i = dynamic_cast<IntToken*>(t.get())) {
        return {stof(i->to_string()), NO_ERROR}; // convert token string into a float
    }

    // Check if this token is a float number and convert to a float
    if (FloatToken* f = dynamic_cast<FloatToken*>(t.get())) {
        return {stof(f->to_string()), NO_ERROR}; 
    }

    // ---------------------------
    // BINARY OPERATORS
    // ---------------------------

    OperatorToken* oper = dynamic_cast<OperatorToken*>(t.get());

    // Confirm the operator is valid
    if (!oper) {
        return {0, UNKNOWN_TOKEN};
    }

    string op = oper->to_string(); // convert the operator to a string

    // Evaluate the left operand
    EvalResult left = evalPrefix(tokens, it);

    // Confirm left operand evaluated successfully
    if (left.errorType != NO_ERROR) {
        return left;
    }

    // Evaluate the right operand
    EvalResult right = evalPrefix(tokens, it);

    // Confirm right operand evaluated successfully
    if (right.errorType != NO_ERROR) {
        return right;
    }

    // -------------------------
    // APPLY OPERATORS
    // -------------------------

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
