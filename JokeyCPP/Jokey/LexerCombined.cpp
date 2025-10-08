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

static LexerCombined* buildCombinedAFD() {
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

void Lexer::processLine(std::string& line, int lineNumber) {
    ReservedWords reservedWords;
    Comments comments;

    size_t i = 0;
    int col = 1;

    Global_AFD * dfa = buildCombinedAFD();

    while (i < line.size()) {
        if (line[i] == '/' && i + 1 < line.size() && line[i + 1] == '/') {
            std::string token = line.substr(i);
            if (comments.accept(token)) tokens.push_back({ token, "COMENTÁRIO" });
            break;
        }
        
        if (line[i] == '/' && i + 1 < line.size() && line[i + 1] == '*') {
            size_t endComment = line.find("*/", i + 2);
            if (endComment == std::string::npos) {
                std::cerr << "Erro léxico: comentário de bloco não fechado | linha " << lineNumber << " | coluna " << col << std::endl;
                break;
            }
            else {
                std::string token = line.substr(i, endComment - i + 2);
                if (comments.accept(token)) tokens.push_back({ token, "COMENTÁRIO" });
                col += (int)token.size();
                i = endComment + 2;
                continue;
            }
        }

        if (std::isspace((unsigned char)line[i])) {
            if (line[i] == '\t') col += 4;
            else col++;
            i++;
            continue;
        }

        int cur = dfa->getStart();
        int lastFinalState = -1;
        std::string lastFinalTokenName = "";
        size_t lastFinalPos = i;

        size_t j = i;
        int steps = 0;
        while (j < line.size()) {
            char c = line[j];
            int nxt = dfa->transiction(cur, c);
            if (nxt == -1) break;
            cur = nxt;
            steps++;
            std::string tokName = dfa->finalToken(cur);
            if (!tokName.empty()) {
                lastFinalState = cur;
                lastFinalTokenName = tokName;
                lastFinalPos = j + 1;
            }
            j++;
        }

        if (lastFinalState == -1) {
            char c = line[i];
            std::string s(1, c);
            if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == ',') {
                tokens.push_back({ s, "SEPARADOR" });
                i++; col++;
                continue;
            }

            if (c == '"') {
                size_t endq = line.find('"', i + 1);
                if (endq == std::string::npos) {
                    std::cerr << "Erro léxico: string não fechada | linha " << lineNumber << " | coluna " << col << std::endl;
                    break;
                }
                else {
                    std::string tok = line.substr(i, endq - i + 1);
                    LiteralString ls;
                    if (ls.accept(tok)) tokens.push_back({ tok, "STRING" });
                    else std::cerr << "Erro string inválida: " << tok << " | linha " << lineNumber << " | coluna " << col << std::endl;
                    col += (int)tok.size();
                    i = endq + 1;
                    continue;
                }
            }

            std::cerr << "Erro léxico no token (caractere inesperado): '" << line[i] << "' | linha " << lineNumber << " | coluna " << col << std::endl;
            i++; col++;
            continue;
        }

        std::string lexeme = line.substr(i, lastFinalPos - i);
        std::string tokenType = lastFinalTokenName;

        if (tokenType == "IDENTIFICATOR") {
            if (reservedWords.isReserved(lexeme)) tokenType = "RESERVADA";
            else {
                Boolean b;
                if (b.isBoolean(lexeme)) tokenType = "BOOLEANO";
            }
        }
        else if (tokenType == "INT") tokenType = "INTEIRO";
        else if (tokenType == "FLOAT") tokenType = "PONTO FLUTUANTE";
        else if (tokenType == "LITERAL") tokenType = "STRING";
        else if (tokenType == "OPERATOR") tokenType = "OPERADOR";

        tokens.push_back({ lexeme, tokenType });

        for (char ch : lexeme) {
            if (ch == '\t') col += 4;
            else col++;
        }
        i = lastFinalPos;
    }

    delete dfa;
}
