#include "LexerCombined.h"
#include "Lexer.h"
#include "Identificator.h"
#include "LiteralString.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include "ReservedWords.h"
#include "Operator.h"
#include "Boolean.h"
#include "Comments.h"
#include "Global_AFD.h"
#include "Automato.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>

LexerCombined::LexerCombined(std::vector<std::pair<std::string, Automato*>> automatos)
    : Global_AFD(automatos) {}

LexerCombined* LexerCombined::buildDefault() {
    auto* id = new Identificator();
    auto* integer = new Integer();
    auto* floating = new FloatingPoint();
    auto* op = new Operator();
    auto* boolean = new Boolean();
    auto* literal = new LiteralString();

    std::vector<std::pair<std::string, Automato*>> tokens = {
        std::pair<std::string, Automato*>{"LITERAL", literal},
        std::pair<std::string, Automato*>{"FLOATING POINT", floating},
        std::pair<std::string, Automato*>{"INTEGER", integer},
        std::pair<std::string, Automato*>{"IDENTIFICATOR", id},
        std::pair<std::string, Automato*>{"OPERATOR", op}
    };

    return new LexerCombined(tokens);
}