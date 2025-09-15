#include "Integer.h"

Integer::Integer() : Automato(0, { 1 }) {
	for (char c = '0'; c <= '9'; c++) {
		addTransiction(0, c, 1);
	}

	for (char c = '0'; c <= '9'; c++) {
		addTransiction(1, c, 1);
	}
}