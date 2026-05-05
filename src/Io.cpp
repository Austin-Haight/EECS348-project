#include <iostream>
#include <string>
#include <iomanip>
#include "controller.cpp"
#include "evaluator.cpp"
#include "Tokenizer.cpp"

using namespace std;

class inputOutput {
    public: 
        void setUserString()
        {
            cout << "Enter a valid expression: " << endl;
            getline(cin, userString);
        }

        string getUserString()
        {
            return userString;
        }

        void startScreen()
        {
            cout << "Program start" << endl;
            
            while(true)
            {
                
                setUserString();

                while(!validateString())
                {
                    setUserString();
                } // end while                                  

                // TO DO: print output
            } // end while 
        } // end startScreen()

        bool validateString()
        {
            int stringLength = this->userString.length();

            if (stringLength <= 2)
            {
                cout << endl << "Invalid input: String can't be of length <= 2 ";
                return false;
            }

            if (stringLength > 9999)
            {
                cout << endl << "Invalid input: String cannot exceed 9999 characters ";
                return false;
            }

            // scans string and returns the first value that is not in the allowed string
                // if npos is returned then all the characters are valid
            if (this->userString.find_first_not_of(this->allowed) != std::string::npos)
            {
                cout << endl << "Invalid input: invalid character read";
                return false;
            }

            return true;
        }

        // Error handling for Evaluator results
    void printEvaluatorError(EvalResult res) {
        cout << "----------------------------" << endl;
        
        switch(res.errorType) {
            case NO_ERROR:
                // Use fixed and setprecision for clean float output
                cout << "Result: " << fixed << setprecision(2) << res.value << endl;
                break;

            case DIV_ZERO:
                cout << "Evaluation Error [Code 1]: Division or Modulo by zero is undefined." << endl;
                break;

            case SYNTAX_ERROR:
                cout << "Evaluation Error [Code 2]: Invalid prefix syntax. Please check operand count." << endl;
                break;

            case UNKNOWN_TOKEN:
                cout << "Evaluation Error [Code 3]: Unrecognized token encountered during evaluation." << endl;
                break;

            default:
                cout << "Unknown Critical Error occurred." << endl;
                break;
        }
        cout << "----------------------------" << endl;
    }

    void printOutput(string output)
    {
        cout << endl << "Final output: " << output;
    }
    private: 
        string userString;

        // contains a list of allowed character inputs 
        string allowed = "1234567890-*%+/()";
};

int main()
{
    inputOutput io;
    io.setUserString();
    cout << io.getUserString();
    io.validateString();
      
}