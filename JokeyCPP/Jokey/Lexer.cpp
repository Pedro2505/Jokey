#include "Lexer.h"
#include "Identificator.h"
#include "LiteralString.h"
#include "Integer.h"
#include "FloatingPoint.h"
#include "ReservedWords.h"
#include "Operator.h"
#include "Boolean.h"
#include "Comments.h"

#include <fstream>
#include <iostream>
#include <sstream>

Lexer::Lexer(const std::string& filename) : filename(filename) {}

void Lexer::analyse() {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Erro ao abrir o arquivo" << filename <<std::endl;
		return;
	}

	std::string line;
	int lineNumber = 1;
	while (std::getline(file, line)) {
		processLine(line, lineNumber);
		lineNumber++;
	}
}

void Lexer::processLine(std::string& line, int lineNumber) {
	Identificator afd;
	LiteralString literal_string;
	Integer integer;
	FloatingPoint floatingPoint;
	ReservedWords reservedWords;
	Operator operat;
	Boolean boolean;
	Comments comments;

	int col = 1;
	size_t i = 0;

	while (i < line.size()) {
		std::string token;
		char c = line[i];

		if (c == '/' && i + 1 < line.size() && line[i + 1] == '/') {
			token = line.substr(i);
			if (comments.accept(token)) {
				tokens.push_back({ token, "COMENTÁRIO" });
			}
			break;
		}
		else if (c == '/' && i + 1 < line.size() && line[i + 1] == '*') {
			size_t endComment = line.find("*/", i + 2);
			if (endComment == std::string::npos) {
				std::cerr << "Erro léxico: comentário de bloco não fechado | linha " << lineNumber << " | coluna " << col << std::endl;
				break;
			}
			else {
				token = line.substr(i, endComment - i + 2);
				if (comments.accept(token)) {
					tokens.push_back({ token, "COMENTÁRIO" });
				}
				i = endComment + 2;
				col += token.size();
				continue;
			}
		}
		else {
			size_t start = i;
			while (i < line.size() && !isspace(line[i]) && line[i] != '(' && line[i] != ')' &&
				line[i] != '{' && line[i] != '}' && line[i] != ';' && line[i] != ',') {
				i++;
			}
			token = line.substr(start, i - start);
			if (token.empty()) {
				i++;
				col++;
				continue;
			}

			if (reservedWords.isReserved(token)) tokens.push_back({ token, "RESERVADA" });
			else if (boolean.isBoolean(token)) tokens.push_back({ token, "BOOLEANO" });
			else if (integer.accept(token)) tokens.push_back({ token, "INTEIRP" });
			else if (floatingPoint.accept(token)) tokens.push_back({ token, "PONTO FLUTUANTE" });
			else if (literal_string.accept(token)) tokens.push_back({ token, "STRING" });
			else if (afd.accept(token)) tokens.push_back({ token, "IDENTIFICADOR" });
			else if (operat.accept(token)) tokens.push_back({ token, "OPERADOR" });
			else std::cerr << "Erro léxico no token: " << token << " | linha " << lineNumber << " | coluna " << col << std::endl;
		}
		col += token.size() + 1;
		i++;
	}
}

void Lexer::printTokens() {
	std::cout << "Token\tTipo" << std::endl;
	for (auto& token : tokens) {
		std::cout << token.first << "\t" << token.second << std::endl;
	}
}