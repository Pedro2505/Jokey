#include "LiteralString.h"

LiteralString::LiteralString() : Automato(0, { 2 }), quot_mark_open('\0') {
	addTransiction(0, '\"', 1);
	addTransiction(0, '\'', 1);

	for (char c = '0'; c <= 'z'; c++) {
		addTransiction(1, c, 1);
	}

	addTransiction(1, ' ', 1);
	addTransiction(1, '@', 1);
	addTransiction(1, '!', 1);
	addTransiction(1, '#', 1);
	addTransiction(1, '<', 1);
	addTransiction(1, '>', 1);
	addTransiction(1, '.', 1);
	addTransiction(1, ',', 1);
	addTransiction(1, ';', 1);
	addTransiction(1, ':', 1);
	addTransiction(1, '+', 1);
	addTransiction(1, '-', 1);
	addTransiction(1, '=', 1);
	addTransiction(1, '\\', 1);
	addTransiction(1, '/', 1);
	addTransiction(1, '|', 1);
	addTransiction(1, '}', 1);
	addTransiction(1, '{', 1);
	addTransiction(1, ']', 1);
	addTransiction(1, '[', 1);
	addTransiction(1, ')', 1);
	addTransiction(1, '(', 1);
	addTransiction(1, '*', 1);
	addTransiction(1, '$', 1);
	addTransiction(1, '%', 1);
}

bool LiteralString::accept(const std::string& str)  {
	int state = initialState;

	for (char c : str) {
		if (state == 0 && ((c == '\"') || (c == '\''))) {
			quot_mark_open = c;
		}

		auto it = transictions.find({ state, c });
		if (it == transictions.end()) {
			if (state == 1 && c == quot_mark_open) {
				state = 2;
				continue;
			}
			return false;
		}

		state = it->second;
	}
	return finalStates.count(state) > 0;
}	