#include "Evaluator.h"

#include <cmath>
#include <string>
#include <list>

#include "Parser.h"

using namespace std;

/*
=======================
EVALUATE METHOD
=======================
Description:
    Entry point for evaluating the expression by initializing evaluation and 
    to start the recursive process.
    
Parameters: 
    tokens (list<unique_ptr<Token>>): list of all tokens from expression in prefix order.
    
Returns:
    EvalResult: showing the computed result or the error information
*/
EvalResult Evaluator::evaluate(list<Token*>& tokens) {
    list<Token*>::iterator it = tokens.begin();
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
    tokens (list<unique_ptr<Token>>): list of all tokens from expression in prefix order.
    it (list<unique_ptr<Token>>::iterator&): reference to the current position in the token list.
    
Returns:
    EvalResult: showing the computed result or the error information
*/
EvalResult Evaluator::evalPrefix(list<Token*>& tokens, list<Token*>::iterator& it) {

    // If no tokens are left, return a syntax error
    if (it == tokens.end()) {
        return {0, SYNTAX_ERROR};
    }

    Token* t = *it; // the token 'it' is pointing to

    ++it; // point to the next token

    // ----------------------
    // UNARY MINUS
    //-----------------------

    if (dynamic_cast<NegationToken*>(t)) {

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

    if (IntToken* i = dynamic_cast<IntToken*>(t)) {
        return {i->to_int(), NO_ERROR}; // convert token string into a float
    }

    // Check if this token is a float number and convert to a float
    if (FloatToken* f = dynamic_cast<FloatToken*>(t)) {
        return {f->to_float(), NO_ERROR}; 
    }

    // ---------------------------
    // BINARY OPERATORS
    // ---------------------------

    OperatorToken* oper = dynamic_cast<OperatorToken*>(t);

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
