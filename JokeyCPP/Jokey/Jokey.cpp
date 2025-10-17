#include "GlobalAFNBuilder.h"
#include "GlobalAFD.h"
#include "Comments.h"
#include "Identificator.h"
#include "LiteralString.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include "ReservedWords.h"
#include "Operator.h"
#include "Boolean.h"
#include "Lexer.h"

#include <vector>
#include <iostream>

int main() {
    std::vector<Automato*> automata = {
        new Comments(),
        new Identificator(),
        new LiteralString(),
        new Integer(),
        new FloatingPoint(),
        new Operator(),
    };

    GlobalAFNBuilder builder;
    Automato globalAFN = builder.build(automata);

    GlobalAFD converter;
    Automato globalAFD = converter.convert(globalAFN);

    Lexer lexer("input.txt", globalAFD);
    lexer.analyse();
    lexer.printTokens();

    std::cout << "\n Análise léxica concluída com sucesso!" << std::endl;
}
