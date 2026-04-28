// parser.cpp
// Implements the parser logic

#include "parser.hpp"
#include <cassert>
#include <sstream>

// Constructor for Token
Token::Token(TokenType t, double v) {
  type = t;
  value = v;
}

// Converts token types to readable names (for error messages)
std::string tokenTypeName(TokenType type) {
  switch (type) {
  case NUMBER:
    return "number";
  case PLUS:
    return "+";
  case MINUS:
    return "-";
  case MULTIPLY:
    return "*";
  case DIVIDE:
    return "/";
  case MODULO:
    return "%";
  case POWER:
    return "**";
  case LPAREN:
    return "(";
  case RPAREN:
    return ")";
  case END_OF_FILE:
    return "end of input";
  default:
    return "unknown";
  }
}

// Create number node
ASTNode *ASTNode::makeNumber(double val) {
  ASTNode *node = new ASTNode();
  node->nodeType = NUM_NODE;
  node->value = val;
  node->left = nullptr;
  node->right = nullptr;
  return node;
}

// Create unary operator node (only one child)
ASTNode *ASTNode::makeUnary(const std::string &op, ASTNode *operand) {
  ASTNode *node = new ASTNode();
  node->nodeType = UNARY_NODE;
  node->op = op;
  node->left = operand;
  node->right = nullptr;
  return node;
}

// Create binary operator node (two children)
ASTNode *ASTNode::makeBinary(const std::string &op, ASTNode *left,
                             ASTNode *right) {
  ASTNode *node = new ASTNode();
  node->nodeType = BINARY_NODE;
  node->op = op;
  node->left = left;
  node->right = right;
  return node;
}

// Error constructor
ParseError::ParseError(std::string message) { m_message = message; }

ParseError::~ParseError() throw() {}

const char *ParseError::what() const throw() { return m_message.c_str(); }

// Success result
ParseResult::ParseResult(ASTNode *node) {
  m_node = node;
  m_hasError = false;
}

// Error result
ParseResult::ParseResult(std::string errorMsg) {
  m_node = nullptr;
  m_error = errorMsg;
  m_hasError = true;
}

bool ParseResult::hasError() const { return m_hasError; }

const std::string &ParseResult::error() const {
  assert(m_hasError);
  return m_error;
}

const ASTNode *ParseResult::node() const {
  assert(!m_hasError);
  return m_node;
}

ASTNode *ParseResult::takeNode() {
  ASTNode *n = m_node;
  m_node = nullptr;
  return n;
}

// Main parse function
ParseResult Parser::parse(const std::vector<Token> &tokens) {
  m_tokens = &tokens;
  m_pos = 0;

  try {
    ASTNode *root = parseExpression();

    // Make sure nothing is left unparsed
    if (!check(END_OF_FILE)) {
      throw ParseError("Unexpected token after expression");
    }

    return ParseResult(root);
  } catch (const ParseError &err) {
    return ParseResult(err.what());
  }
}

// Current token
const Token &Parser::current() const { return (*m_tokens)[m_pos]; }

// Move to next token
const Token &Parser::advance() {
  if (current().type != END_OF_FILE) {
    m_pos++;
  }
  return (*m_tokens)[m_pos - 1];
}

// Check expected token
const Token &Parser::expect(TokenType expected) {
  if (check(expected)) {
    return advance();
  }

  std::ostringstream oss;
  oss << "Expected " << tokenTypeName(expected) << " but found "
      << tokenTypeName(current().type);

  throw ParseError(oss.str());
}

// Check token type
bool Parser::check(TokenType type) const { return current().type == type; }

// Handles + and - (lowest precedence)
// Uses a loop → left associative
ASTNode *Parser::parseExpression() {
  ASTNode *left = parseTerm();

  while (check(PLUS) || check(MINUS)) {
    std::string op = check(PLUS) ? "+" : "-";
    advance();

    ASTNode *right = parseTerm();

    left = ASTNode::makeBinary(op, left, right);
  }

  return left;
}

// Handles *, /, %
// Also left associative
ASTNode *Parser::parseTerm() {
  ASTNode *left = parsePower();

  while (check(MULTIPLY) || check(DIVIDE) || check(MODULO)) {
    std::string op;

    if (check(MULTIPLY))
      op = "*";
    else if (check(DIVIDE))
      op = "/";
    else
      op = "%";

    advance();

    ASTNode *right = parsePower();

    left = ASTNode::makeBinary(op, left, right);
  }

  return left;
}

// Handles exponentiation (**)
// Uses recursion → right associative
ASTNode *Parser::parsePower() {
  ASTNode *base = parseUnary();

  if (check(POWER)) {
    advance();

    // recursive call → makes it right associative
    ASTNode *exponent = parsePower();

    return ASTNode::makeBinary("**", base, exponent);
  }

  return base;
}

// Handles unary + and -
// Allows things like --3 or -(4+2)
ASTNode *Parser::parseUnary() {
  if (check(PLUS) || check(MINUS)) {
    std::string op = check(PLUS) ? "+" : "-";
    advance();

    ASTNode *operand = parseUnary();

    return ASTNode::makeUnary(op, operand);
  }

  return parsePrimary();
}

// Handles numbers and parentheses
ASTNode *Parser::parsePrimary() {
  if (check(NUMBER)) {
    double val = current().value;
    advance();
    return ASTNode::makeNumber(val);
  }

  if (check(LPAREN)) {
    advance();

    ASTNode *expr = parseExpression();

    expect(RPAREN);

    return expr;
  }

  throw ParseError("Expected number or '('");
}