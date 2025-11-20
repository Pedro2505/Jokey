#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "SemanticAnalyzer.h"

int main()
{
    Lexer lexer("input.jk");
    lexer.analyse();
    const auto &tokens = lexer.getAllTokens();

    std::vector<Token> tks;
    for (auto &tk : tokens)
    {
        tks.push_back(tk);
    }

    Parser p(tks);
    try
    {
        auto program = p.parseProgram();

        // semantic analysis
        SemanticAnalyzer sem;
        sem.analyze(program);
        if (!sem.errors().empty())
        {
            std::cerr << "Erros semânticos encontrados: " << sem.errors().size() << std::endl;
            return 1;
        }

        program->dump();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro sintatico: " << e.what() << std::endl;
    }
    return 0;
}