#pragma once
#include <string>

struct Token
{
    std::string lexeme;
    std::string type;  
    int line;          
    int col;           

    Token() : lexeme(""), type(""), line(0), col(0) {}

    Token(std::string l, std::string t, int ln, int c)
        : lexeme(std::move(l)), type(std::move(t)), line(ln), col(c) {
    }
};
