#pragma once
#include <string>
#include <vector>

class Lexer
{
	public:
		Lexer(const std::string& filename);
		void analyse();
		void printTokens();

	private:
		std::string filename;
		std::vector<std::pair<std::string, std::string>> tokens;
		void processLine(std::string& line, int lineNumber);
};

