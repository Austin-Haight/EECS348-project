#pragma once

#include "Token.h"
#include "Error.h"
#include <unordered_map>
#include <string>
#include <list>
#include <optional>

using namespace std;

class DFA
{
 private:
  unordered_map<char, int> _map[8];
  bool _exit[8];
  int _state;
  int _count;
  int _pos;

  bool step(string, int);

 public:
  DFA();
  DFA(unordered_map<char, int>[8], bool[8]);
  void insert(int, pair<char, int>);
  void setState(int, bool);
  int read(const string, int);
};

class Tokenizer
{
 private:
  list<Token*>* _out;
  DFA intDFA;
  DFA floatDFA;
  int pos;
  int outlen;
  void initDFA();

 public:
  Tokenizer();
  Tokenizer(list<Token*>*);
  optional<TokenizerError> tokenize(string);
  //~Tokenizer();
};