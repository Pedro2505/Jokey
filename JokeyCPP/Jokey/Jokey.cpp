#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"

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
            for (const auto& err : sem.errors()) {
                std::cerr << "  " << err << std::endl;
            }
            return 1;
        }

        program->dump();

        // code generation
        CodeGenerator codeGen;
        std::string irCode = codeGen.generate(program);
        
        // Imprime o IR gerado
        std::cout << "\n=== LLVM IR Gerado ===\n";
        std::cout << irCode;
        
        // Salva em arquivo
        std::ofstream outFile("output.ll");
        if (outFile.is_open()) {
            outFile << irCode;
            outFile.close();
            std::cout << "\nIR salvo em output.ll\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro sintatico: " << e.what() << std::endl;
    }
    return 0;
}