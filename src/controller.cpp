#include "Tokenizer.cpp"
#include "Parser.cpp"

#include <iostream>
#include <string>
#include <list>
#include <memory>
#include <optional>
#include <vector>

using namespace std;

class Controller
{
private:
    vector<string> history;
    list<unique_ptr<Token>> tokens;
    Tokenizer tokenizer;

    bool isQuitCommand(const string& input)
    {
        return input == "quit" || input == "Quit" || input == "QUIT" ||
               input == "exit" || input == "Exit" || input == "EXIT";
    }

    bool isHistoryCommand(const string& input)
    {
        return input == "history" || input == "History" || input == "HISTORY";
    }

    void displayHistory()
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

    void displayTokens(const list<Token*>& tokens)
    {
        cout << "Parser output: ";

        for (Token* token : tokens)
        {
            cout << token->to_string() << " ";
        }

        cout << endl;
    }

    void processExpression(const string& input)
    {
        optional<TokenizerError> tokenizerError = tokenizer.tokenize(input);

        if (tokenizerError)
        {
            cout << "Error: Tokenizer error at index "
                 << tokenizerError->location << endl;
            return;
        }

        Parser parser;
        ParseResult parseResult = parser.parse(tokens);

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

        // Evaluator should be called here once but the parser and Evaluator dont agree yet on the token types.
    }

public:
    Controller()
    {
        tokenizer = Tokenizer(&tokens);
    }

    void run()
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
};