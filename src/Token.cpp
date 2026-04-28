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
  IntToken(string val)
  {
   value=val;
  }
  string to_string()
  {
   return value;
  }
};

class FloatToken : public Token
{
 public:
  FloatToken(string val){value=val;}
  string to_string()
  {
   return value;
  }
};

class OperatorToken : public Token
{
 public:
  const int inputs = 2;
  OperatorToken(string val)
  {
   value=val;
  }
  string to_string()
  {
   return value;
  }
};

class NegationToken : public Token
{
 public:
  const int inputs = 1;
  string to_string()
  {
   return "-";
  }
};

class OpenParenthesesToken : public Token
{
 public:
  string to_string()
  {
   return "(";
  }
};

class CloseParenthesesToken : public Token
{
 public:
  string to_string()
  {
   return ")";
  }
};