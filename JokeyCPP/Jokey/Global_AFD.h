#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include "Automato.h"

class Global_AFD
{
	public:
		Global_AFD(std::vector<std::pair<std::string, Automato*>> tokens);
		int getStart();
		int transiction(int stateId, char c);

		int finalTokenIndex(int stateId);
		std::string tokenNameByIndex(int idx);
		void print_summary();

	private:
		std::map<std::string, int> reprToId;
		std::vector<std::vector<int>> idToRepr; 
		std::map<int, std::map<char, int>> trans;

		int startId;
		std::vector<std::string> tokenNames;
		std::vector<Automato*> automata;

		std::string reprKey(const std::vector<int>& v);
		int getOrCreateState(const std::vector<int>& repr);
		void build();
};

