#include <iostream>
#include "Identificator.h"
#include "LiteralString.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include "ReservedWords.h"
#include "Operator.h"
#include "Boolean.h"
#include "Comments.h"
#include "Lexer.h"

int main() {
    /*Identificator afd;
    LiteralString ls;
    Integer integer;
    FloatingPoint floatingPoint;
    ReservedWords reservedWords;
    Operator op;
    Boolean boolean;
    Comments comments;*/

    //std::string identificadoresTestes[] = { "abc", "a1b2", "1abc", "Abc123" };
    //std::string stringsLiteraisTestes[] = { "''", "\"\"", "'1abc'", "\"Abc123\"", "'ABC DEF GFEA'", "\"a'", "'a\"", "\"a", "'a","a\"", "a'"};
    //std::string inteirosTestes[] = { "''", "\"\"", "1", "2", "1234", "4321", "1.5", "1.", "1a", "1qv"};
    //std::string floatTestes[] = { "''", "\"\"", "1", "2", "12", "432", "1.5", "1.", "1.23", "1.242.2", "0.23423452", "1.2342"};
    //std::string palavrasReservadasTestes[] = { "Keep", "as", "str", "string", "inteeger" };
    //std::string operadoresTestes[] = { "+", "-", "{}", "=", ">", ">=", "<=", "+=", "-=", "*=", "/=", "&", "&&", "|", "||"};
    //std::string booleanosTestes[] = { "True", "true", "Ttrue", "Truee", "False", "false", "ffalsee", "FFalsee" };
    //std::string comentariosTestes[] = { "// Teste", "//", "/ Teste", "/ Teste /", "/* Teste */", "/* Teste", "/* Teste /", "/* Teste *", "Teste */", "// Teste */" , "/* Teste //", "/* Teste \n Teste */"};

    /*std::cout << "Testando identificadores: " << std::endl;
    for (auto& i : identificadoresTestes) {
        bool ok = afd.accept(i);
        std::cout << i << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Testando strings: " << std::endl;
    for (auto& str : stringsLiteraisTestes) {
        bool ok = ls.accept(str);
        std::cout << str << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Testando inteiros: " << std::endl;
    for (auto& inteiro : inteirosTestes) {
        bool ok = integer.accept(inteiro);
        std::cout << inteiro << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Testando floats: " << std::endl;
    for (auto& flt : floatTestes) {
        bool ok = floatingPoint.accept(flt);
        std::cout << flt << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Testando palavras reservadas: " << std::endl;
    for (auto& palavra : palavrasReservadasTestes) {
        if (reservedWords.isReserved(palavra)) {
            std::cout << palavra << " -> PALAVRA RESERVADA" << std::endl;
        }
        else if (afd.accept(palavra)) {
            std::cout << palavra << " -> IDENTIFICADOR" << std::endl;
        }
        else {
            std::cout << palavra << " -> REJEITADO" << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "Testando operadores: " << std::endl;
    for (auto& operador : operadoresTestes) {
        bool ok = op.accept(operador);
        std::cout << operador << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Testando booleanos: " << std::endl;
    for (auto& booleano : booleanosTestes) {
        if (boolean.isBoolean(booleano)) {
            std::cout << booleano << " -> BOOLEANO" << std::endl;
        }
        else if (afd.accept(booleano)) {
            std::cout << booleano << " -> IDENTIFICADOR" << std::endl;
        }
        else {
            std::cout << booleano << " -> REJEITADO" << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "Testando comentários: " << std::endl;
    for (auto& comentario : comentariosTestes) {
        bool ok = comments.accept(comentario);
        std::cout << comentario << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }

    return 0*/;

    Lexer lexer("input.txt");
    lexer.analyse();
    lexer.printTokens();
    return 0;

}
