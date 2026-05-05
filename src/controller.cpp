#include "Tokenizer.cpp"
#include "Parser.cpp"
#include "InputOutput.cpp"

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

    bool isQuitCommand(const string& input)
    {
        return input == "quit" || input == "Quit" || input == "QUIT" ||
               input == "exit" || input == "Exit" || input == "EXIT";
    }

    bool isHistoryCommand(const string& input)
    {
        return input == "history" || input == "History" || input == "HISTORY";
    }

    // just for testing
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
        list<unique_ptr<Token>> tokens;

        Tokenizer tokenizer(tokens);
        optional<TokenizerError> tokenizerError = tokenizer.tokenize(input);

        if (tokenizerError)
        {
            io.printError(tokenizerError, "Tokenizer");
            return;
        }

        Parser parser;
        ParseResult parseResult = parser.parse(tokens);

        if (parseResult.error)
        {
            io.printParserError(parseResult.error, "Parser");
            return;
        }

        (parseResult.tokens);

        // Evaluator should be called here once but the parser and Evaluator dont agree yet on the token types.
    }

public:
    void run()
    {
        inputOutput io;

        io.startScreen();

        while (true)
        {
            do 
            {
                io.setUserString();

                if (isQuitCommand(io.getUserString()))
                {
                    io.endMessage();
                    break;
                }

                if (isHistoryCommand(io.getUserString()))
                {
                    io.displayHistory();
                    continue;
                }

            } while (!io.validateString())           

            processExpression(io.getUserString());
        }
    }
};