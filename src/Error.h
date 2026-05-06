#pragma once

class Error{};

class TokenizerError
{
    public:
        int location;
        TokenizerError(int);
};