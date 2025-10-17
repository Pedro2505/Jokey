#include "Lexer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>

Lexer::Lexer(const std::string& filename, const Automato& afd)
    : filename(filename), globalAFD(afd) {
}

void Lexer::analyse() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << filename << std::endl;
        return;
    }

    std::string line;
    int lineNumber = 1;
    while (std::getline(file, line)) {
        processLine(line, lineNumber);
        lineNumber++;
    }
}

void Lexer::processLine(const std::string& line, int lineNumber) {
    size_t i = 0;
    int col = 1;

    while (i < line.size()) {
        if (isspace(line[i])) {
            i++;
            col++;
            continue;
        }

        std::string token;
        size_t start = i;
        int lastFinalState = -1;
        size_t lastFinalPos = start;

        int state = globalAFD.getInitialState(); // novo método que precisa estar público
        for (; i < line.size(); ++i) {
            char c = line[i];

            int nextState = globalAFD.move(state, c); // novo método também
            if (nextState == -1) break;
            state = nextState;
            token.push_back(c);

            if (globalAFD.isFinal(state)) {
                lastFinalState = state;
                lastFinalPos = i;
            }
        }

        if (lastFinalState == -1) {
            std::cerr << "Erro léxico no token: " << token << " | linha " << lineNumber << " | coluna " << col << std::endl;
            i++;
            col++;
            continue;
        }

        std::string validToken = line.substr(start, lastFinalPos - start + 1);
        std::string tokenName = globalAFD.getTokenName(lastFinalState); // vem do AFN original
        tokens.push_back({ validToken, tokenName });

        i = lastFinalPos + 1;
        col += validToken.size();
    }
}

void Lexer::printTokens() {
    std::cout << "Token\tTipo" << std::endl;
    for (auto& token : tokens) {
        std::cout << token.first << "\t" << token.second << std::endl;
    }
}
