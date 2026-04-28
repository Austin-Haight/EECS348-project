#include "Token.cpp"
#include "Error.cpp"
#include <list>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <memory>

using namespace std;

class DFA
{
 private:
  unordered_map<char, int> _map[8];
  bool _exit[8];
  int _state;
  int _count;
  int _pos;

  bool step(string s, int pos)
  {
   if(_map[_state].count(s[pos])>0)
   {
    _state=_map[_state].at(s[pos]);
    _count++;
    _pos++;
   }
   else
   {
    return false;
   }
  }

 public:
  DFA()
  {
   for(int i=0;i<8;i++)
   {
    _map[i] = unordered_map<char, int>();
   }
  }

  DFA(unordered_map<char, int> map[8], bool exstates[8])
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

  void insert(int state, pair<char, int> map)
  {
   _map[state].insert(map);
  }

  void setState(int state, bool exstate)
  {
   _exit[state] = exstate;
  }

  int read(const string s, int pos)
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
};

class Tokenizer
{
 private:
  list<unique_ptr<Token>>* _out;
  DFA intDFA;
  DFA floatDFA;
  int pos;
  int outlen;

  void initDFA()
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
 public:
  Tokenizer(list<unique_ptr<Token>> &output)
  {
   _out = &output;
   *_out = list<unique_ptr<Token>>();
  }

  TokenizerError tokenize(string input)
  {
   pos=0;
   outlen=0;
   (*_out).clear();

   //strip spaces from the input
   string stripped;
   for(int i=0;i<input.length();i++)
   {
    if(input[i]==' '){continue;}
    stripped.append(to_string(input[i]));
   }

   for(int i=0;i<stripped.length();)
   {
    switch(stripped[i])
    {
     case '-':
      if(typeid((*_out).back())==typeid(OperatorToken))
      {
       unique_ptr<Token> t(new NegationToken());
       (*_out).push_back(t);
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
      int f = floatDFA.read(stripped, i);
      if(f)
      {
       char* s;
       stripped.copy(s, f, i);
       unique_ptr<Token> t(new FloatToken(s));
       (*_out).push_back(t);
       i+=f;
       break;
      }
      int in = intDFA.read(stripped, i);
      if(in)
      {
       char* s;
       stripped.copy(s, in, i);
       unique_ptr<Token> t(new IntToken(s));
       (*_out).push_back(t);
       i+=in;
       break;
      }
      return TokenizerError(i);
     case '+':
      {
       unique_ptr<Token> t(new OperatorToken("+"));
       (*_out).push_back(t);
       i++;
       break;
      }
     case '*':
      if(i+1<stripped.length() && stripped[i+1]=='*')
      {
       unique_ptr<Token> t(new OperatorToken("**"));
       (*_out).push_back(t);
       i+=2;
       break;
      }
      {
       unique_ptr<Token> t(new OperatorToken("*"));
       (*_out).push_back(t);
       i++;
       break;
      }
     case '/':
      if(i+1<stripped.length() && stripped[i+1]=='/')
      {
       unique_ptr<Token> t(new OperatorToken("//"));
       (*_out).push_back(t);
       i+=2;
       break;
      }
      {
       unique_ptr<Token> t(new OperatorToken("/"));
       (*_out).push_back(t);
       i++;
       break;
      }
     case '%':
      {
       unique_ptr<Token> t(new OperatorToken("%"));
       (*_out).push_back(t);
       i++;
       break;
      }
     case '(':
      {
       unique_ptr<Token> t(new OpenParenthesesToken());
       (*_out).push_back(t);
       i++;
       break;
      }
     case ')':
      {
       unique_ptr<Token> t(new CloseParenthesesToken());
       (*_out).push_back(t);
       i++;
       break;
      }
     default:
      return TokenizerError(i);
    }
   }
  }
};