#pragma once
#include <string>
#include <list>
#include <optional>
#include <vector>
#include <memory>
#include "Token.h"

using namespace std;

// ---------------------------------------------------------------------------
// ParserError
// ---------------------------------------------------------------------------
// Signals a syntax problem found during parsing.
// 'position' is the index into the token list where the error occurred.
class ParserError
{
    public:
    int position;
    string message;
    ParserError(int, string);
};

// ---------------------------------------------------------------------------
// ParseResult
// ---------------------------------------------------------------------------
// Returned by Parser::parse().
// On success  -> error is empty,  tokens holds the prefix-order token list.
// On failure  -> error holds a ParserError, tokens is empty.
struct ParseResult
{
    list<Token*> tokens;                  // prefix (pre-order) traversal output
    optional<ParserError> error;
};

// ---------------------------------------------------------------------------
// TreeNode  (internal expression tree)
// ---------------------------------------------------------------------------
// The parser builds a temporary binary/unary expression tree so that
// PEMDAS precedence and parentheses are handled cleanly.  The tree is then
// flattened into a prefix token list for the evaluator.
struct TreeNode
{
    Token* token = nullptr;           // the operator or operand at this node
    TreeNode* left  = nullptr;        // first (or only) child
    TreeNode* right = nullptr;        // second child (binary operators only)

    ~TreeNode()
    {
        delete left;
        delete right;
    }
};

// ---------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------
// Implements a straightforward recursive-descent parser.
//
// Grammar (from lowest to highest precedence):
//
//   expression  := term   ( ('+' | '-')  term   )*
//   term        := factor ( ('*' | '/' | '//' | '%') factor )*
//   factor      := primary ( '**' factor )*          <- right-associative
//   unary       := '-' primary | primary
//   primary     := number | '(' expression ')'
//   number      := IntToken | FloatToken
//
// The parser consumes a flat list<unique_ptr<Token>> produced by Tokenizer
// and writes a flat prefix-order list<Token*> into ParseResult.
// The Token objects are still owned by the caller's list; the parser only
// borrows raw pointers.
// ---------------------------------------------------------------------------
class Parser
{
    private:
        vector<Token*> _tokens;
        int _pos;
        Token* peek();
        Token* consume();
        bool isOperator(Token*, const string&);
        bool isAddSub(Token*);
        bool isMulDivMod(Token*);
        unique_ptr<TreeNode> makeNode(Token*, unique_ptr<TreeNode>,unique_ptr<TreeNode>);

        unique_ptr<TreeNode> parseExpression(optional<ParserError>&);
        unique_ptr<TreeNode> parseTerm(optional<ParserError>&);
        unique_ptr<TreeNode> parseFactor(optional<ParserError>&);
        unique_ptr<TreeNode> parseUnary(optional<ParserError>&);
        unique_ptr<TreeNode> parsePrimary(optional<ParserError>&);
        void prefixFlatten(TreeNode*, list<Token*>&);
    public:
        Parser();
        ParseResult parse(const list<Token*>&);

};