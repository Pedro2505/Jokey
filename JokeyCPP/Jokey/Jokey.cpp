#include <iostream>
#include "Identificator.h"
#include "LiteralString.h"

int main() {
    Identificator afd;
    LiteralString ls;

    std::string identificadoresTestes[] = { "abc", "a1b2", "1abc", "Abc123" };
    std::string stringsLiteraisTestes[] = { "''", "\"\"", "'1abc'", "\"Abc123\"", "\"a'", "'a\"", "\"a", "'a","a\"", "a'"};


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

    return 0;
}
