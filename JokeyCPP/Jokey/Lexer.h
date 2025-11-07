#pragma once
#include <string>
#include <vector>
#include "Token.h"
#include "LexerCombined.h"

class Lexer {
public:
    Lexer(const std::string& filename);
    ~Lexer();

    void analyse();
    Token nextToken();
    Token peek(int k = 1);
    const std::vector<Token>& getAllTokens() const;

    void printTokens() const;

private:
    std::string filename;
    std::string buffer;   
    size_t pos = 0;       
    int line = 1;
    int col = 1;

    LexerCombined* combinedDFA = nullptr;
    std::vector<Token> tokensAll;
    size_t nextIdx = 0;           

    void loadFile();
    void tokenizeAll();              
    bool eof() const { return pos >= buffer.size(); }
    char peekChar(size_t offset = 0) const;
    void advancePos(size_t n);
    void skipWhitespace();
};
