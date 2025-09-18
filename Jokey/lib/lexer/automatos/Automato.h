#pragma once
#include <string>
#include <map>
#include <set>

class Automato
{
	public:
		Automato(int initial, std::set<int> finals);

		void addTransiction(int origin, char simbol, int destination);
		bool accept(const std::string& str);

	protected:
		int initialState;
		std::set<int> finalStates;
		std::map<std::pair<int, char>, int> transictions;
};

