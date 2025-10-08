#include "Automato.h"

Automato::Automato(int initial, std::set<int> finals)
	: initialState(initial), finalStates(finals) { }

void Automato::addTransiction(int origin, char simbol, int destination) {
	transictions[{origin, simbol}] = destination;
}

bool Automato::accept(const std::string& str) {
	int state = initialState;

	for (char c : str) {
		auto it = transictions.find({ state, c });
		if (it == transictions.end()) {
			return false;
		}
		state = it->second;
	}

	return finalStates.count(state) > 0;
}

int Automato::step(int state, char c) {
	auto it = transictions.find({ state, c });
	if (it == transictions.end()) return -1;

	return it->second;
}

int Automato::getInitial() {
	return initialState;
}

const std::set<int> Automato::getFinals() {
	return finalStates;
}

const std::map<std::pair<int, char>, int> Automato::getTransictions() {
	return transictions;
}