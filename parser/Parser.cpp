#include "parser.hpp"
#include <cctype>

// Constructor: stores where we should put the final tree
Parser::Parser(TreeNode *&out) {
  output = &out;
  tokens = nullptr;
}

// Check if we reached the end of the token list
bool Parser::atEnd() { return current == tokens->end(); }

// Get the current token as a string
// (we rely on tokenizer's to_string())
string Parser::currentValue() {
  if (atEnd()) {
    return "";
  }

  return (*current)->to_string();
}

// Move to the next token
void Parser::moveNext() {
  if (!atEnd()) {
    ++current;
  }
}

// Check if a string is a number (int or decimal)
bool Parser::isNumber(string value) {
  if (value.length() == 0) {
    return false;
  }

  int start = 0;
  bool hasDigit = false;
  bool hasDecimal = false;

  // Handle negative numbers like "-5"
  if (value[0] == '-') {
    if (value.length() == 1) {
      return false;
    }

    start = 1;
  }

  // Go through each character and check if valid
  for (int i = start; i < value.length(); i++) {
    if (isdigit(value[i])) {
      hasDigit = true;
    } else if (value[i] == '.') {
      // Only allow one decimal point
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

// Check if token is + or -
bool Parser::isAddOp(string value) { return value == "+" || value == "-"; }

// Check if token is *, /, or %
bool Parser::isMultOp(string value) {
  return value == "*" || value == "/" || value == "%";
}

// Check if token is **
bool Parser::isPowerOp(string value) { return value == "**"; }

// Main function that starts parsing
ParserResult Parser::parse(list<unique_ptr<Token>> &input) {
  tokens = &input;
  current = tokens->begin();

  ParserResult result;

  // Start parsing from the highest level (expression)
  TreeNode *root = parseExpression(result);

  // If an error happened, return it
  if (!result.success) {
    return result;
  }

  // If we didn't reach the end, something extra is in input
  if (!atEnd()) {
    return ParserResult("Extra token found after expression.");
  }

  // Save the final tree
  *output = root;

  return result;
}

// Handles + and - (lowest precedence)
// Example: 3 + 4 - 2
TreeNode *Parser::parseExpression(ParserResult &result) {
  TreeNode *left = parseTerm(result);

  if (!result.success) {
    return nullptr;
  }

  // Keep combining while we see + or -
  while (!atEnd() && isAddOp(currentValue())) {
    string op = currentValue();
    moveNext();

    TreeNode *right = parseTerm(result);

    if (!result.success) {
      return nullptr;
    }

    // Build tree node
    left = new TreeNode(op, left, right);
  }

  return left;
}

// Handles *, /, % (middle precedence)
// Example: 4 * 2 / 8
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

// Handles exponentiation (**)
// Right-associative: 2 ** 3 ** 2 = 2 ** (3 ** 2)
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

// Handles:
// - numbers
// - parentheses
// - unary + and -
TreeNode *Parser::parseFactor(ParserResult &result) {
  if (atEnd()) {
    result = ParserResult("Expected a number or parenthesis.");
    return nullptr;
  }

  string value = currentValue();

  // Unary plus: just skip it
  if (value == "+") {
    moveNext();
    return parseFactor(result);
  }

  // Unary minus: create a "neg" node
  if (value == "-") {
    moveNext();

    TreeNode *factor = parseFactor(result);

    if (!result.success) {
      return nullptr;
    }

    return new TreeNode("neg", factor, nullptr);
  }

  // Handle parentheses
  if (value == "(") {
    moveNext();

    TreeNode *inside = parseExpression(result);

    if (!result.success) {
      return nullptr;
    }

    // Must have closing parenthesis
    if (atEnd() || currentValue() != ")") {
      result = ParserResult("Missing closing parenthesis.");
      return nullptr;
    }

    moveNext();

    return inside;
  }

  // If it's a number, return it
  if (isNumber(value)) {
    moveNext();
    return new TreeNode(value);
  }

  // Error: unexpected closing parenthesis
  if (value == ")") {
    result = ParserResult("Unexpected closing parenthesis.");
    return nullptr;
  }

  // Any other case = invalid
  result = ParserResult("Invalid expression.");
  return nullptr;
}