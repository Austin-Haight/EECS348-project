#include "Parser.h"
#include "Token.h"
#include "Error.h"
#include <list>
#include <vector>
#include <memory>
#include <optional>
#include <string>

using namespace std;

ParserError::ParserError(int pos, string msg): position(pos), message(msg) {}

Token* Parser::peek()
{
    if (_pos < (int)_tokens.size())
        return _tokens[_pos];
    return nullptr;
}

Token* Parser::consume()
{
    if (_pos < (int)_tokens.size())
        return _tokens[_pos++];
    return nullptr;
}

bool Parser::isOperator(Token* t, const string& val)
{
    if (!t) return false;
    OperatorToken* op = dynamic_cast<OperatorToken*>(t);
    return op && op->to_string() == val;
}

bool Parser::isAddSub(Token* t)
{
    if (!t) return false;
    OperatorToken* op = dynamic_cast<OperatorToken*>(t);
    if (!op) return false;
    string v = op->to_string();
    return v == "+" || v == "-";
}

bool Parser::isMulDivMod(Token* t)
{
    if (!t) return false;
    OperatorToken* op = dynamic_cast<OperatorToken*>(t);
    if (!op) return false;
    string v = op->to_string();
    return v == "*" || v == "/" || v == "//" || v == "%";
}

// Allocate a node. Ownership flows via unique_ptr, so the tree destructor
// handles all cleanup automatically — no manual tracking needed.
unique_ptr<TreeNode> Parser::makeNode(Token* t,
                              unique_ptr<TreeNode> left  = nullptr,
                              unique_ptr<TreeNode> right = nullptr)
{
    auto n   = make_unique<TreeNode>();
    n->token = t;
    n->left  = left.release();
    n->right = right.release();
    return n;
}

// ---- recursive-descent grammar rules ----------------------------------

// expression := term ( ('+' | '-') term )*
unique_ptr<TreeNode> Parser::parseExpression(optional<ParserError>& err)
{
    unique_ptr<TreeNode> left = parseTerm(err);
    if (err) return nullptr;

    while (isAddSub(peek()))
    {
        Token* op = consume();
        unique_ptr<TreeNode> right = parseTerm(err);
        if (err) return nullptr;

        left = makeNode(op, std::move(left), std::move(right));
    }
    return left;
}

// term := factor ( ('*' | '/' | '//' | '%') factor )*
unique_ptr<TreeNode> Parser::parseTerm(optional<ParserError>& err)
{
    unique_ptr<TreeNode> left = parseFactor(err);
    if (err) return nullptr;

    while (isMulDivMod(peek()))
    {
        Token* op = consume();
        unique_ptr<TreeNode> right = parseFactor(err);
        if (err) return nullptr;

        left = makeNode(op, std::move(left), std::move(right));
    }
    return left;
}

// factor := unary ( '**' factor )*    (right-associative exponentiation)
unique_ptr<TreeNode> Parser::parseFactor(optional<ParserError>& err)
{
    unique_ptr<TreeNode> base = parseUnary(err);
    if (err) return nullptr;

    if (isOperator(peek(), "**"))
    {
        Token* op = consume();
        unique_ptr<TreeNode> exp = parseFactor(err);   // recurse for right-assoc.
        if (err) return nullptr;

        return makeNode(op, std::move(base), std::move(exp));
    }
    return base;
}

// unary := NegationToken primary | primary
unique_ptr<TreeNode> Parser::parseUnary(optional<ParserError>& err)
{
    if (isOperator(peek(), "~"))   // unary minus
    {
        Token* op = consume();

        Token* next = peek(); // check that something valid comes next

         // invalid: nothing or ")" comes next
        if (!next || dynamic_cast<CloseParenthesesToken*>(next))
        {
            err = ParserError(_pos, "Missing operand after unary '-'");
            return nullptr;
        }

        unique_ptr<TreeNode> child = parseUnary(err);
        if (err) return nullptr;

        return makeNode(op, std::move(child), nullptr);
    }

    if (isOperator(peek(), "u+"))  // unary plus
    {
        consume(); // just discard it

        Token* next = peek(); // check that something valid comes next

         // invalid: nothing or ")" comes next
        if (!next || dynamic_cast<CloseParenthesesToken*>(next))
        {
            err = ParserError(_pos, "Missing operand after unary '+'");
            return nullptr;
        }

        return parseUnary(err); // no node created
    }

    return parsePrimary(err);
}

// primary := '(' expression ')' | IntToken | FloatToken
unique_ptr<TreeNode> Parser::parsePrimary(optional<ParserError>& err)
{
    Token* t = peek();

    // Parenthesised sub-expression
    if (dynamic_cast<OpenParenthesesToken*>(t))
    {
        consume(); // eat '('
        unique_ptr<TreeNode> inner = parseExpression(err);
        if (err) return nullptr;

        if (!dynamic_cast<CloseParenthesesToken*>(peek()))
        {
            err = ParserError(_pos, "Expected closing parenthesis ')'");
            return nullptr;
        }
        consume(); // eat ')'
        return inner;
    }

    // Integer literal
    if (dynamic_cast<IntToken*>(t))
    {
        consume();
        return makeNode(t);
    }

    // Float literal
    if (dynamic_cast<FloatToken*>(t))
    {
        consume();
        return makeNode(t);
    }

    // Nothing valid here
    err = ParserError(_pos,
        t ? "Unexpected token: " + t->to_string()
          : "Unexpected end of expression");
    return nullptr;
}

// ---- tree -> prefix list ----------------------------------------------

// Pre-order traversal: operator/unary first, then children left-to-right.
void Parser::prefixFlatten(TreeNode* node, list<Token*>& out)
{
    if (!node) return;
    out.push_back(node->token);
    prefixFlatten(node->left,  out);
    prefixFlatten(node->right, out);
}

Parser::Parser() : _pos(0) {}

// -----------------------------------------------------------------------
// parse()
//
// Takes the token list produced by Tokenizer (the caller retains
// ownership of those unique_ptrs / Token objects).
// Returns a ParseResult with either a prefix token list or an error.
// -----------------------------------------------------------------------
ParseResult Parser::parse(const list<Token*>& tokenList)
{
    ParseResult result;

    // Build a flat, borrowing vector for easy indexed access.
    _tokens.clear();
    _pos = 0;
    for (const auto& uptr : tokenList)
        _tokens.push_back(uptr);

    if (_tokens.empty())
    {
        result.error = ParserError(0, "Empty token list — nothing to parse");
        return result;
    }

    // Parse the full expression.
    // 'root' is a unique_ptr: if we return early on error, it destructs
    // automatically and recursively deletes all child nodes — no leaks,
    // no double-deletes.
    optional<ParserError> err;
    unique_ptr<TreeNode> root = parseExpression(err);

    if (err)
    {
        result.error = err;
        return result;   // root unique_ptr destructs here if non-null
    }

    // Make sure every token was consumed (no trailing garbage).
    if (_pos < (int)_tokens.size())
    {
        result.error = ParserError(_pos,
            "Unexpected token after end of expression: " +
            _tokens[_pos]->to_string());
        return result;   // root unique_ptr destructs here
    }

    // Flatten the tree into prefix order.
    prefixFlatten(root.get(), result.tokens);

    return result;
    // root unique_ptr destructs here, cleaning up the entire tree.
}
