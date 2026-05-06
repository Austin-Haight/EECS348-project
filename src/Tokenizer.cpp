#include "Tokenizer.h"
#include "Token.h"
#include "Error.h"
#include <list>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <optional>
#include <algorithm>

using namespace std;

bool DFA::step(string s, int pos)
{
 if(_map[_state].count(s[pos])>0)
 {
  _state=_map[_state].at(s[pos]);
  _count++;
  _pos++;
  return true;
 }
 return false;
}

DFA::DFA()
{
 for(int i=0;i<8;i++)
 {
  _map[i] = unordered_map<char, int>();
 }
}

DFA::DFA(unordered_map<char, int> map[8], bool exstates[8])
{
 for(int i=0;i<8;i++)
 {
  _map[i]=unordered_map(map[i]);
 }
 for(int i=0;i<8;i++)
 {
  _exit[i]=exstates[i];
 }
}

void DFA::insert(int state, pair<char, int> map)
{
 _map[state].insert(map);
}

void DFA::setState(int state, bool exstate)
{
 _exit[state] = exstate;
}

int DFA::read(const string s, int pos)
{
 _count=0;
 _state=0;
 _pos=pos;
 while(_pos<s.length() && step(s, _pos)){}
 if(_exit[_state])
  return _count;
 else
  return 0;
}

void Tokenizer::initDFA()
{
 //integer DFA
 intDFA.insert(0, pair<char, int>('-', 1));//possible negative sign
  for(int i=0;i<10;i++)//the digits
 {
  intDFA.insert(0, pair<char, int>('0'+i, 2));
  intDFA.insert(1, pair<char, int>('0'+i, 2));
  intDFA.insert(2, pair<char, int>('0'+i, 2));
 }
  intDFA.setState(0, false);//if we fail on the first step there's clearly not an int here
 intDFA.setState(1, false);//make sure there's at least one digit after the negative sign
 intDFA.setState(2, true);
 
 //float DFA
 floatDFA.insert(0, pair<char, int>('-', 1));//possible negative sign
  for(int i=0;i<10;i++)//the digits
 {
  //before the period
  floatDFA.insert(0, pair<char, int>('0'+i, 2));
  floatDFA.insert(1, pair<char, int>('0'+i, 2));
  floatDFA.insert(2, pair<char, int>('0'+i, 2));
   //after the period
  floatDFA.insert(3, pair<char, int>('0'+i, 5));
  floatDFA.insert(4, pair<char, int>('0'+i, 5));
 }
  //periods
 floatDFA.insert(0, pair<char, int>('.', 3));//if the period is the first character
 floatDFA.insert(1, pair<char, int>('.', 4));//if the period is not the first character
 floatDFA.insert(2, pair<char, int>('.', 5));//if the period is not the first character and a digit appears before it
  floatDFA.setState(0, false);
 floatDFA.setState(1, false);
 floatDFA.setState(2, false);//require at least one period to exist
 floatDFA.setState(3, false);
 floatDFA.setState(4, false);
 floatDFA.setState(5, true);
}

Tokenizer::Tokenizer()
{
}

Tokenizer::Tokenizer(list<Token*>* output)
 {
  _out = output;
  initDFA();
 }

optional<TokenizerError> Tokenizer::tokenize(string input)
{
 pos=0;
 outlen=0;
 while(!(*_out).empty())
 {
  delete ((*_out).front());
  (*_out).pop_front();
 }
 (*_out).clear();
 //strip spaces from the input
 input.erase(remove_if(input.begin(), input.end(), ::isspace), input.end());

 for(int i=0;i<input.length();)
 {
  switch(input[i])
  {
   case '-':
    if((*((*_out).back())).inputs==0)
    {
      (*_out).push_back(new OperatorToken("-"));
      i++;
      break;
    }
    if(i+1<input.length() && input[i+1]=='(')//this is the only case where a negation token should actually exist, otherwise it's just part of an int or float
    {
     (*_out).push_back(new NegationToken());
     i++;
     break;
    }
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
   case '.':
    {
     int f = floatDFA.read(input, i);
     if(f)
     {
      char s[f+1];
      input.copy(s, f, i);
      s[f]='\0';
      (*_out).push_back(new FloatToken(s));
      i+=f;
      break;
     }
     int in = intDFA.read(input, i);
     if(in)
     {
      char s[in+1];
      input.copy(s, in, i);
      s[in]='\0';
      (*_out).push_back(new IntToken(s));
      i+=in;
      break; 
     }
    }
    return TokenizerError(i);
   case '+':
    {
     (*_out).push_back(new OperatorToken("+"));
     i++;
     break;
    }
   case '*':
    if(i+1<input.length() && input[i+1]=='*')
    {
     (*_out).push_back(new OperatorToken("**"));
     i+=2;
     break;
    }
    {
     (*_out).push_back(new OperatorToken("*"));
     i++;
     break;
    }
   case '/':
    if(i+1<input.length() && input[i+1]=='/')
    {
     (*_out).push_back(new OperatorToken("//"));
     i+=2;
     break;
    }
    {
     (*_out).push_back(new OperatorToken("/"));
     i++;
     break;
    }
   case '%':
    {
     (*_out).push_back(new OperatorToken("%"));
     i++;
     break;
    }
   case '(':
    {
     (*_out).push_back(new OpenParenthesesToken());
     i++;
     break;
    }
   case ')':
    {
     (*_out).push_back(new CloseParenthesesToken());
     i++;
     break;
    }
   default:
    return TokenizerError(i);
  }
  (*(*_out).back()).IsTerminator = true;
 }
 return optional<TokenizerError>();
}