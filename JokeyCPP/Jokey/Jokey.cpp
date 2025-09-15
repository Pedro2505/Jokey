#include <iostream>
#include "Identificator.h"
#include "LiteralString.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include "ReservedWords.h"

int main() {
    Identificator afd;
    LiteralString ls;
    Integer integer;
    FloatingPoint floatingPoint;
    ReservedWords reservedWords;


    std::string identificadoresTestes[] = { "abc", "a1b2", "1abc", "Abc123" };
    std::string stringsLiteraisTestes[] = { "''", "\"\"", "'1abc'", "\"Abc123\"", "\"a'", "'a\"", "\"a", "'a","a\"", "a'"};
    std::string inteirosTestes[] = { "''", "\"\"", "1", "2", "1234", "4321", "1.5", "1.", "1a", "1qv"};
    std::string floatTestes[] = { "''", "\"\"", "1", "2", "12", "432", "1.5", "1.", "1.23", "1.242.2", "0.23423452", "1.2342"};
    std::string palavrasReservadasTestes[] = { "Keep", "as", "str", "string", "inteeger" };


    std::cout << "Testando identificadores: " << std::endl;
    for (auto& i : identificadoresTestes) {
        bool ok = afd.accept(i);
        std::cout << i << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }
    
    std::cout << "Testando strings: " << std::endl;
    for (auto& str : stringsLiteraisTestes) {
        bool ok = ls.accept(str);
        std::cout << str << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }

    std::cout << "Testando inteiros: " << std::endl;
    for (auto& inteiro : inteirosTestes) {
        bool ok = integer.accept(inteiro);
        std::cout << inteiro << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }

    std::cout << "Testando floats: " << std::endl;
    for (auto& flt : floatTestes) {
        bool ok = floatingPoint.accept(flt);
        std::cout << flt << " -> " << (ok ? "ACEITO" : "REJEITADO") << std::endl;
    }

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


    return 0;
}
