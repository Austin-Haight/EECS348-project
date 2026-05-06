#pragma once
#include <string>

using namespace std;

class Token
{
 protected:
  string value;
 public:
  const int inputs = 0;
  bool IsTerminator = false;

  virtual string to_string() = 0;
};

class IntToken : public Token
{
    public:
        IntToken(string);
        int to_int();
        string to_string();
};

class FloatToken : public Token
{
 public:
  FloatToken(string);
  float to_float();
  string to_string();
};

class OperatorToken : public Token
{
 public:
  const int inputs=2;
  OperatorToken(string);
  string to_string();
};

class NegationToken : public Token
{
 public:
  const int inputs = 1;
  string to_string();
};

class OpenParenthesesToken : public Token
{
 public:
  string to_string();
};

class CloseParenthesesToken : public Token
{
 public:
  string to_string();
};