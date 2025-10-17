#pragma once
#include "Automato.h"
#include <string>
#include <vector>
#include <utility>

class Lexer {
private:
    Automato globalAFD;
    std::string filename;
    std::vector<std::pair<std::string, std::string>> tokens;

public:
    Lexer(const std::string& filename, const Automato& afd);
    void analyse();
    void processLine(const std::string& line, int lineNumber);
    void printTokens();
};
