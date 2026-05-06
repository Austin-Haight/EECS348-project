#include "Token.h"
#include <string>

using namespace std;

IntToken::IntToken(string val)
{
 value=val;
}

int IntToken::to_int()
{
  int out=0;
  bool negative=false;
  for(int i=0; i<value.length();i++)
  {
   if(value[i]=='\0')
   {
    break;
   }
   if(value[i]=='-')
   {
    negative=true;
    continue;
   }
   out*=10;
   out+=((int) value[i])-'0';
  }
  if(negative)
  {
    return -1*out;
  }
  return out;
}

string IntToken::to_string()
{
 return value;
}

FloatToken::FloatToken(string val){value=val;}
float FloatToken::to_float()
{
  float out=0.;
  int dotpos=0;
  bool negative=false;
  for(int i=0;i<value.length();i++)
  {
   if(value[i]=='\0')
   {
    break;
   }
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
    out+=((int) value[i])-'0';
   }
  }
  return (-1*negative)*out/(value.length()-dotpos);
}

string FloatToken::to_string()
{
 return value;
}

OperatorToken::OperatorToken(string val)
{
 value=val;
}

string OperatorToken::to_string()
{
 return value;
}

string NegationToken::to_string()
{
 return "-";
}

string OpenParenthesesToken::to_string()
{
 return "(";
}

string CloseParenthesesToken::to_string()
{
 return ")";
}
