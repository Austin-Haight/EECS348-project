#include "Token.cpp"
#include "Error.cpp"
#include <list>
#include <vector>
#include <memory>
#include <optional>
#include <string>

using namespace std;

// ---------------------------------------------------------------------------
// ParserError
// ---------------------------------------------------------------------------
// Signals a syntax problem found during parsing.
// 'position' is the index into the token list where the error occurred.
class ParserError : public Error
{
public:
    int position;
    string message;

    ParserError(int pos, string msg)
        : position(pos), message(msg) {}
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
    // ---- state ------------------------------------------------------------
    vector<Token*> _tokens;   // borrowed, flat view of the tokenizer output
    int            _pos;      // current read position

    // ---- helpers ----------------------------------------------------------

    Token* peek()
    {
        if (_pos < (int)_tokens.size())
            return _tokens[_pos];
        return nullptr;
    }

    Token* consume()
    {
        if (_pos < (int)_tokens.size())
            return _tokens[_pos++];
        return nullptr;
    }

    bool isOperator(Token* t, const string& val)
    {
        if (!t) return false;
        OperatorToken* op = dynamic_cast<OperatorToken*>(t);
        return op && op->to_string() == val;
    }

    bool isAddSub(Token* t)
    {
        if (!t) return false;
        OperatorToken* op = dynamic_cast<OperatorToken*>(t);
        if (!op) return false;
        string v = op->to_string();
        return v == "+" || v == "-";
    }

    bool isMulDivMod(Token* t)
    {
        if (!t) return false;
        OperatorToken* op = dynamic_cast<OperatorToken*>(t);
        if (!op) return false;
        string v = op->to_string();
        return v == "*" || v == "/" || v == "//" || v == "%";
    }

    // Allocate a node. Ownership flows via unique_ptr, so the tree destructor
    // handles all cleanup automatically — no manual tracking needed.
    unique_ptr<TreeNode> makeNode(Token* t,
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
    unique_ptr<TreeNode> parseExpression(optional<ParserError>& err)
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
    unique_ptr<TreeNode> parseTerm(optional<ParserError>& err)
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
    unique_ptr<TreeNode> parseFactor(optional<ParserError>& err)
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
    unique_ptr<TreeNode> parseUnary(optional<ParserError>& err)
    {
        if (dynamic_cast<NegationToken*>(peek()))
        {
            Token* neg = consume();
            unique_ptr<TreeNode> child = parsePrimary(err);
            if (err) return nullptr;

            return makeNode(neg, std::move(child));
        }
        return parsePrimary(err);
    }

    // primary := '(' expression ')' | IntToken | FloatToken
    unique_ptr<TreeNode> parsePrimary(optional<ParserError>& err)
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
    void prefixFlatten(TreeNode* node, list<Token*>& out)
    {
        if (!node) return;
        out.push_back(node->token);
        prefixFlatten(node->left,  out);
        prefixFlatten(node->right, out);
    }

public:
    Parser() : _pos(0) {}

    // -----------------------------------------------------------------------
    // parse()
    //
    // Takes the token list produced by Tokenizer (the caller retains
    // ownership of those unique_ptrs / Token objects).
    // Returns a ParseResult with either a prefix token list or an error.
    // -----------------------------------------------------------------------
    ParseResult parse(const list<unique_ptr<Token>>& tokenList)
    {
        ParseResult result;

        // Build a flat, borrowing vector for easy indexed access.
        _tokens.clear();
        _pos = 0;
        for (const auto& uptr : tokenList)
            _tokens.push_back(uptr.get());

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
};