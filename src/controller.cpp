#include "controller.h"
#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <optional>
#include <vector>

using namespace std;

bool Controller::isQuitCommand(const string& input)
{
    return input == "quit" || input == "Quit" || input == "QUIT" ||
           input == "exit" || input == "Exit" || input == "EXIT";
}

bool Controller::isHistoryCommand(const string& input)
{
    return input == "history" || input == "History" || input == "HISTORY";
}

void Controller::displayHistory()
{
    if (history.empty())
    {
        cout << "No past expressions in this session." << endl;
        return;
    }

    cout << "Expression History:" << endl;

    for (int i = 0; i < history.size(); i++)
    {
        cout << i + 1 << ".) " << history[i] << endl;
    }
}

void Controller::displayTokens(const list<Token*>& tokens)
{
    cout << "Parser output: ";

    for (Token* token : tokens)
    {
        cout << token->to_string() << " ";
    }

    cout << endl;
}

void Controller::processExpression(const string& input)
{
    optional<TokenizerError> tokenizerError = tokenizer.tokenize(input);

    if (tokenizerError)
    {
        cout << "Error: Tokenizer error at index "
            << tokenizerError->location << endl;
        return;
    }

    ParseResult parseResult = parser.parse(*tokens);

    if (parseResult.error)
    {
        cout << "Error: Parser error at index "
            << parseResult.error->position
            << ": " << parseResult.error->message << endl;
        return;
    }

    history.push_back(input);

    cout << "Input: " << input << endl;
    displayTokens(parseResult.tokens);

    EvalResult evalResult = evaluator.evaluate(parseResult.tokens);

    if(evalResult.errorType!=0)
    {
        io.printEvaluatorError(evalResult);
    }
    else
    {
        io.printOutput(to_string(evalResult.value));
    }
}

Controller::Controller()
{
    tokens = new list<Token*>;
    tokenizer = Tokenizer(tokens);
}

Controller::~Controller()
{
    while(!(*tokens).empty())
    {
        delete ((*tokens).front());
        (*tokens).pop_front();
    }
    delete tokens;
}

void Controller::run()
{
    string input;

    cout << "Arithmetic Expression Evaluator" << endl;
    cout << "Enter an arithmetic expression." << endl;
    cout << "Type history to view previous expressions." << endl;
    cout << "Type quit to exit." << endl;

    while (true)
    {
        cout << endl;
        cout << "Expression: ";
        getline(cin, input);

        if (isQuitCommand(input))
        {
            cout << "Program ended." << endl;
            break;
        }

        if (isHistoryCommand(input))
        {
            displayHistory();
            continue;
        }

        if (input.empty())
        {
            cout << "Error: Empty expression." << endl;
            continue;
        }

        processExpression(input);
    }
}
