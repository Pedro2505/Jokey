#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"

int main() {
    Lexer lexer("input.jk");
    lexer.analyse();
    const auto& tokens = lexer.getAllTokens();

    std::vector<Token> tks;
    for (auto& tk : tokens) {
        tks.push_back(tk);
    }

    Parser p(tks);
    try {
        auto program = p.parseProgram();
        program->dump();
    }
    catch (const std::exception& e) {
        std::cerr << "Erro sintático: " << e.what() << std::endl;
    }
    return 0;
}