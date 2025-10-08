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

		int step(int state, char c);
		int getInitial();
		const std::set<int> getFinals();
		const std::map<std::pair<int, char>, int> getTransictions();

	protected:
		int initialState;
		std::set<int> finalStates;
		std::map<std::pair<int, char>, int> transictions;
};

