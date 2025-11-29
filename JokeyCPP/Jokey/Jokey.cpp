#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "SemanticAnalyzer.h"
#include "CodeGenerator.h"

int main()
{
    system("chcp 65001 > nul");
    Lexer lexer("input.jk");
    lexer.analyse();
    const auto &tokens = lexer.getAllTokens();

    std::vector<Token> tks;
    for (auto &tk : tokens)
        tks.push_back(tk);

    Parser p(tks);
    try
    {
        auto program = p.parseProgram();

        SemanticAnalyzer sem;
        sem.analyze(program);
        if (!sem.errors().empty())
        {
            std::cerr << "Erros semanticos encontrados." << std::endl;
            return 1;
        }

        program->dump();

        CodeGenerator codeGen;
        std::string irCode = codeGen.generate(program);

        std::ofstream outFile("output.ll");
        if (outFile.is_open())
        {
            outFile << irCode;
            outFile.close();
            std::cout << "\n[Jokey] IR salvo em output.ll\n";
        }
        else
        {
            std::cerr << "Erro ao salvar output.ll\n";
            return 1;
        }

        std::cout << "\n[Jokey] Compilando com Clang...\n";

        int clangResult = system("clang output.ll print.c -o programa.exe -Wno-override-module");

        if (clangResult == 0)
        {
            std::cout << "[Jokey] Compilacao OK! Executando programa.exe:\n";
            std::cout << "--------------------------------------------------\n";

            int runResult = system("programa.exe");

            std::cout << "\n--------------------------------------------------\n";
            std::cout << "[Jokey] Programa finalizado com codigo " << runResult << "\n";
        }
        else
        {
            std::cerr << "[Jokey] Erro: Clang falhou na compilacao.\n";
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Erro sintatico: " << e.what() << std::endl;
        return 1;
    }

    system("pause");
    return 0;
}