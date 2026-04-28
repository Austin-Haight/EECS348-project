#include "parser.hpp"

#include <cctype>

TreeNode::TreeNode(string val) {
  value = val;
  left = nullptr;
  right = nullptr;
}

TreeNode::TreeNode(string val, TreeNode *l, TreeNode *r) {
  value = val;
  left = l;
  right = r;
}

ParserResult::ParserResult() {
  success = true;
  errorMessage = "";
}

ParserResult::ParserResult(string message) {
  success = false;
  errorMessage = message;
}

Parser::Parser(TreeNode *&out) {
  output = &out;
  tokens = nullptr;
}

bool Parser::atEnd() { return current == tokens->end(); }

string Parser::currentValue() {
  if (atEnd()) {
    return "";
  }

  return (*current)->to_string();
}

void Parser::moveNext() {
  if (!atEnd()) {
    ++current;
  }
}

bool Parser::isNumber(string value) {
  if (value.length() == 0) {
    return false;
  }

  int start = 0;
  bool hasDigit = false;
  bool hasDecimal = false;

  if (value[0] == '-') {
    if (value.length() == 1) {
      return false;
    }

    start = 1;
  }

  for (int i = start; i < value.length(); i++) {
    if (isdigit(value[i])) {
      hasDigit = true;
    } else if (value[i] == '.') {
      if (hasDecimal) {
        return false;
      }

      hasDecimal = true;
    } else {
      return false;
    }
  }

  return hasDigit;
}

bool Parser::isAddOp(string value) { return value == "+" || value == "-"; }

bool Parser::isMultOp(string value) {
  return value == "*" || value == "/" || value == "%";
}

bool Parser::isPowerOp(string value) { return value == "**"; }

ParserResult Parser::parse(list<unique_ptr<Token>> &input) {
  tokens = &input;
  current = tokens->begin();

  ParserResult result;

  TreeNode *root = parseExpression(result);

  if (!result.success) {
    return result;
  }

  if (!atEnd()) {
    return ParserResult("Extra token found after expression.");
  }

  *output = root;

  return result;
}

// Handles + and -
TreeNode *Parser::parseExpression(ParserResult &result) {
  TreeNode *left = parseTerm(result);

  if (!result.success) {
    return nullptr;
  }

  while (!atEnd() && isAddOp(currentValue())) {
    string op = currentValue();
    moveNext();

    TreeNode *right = parseTerm(result);

    if (!result.success) {
      return nullptr;
    }

    left = new TreeNode(op, left, right);
  }

  return left;
}

// Handles *, /, %
TreeNode *Parser::parseTerm(ParserResult &result) {
  TreeNode *left = parsePower(result);

  if (!result.success) {
    return nullptr;
  }

  while (!atEnd() && isMultOp(currentValue())) {
    string op = currentValue();
    moveNext();

    TreeNode *right = parsePower(result);

    if (!result.success) {
      return nullptr;
    }

    left = new TreeNode(op, left, right);
  }

  return left;
}

// Handles **
// Recursive so 2 ** 3 ** 2 becomes 2 ** (3 ** 2)
TreeNode *Parser::parsePower(ParserResult &result) {
  TreeNode *left = parseFactor(result);

  if (!result.success) {
    return nullptr;
  }

  if (!atEnd() && isPowerOp(currentValue())) {
    string op = currentValue();
    moveNext();

    TreeNode *right = parsePower(result);

    if (!result.success) {
      return nullptr;
    }

    left = new TreeNode(op, left, right);
  }

  return left;
}

// Handles numbers, parentheses, unary +, and unary -
TreeNode *Parser::parseFactor(ParserResult &result) {
  if (atEnd()) {
    result = ParserResult("Expected a number or parenthesis.");
    return nullptr;
  }

  string value = currentValue();

  // Unary plus
  if (value == "+") {
    moveNext();
    return parseFactor(result);
  }

  // Unary minus
  if (value == "-") {
    moveNext();

    TreeNode *factor = parseFactor(result);

    if (!result.success) {
      return nullptr;
    }

    return new TreeNode("neg", factor, nullptr);
  }

  // Parenthesized expression
  if (value == "(") {
    moveNext();

    TreeNode *inside = parseExpression(result);

    if (!result.success) {
      return nullptr;
    }

    if (atEnd() || currentValue() != ")") {
      result = ParserResult("Missing closing parenthesis.");
      return nullptr;
    }

    moveNext();

    return inside;
  }

  // Number
  if (isNumber(value)) {
    moveNext();
    return new TreeNode(value);
  }

  if (value == ")") {
    result = ParserResult("Unexpected closing parenthesis.");
    return nullptr;
  }

  result = ParserResult("Invalid expression.");
  return nullptr;
}