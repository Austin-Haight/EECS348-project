#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <memory>
#include <string>

using namespace std;

// Temporary token interface assumption:
// Each token can return its text using to_string()
class Token {
public:
  virtual string to_string() = 0;
};

// Simple expression tree node
class TreeNode {
public:
  string value;
  TreeNode *left;
  TreeNode *right;

  TreeNode(string val);
  TreeNode(string val, TreeNode *l, TreeNode *r);
};

// Parser result: stores success or error message
class ParserResult {
public:
  bool success;
  string errorMessage;

  ParserResult();
  ParserResult(string message);
};

class Parser {
private:
  list<unique_ptr<Token> > *tokens;
  list<unique_ptr<Token> >::iterator current;
  TreeNode **output;

  bool atEnd();
  string currentValue();
  void moveNext();

  bool isNumber(string value);
  bool isAddOp(string value);
  bool isMultOp(string value);
  bool isPowerOp(string value);

  TreeNode *parseExpression(ParserResult &result);
  TreeNode *parseTerm(ParserResult &result);
  TreeNode *parsePower(ParserResult &result);
  TreeNode *parseFactor(ParserResult &result);

public:
  Parser(TreeNode *&out);

  ParserResult parse(list<unique_ptr<Token> > &input);
};

#endif