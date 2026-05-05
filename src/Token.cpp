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
  int to_int()
  {
    int out=0;
    bool negative=false;
    for(int i=0; i<value.length();i++)
    {
     if(value[i]=='-')
     {
      negative=true;
      continue;
     }
     out*=10;
     out+=value[i];
    }
    return (-1*negative)*out;
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
  float to_float()
  {
    float out=0.;
    int dotpos=0;
    bool negative=false;
    for(int i=0;i<value.length();i++)
    {
     if(value[i]=='-')
     {
      negative=true;
      continue;
     }
     if(value[i]=='.')
     {
      dotpos=i;
     }
     else
     {
      out*=10;
      out+=value[i];
     }
    }
    return (-1*negative)*out/(value.length()-dotpos);
  }
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