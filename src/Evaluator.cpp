#include "Evaluator.h"

#include <cmath>
#include <string>
#include <list>

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

    // -------------------------
    // SAFETY CHECK
    // -------------------------
    if (it == tokens.end()) {
        return {0, SYNTAX_ERROR};
    }

    Token* t = *it;
    ++it;

    // -------------------------
    // NUMBER
    // -------------------------
    if (IntToken* i = dynamic_cast<IntToken*>(t)) {
        return {i->to_int(), NO_ERROR};
    }

    if (FloatToken* f = dynamic_cast<FloatToken*>(t)) {
        return {f->to_float(), NO_ERROR};
    }

    // -------------------------
    // OPERATOR
    // -------------------------
    OperatorToken* oper = dynamic_cast<OperatorToken*>(t);

    if (!oper) {
        return {0, UNKNOWN_TOKEN};
    }

    string op = oper->to_string();

    // -------------------------
    // UNARY OPERATORS
    // -------------------------

    // unary minus
    if (op == "~") {
        EvalResult sub = evalPrefix(tokens, it);
        if (sub.errorType != NO_ERROR) return sub;

        return {-sub.value, NO_ERROR};
    }

    // unary plus (no-op)
    if (op == "u+") {
        EvalResult sub = evalPrefix(tokens, it);
        if (sub.errorType != NO_ERROR) return sub;

        return {sub.value, NO_ERROR};
    }

    // -------------------------
    // BINARY OPERATORS
    // -------------------------

    EvalResult left = evalPrefix(tokens, it);
    if (left.errorType != NO_ERROR) {
        return left;
    }

    if (it == tokens.end()) {
        return {0, SYNTAX_ERROR};
    }

    EvalResult right = evalPrefix(tokens, it);
    if (right.errorType != NO_ERROR) {
        return right;
    }

    // -------------------------
    // APPLY OPERATORS
    // -------------------------

    if (op == "+") {
        return {left.value + right.value, NO_ERROR};
    }

    if (op == "-") {
        return {left.value - right.value, NO_ERROR};
    }

    if (op == "*") {
        return {left.value * right.value, NO_ERROR};
    }

    if (op == "/") {
        if (right.value == 0) return {0, DIV_ZERO};
        return {left.value / right.value, NO_ERROR};
    }

    if (op == "%") {
        if (right.value == 0) return {0, DIV_ZERO};
        return {left.value - right.value * floor(left.value / right.value), NO_ERROR};
    }

    if (op == "**") {
        return {pow(left.value, right.value), NO_ERROR};
    }

    return {0, UNKNOWN_TOKEN};
}