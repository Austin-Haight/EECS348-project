#include <string>

using namespace std;

class Error
{
};

class TokenizerError : Error
{
 public:
  int location;
  TokenizerError(int loc)
  {
   location = loc;
  }
};