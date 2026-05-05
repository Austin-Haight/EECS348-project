#include "Tokenizer.cpp"
#include "Parser.cpp"
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

void runTest(string input)
{
    cout << "==========================" << endl;
    cout << "Input: " << input << endl;

    list<unique_ptr<Token>> tokens;
    Tokenizer tokenizer(tokens);

    optional<TokenizerError> tokError = tokenizer.tokenize(input);

    if (tokError)
    {
        cout << "Tokenizer error at index: " << tokError->location << endl;
        return;
    }

    cout << "Tokenizer output: ";
    for (auto &t : tokens)
    {
        cout << t->to_string() << " ";
    }
    cout << endl;

    Parser parser;
    ParseResult result = parser.parse(tokens);

    if (result.error)
    {
        cout << "Parser error at index " << result.error->position
             << ": " << result.error->message << endl;
    }
    else
    {
        cout << "Parser prefix output: ";
        for (Token *t : result.tokens)
        {
            cout << t->to_string() << " ";
        }
        cout << endl;
    }
}

int main()
{
    vector<string> tests = {
        "3 + 4 * 2",
        "(3 + 4) * 2",
        "8 - (5 - 2)",
        "10 - 3 - 2",
        "2 ** 3 ** 2",
        "42",
        "((2 + 3) * (4 - 1))",
        "3 +",
        "(3 + 4",
        "3 + 4)",
        "3 + * 4",
        "3 **",
        "-5 + 3",
        "-(2 + 3) * 4",
        "3.14 * 2",
        "2.5 ** 3",
        "3 + 4 * 2 / (1 - 5) ** 2 ** 3",
        "3 + 4 * 2 / (1 - 5) ** 2 ** 3 + 6",
        "3 + 4 * 2 / (1 - 5) ** 2 ** 3 + 6 - 7 % 2",
    };

    for (const string &test : tests)
    {
        runTest(test);
    }

    return 0;
}