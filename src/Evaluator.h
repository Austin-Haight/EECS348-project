#pragma once

#include "Token.h"

#include <list>

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
        EvalResult evaluate(list<Token*>& tokens);
    
    private:
        EvalResult evalPrefix(list<Token*>& tokens, list<Token*>::iterator& it);
};