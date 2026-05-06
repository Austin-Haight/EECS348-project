#pragma once
#include "Tokenizer.h"
#include "Parser.h"
#include "Evaluator.h"
#include "Io.h"

#include <string>
#include <list>
#include <memory>
#include <vector>

class Controller
{
    private:
        vector<string> history;
        list<Token*> *tokens;
        Tokenizer tokenizer;
        Evaluator evaluator;
        Parser parser;
        InputOutput io;
        bool isQuitCommand(const string&);
        bool isHistoryCommand(const string&);
        void displayHistory();
        void displayTokens(const list<Token*>&);
        void processExpression(const string&);
    public:
        Controller();
        ~Controller();
        void run();
};