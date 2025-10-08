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

void Lexer::printTokens() {
	std::cout << "Token\tTipo" << std::endl;
	for (auto& token : tokens) {
		std::cout << token.first << "\t" << token.second << std::endl;
	}
}