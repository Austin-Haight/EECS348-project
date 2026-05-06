#pragma once

#include <string>
#include "Evaluator.h"

using namespace std;

class InputOutput
{
    private:
        string userString;
        // contains a list of allowed character inputs 
        string allowed = "1234567890-*%+/()";
    public:
        void setUserString();
        string getUserString();
        void startScreen();
        bool validateString();
        void printEvaluatorError(EvalResult);
        void printOutput(string);
};