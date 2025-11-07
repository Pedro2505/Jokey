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

Lexer::Lexer(const std::string& filename) : filename(filename) {
    auto* id = new Identificator();
    auto* integer = new Integer();
    auto* floating = new FloatingPoint();
    auto* op = new Operator();
    auto* boolean = new Boolean();
    auto* literal = new LiteralString();

    std::vector<std::pair<std::string, Automato*>> automatos = {
        {"LITERAL", literal},
        {"FLOATING", floating},
        {"INTEGER", integer},
        {"IDENTIFICATOR", id},
        {"OPERATOR", op}
    };

    combinedDFA = new Global_AFD(automatos);
}

Lexer::~Lexer() {
    delete combinedDFA;
}

void Lexer::loadFile() {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        std::cerr << "Erro ao abrir arquivo: " << filename << std::endl;
        return;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    buffer = ss.str();
    pos = 0;
    line = 1;
    col = 1;
}

char Lexer::peekChar(size_t offset) const {
    if (pos + offset >= buffer.size()) return '\0';
    return buffer[pos + offset];
}

void Lexer::advancePos(size_t n) {
    for (size_t k = 0; k < n && pos < buffer.size(); ++k) {
        char c = buffer[pos++];
        if (c == '\n') {
            line++;
            col = 1;
        }
        else if (c == '\t') {
            col += 4;
        }
        else {
            col++;
        }
    }
}

void Lexer::skipWhitespace() {
    while (!eof()) {
        char c = peekChar();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n')
            advancePos(1);
        else break;
    }
}

void Lexer::analyse() {
    loadFile();
    tokenizeAll();
}

const std::vector<Token>& Lexer::getAllTokens() const {
    return tokensAll;
}

void Lexer::printTokens() const {
    std::cout << "Lexeme\tTipo\tLinha:Col\n";
    for (const auto& t : tokensAll) {
        std::cout << t.lexeme << "\t" << t.type << "\t" << t.line << ":" << t.col << "\n";
    }
}

void Lexer::tokenizeAll() {
    tokensAll.clear();
    nextIdx = 0;

    ReservedWords reserved;
    Comments comments;

    while (!eof()) {
        skipWhitespace();
        if (eof()) break;

        int tokLine = line;
        int tokCol = col;

        if (peekChar() == '/' && peekChar(1) == '/') {
            size_t s = pos;
            while (s < buffer.size() && buffer[s] != '\n') s++;
            std::string lex = buffer.substr(pos, s - pos);
            tokensAll.emplace_back(lex, "COMENTÁRIO", tokLine, tokCol);
            advancePos(s - pos);
            continue;
        }

        if (peekChar() == '/' && peekChar(1) == '*') {
            size_t s = pos + 2;
            bool found = false;
            while (s + 1 < buffer.size()) {
                if (buffer[s] == '*' && buffer[s + 1] == '/') {
                    found = true;
                    s += 2;
                    break;
                }
                s++;
            }
            if (!found) {
                std::cerr << "Erro léxico: comentário de bloco não fechado | linha " << tokLine << " | coluna " << tokCol << std::endl;
                advancePos(buffer.size() - pos);
                break;
            }
            else {
                std::string lex = buffer.substr(pos, s - pos);
                tokensAll.emplace_back(lex, "COMENTÁRIO", tokLine, tokCol);
                advancePos(s - pos);
                continue;
            }
        }

        if (peekChar() == '"') {
            size_t s = pos + 1;
            bool closed = false;
            while (s < buffer.size()) {
                if (buffer[s] == '"' && buffer[s - 1] != '\\') { closed = true; s++; break; }
                s++;
            }
            if (!closed) {
                std::cerr << "Erro léxico: string não fechada | linha " << tokLine << " | coluna " << tokCol << std::endl;
                advancePos(buffer.size() - pos);
                break;
            }
            else {
                std::string lex = buffer.substr(pos, s - pos);
                LiteralString ls;
                if (ls.accept(lex))
                    tokensAll.emplace_back(lex, "STRING", tokLine, tokCol);
                else
                    std::cerr << "String inválida: " << lex << " | linha " << tokLine << " | coluna " << tokCol << std::endl;
                advancePos(s - pos);
                continue;
            }
        }

        char c0 = peekChar();
        if (c0 == '(' || c0 == ')' || c0 == '{' || c0 == '}' || c0 == ';' || c0 == ',') {
            std::string s(1, c0);
            tokensAll.emplace_back(s, "SEPARADOR", tokLine, tokCol);
            advancePos(1);
            continue;
        }

        int cur = combinedDFA->getStart();
        size_t j = pos;
        int lastFinalIndex = -1;
        size_t lastFinalPos = pos;

        while (j < buffer.size()) {
            char ch = buffer[j];
            int nxt = combinedDFA->transiction(cur, ch);
            if (nxt == -1) break;
            cur = nxt;
            int fidx = combinedDFA->finalTokenIndex(cur);
            if (fidx != -1) {
                lastFinalIndex = fidx;
                lastFinalPos = j + 1;
            }
            j++;
        }

        if (lastFinalIndex == -1) {
            std::cerr << "Erro léxico no token: '" << peekChar() << "' | linha " << tokLine << " | coluna " << tokCol << std::endl;
            advancePos(1);
            continue;
        }

        std::string lexeme = buffer.substr(pos, lastFinalPos - pos);
        std::string tokenType = combinedDFA->tokenNameByIndex(lastFinalIndex);

        if (tokenType == "IDENTIFICATOR") {
            if (reserved.isReserved(lexeme))
                tokenType = "RESERVADA";
            else {
                Boolean b;
                if (b.isBoolean(lexeme)) tokenType = "BOOLEANO";
                else tokenType = "IDENTIFICADOR";
            }
        }
        else if (tokenType == "INTEGER") tokenType = "INTEIRO";
        else if (tokenType == "FLOATING") tokenType = "PONTO_FLUTUANTE";
        else if (tokenType == "LITERAL") tokenType = "STRING";
        else if (tokenType == "OPERATOR") tokenType = "OPERADOR";

        tokensAll.emplace_back(lexeme, tokenType, tokLine, tokCol);
        advancePos(lastFinalPos - pos);
    }
}

Token Lexer::nextToken() {
    if (nextIdx >= tokensAll.size()) return Token("", "EOF", line, col);
    return tokensAll[nextIdx++];
}

Token Lexer::peek(int k) {
    size_t idx = nextIdx + (k - 1);
    if (idx >= tokensAll.size()) return Token("", "EOF", line, col);
    return tokensAll[idx];
}
