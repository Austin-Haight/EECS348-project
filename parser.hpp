// parser.h
// Recursive descent parser for arithmetic expressions
//
// This parser takes tokens from the tokenizer and builds an AST (tree)
// that represents the structure of the expression.
//
// Pipeline:
// Tokenizer -> Parser -> Evaluator
//
// The parser handles:
// - operator precedence
// - associativity
// - parentheses
// - unary + and -
// - syntax errors

#pragma once

#include <cstddef>
#include <exception>
#include <string>
#include <vector>

// All token types the parser understands
enum TokenType {
  NUMBER,
  PLUS,
  MINUS,
  MULTIPLY,
  DIVIDE,
  MODULO,
  POWER,
  LPAREN,
  RPAREN,
  END_OF_FILE
};

// Converts token type to readable text (used for errors)
std::string tokenTypeName(TokenType type);

// Represents a token from the tokenizer
struct Token {
  TokenType type;
  double value;

  Token(TokenType t, double v = 0.0);
};

// Types of AST nodes
enum NodeType { NUM_NODE, UNARY_NODE, BINARY_NODE };

// Node in the expression tree
struct ASTNode {
  NodeType nodeType;
  std::string op;
  double value;

  ASTNode *left;
  ASTNode *right;

  static ASTNode *makeNumber(double val);

  static ASTNode *makeUnary(const std::string &op, ASTNode *operand);

  static ASTNode *makeBinary(const std::string &op, ASTNode *left,
                             ASTNode *right);
};

// Represents a parsing error
class ParseError : public std::exception {
public:
  ParseError(std::string message);
  virtual ~ParseError() throw();
  virtual const char *what() const throw();

private:
  std::string m_message;
};

// Holds either a valid AST or an error message
class ParseResult {
public:
  ParseResult(ASTNode *node);
  ParseResult(std::string errorMsg);

  bool hasError() const;
  const std::string &error() const;
  const ASTNode *node() const;
  ASTNode *takeNode();

private:
  ASTNode *m_node;
  std::string m_error;
  bool m_hasError;
};

// Main parser class
class Parser {
public:
  // Entry point: converts tokens into an AST
  ParseResult parse(const std::vector<Token> &tokens);

private:
  const std::vector<Token> *m_tokens;
  size_t m_pos;

  const Token &current() const;
  const Token &advance();
  const Token &expect(TokenType expected);
  bool check(TokenType type) const;

  // Each function represents a level of precedence

  // Lowest precedence: + and -
  ASTNode *parseExpression();

  // Medium: *, /, %
  ASTNode *parseTerm();

  // High: ** (right associative)
  ASTNode *parsePower();

  // Unary + and -
  ASTNode *parseUnary();

  // Numbers and parentheses
  ASTNode *parsePrimary();
};