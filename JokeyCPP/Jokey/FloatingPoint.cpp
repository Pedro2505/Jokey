#include "FloatingPoint.h"

FloatingPoint::FloatingPoint() : Automato(0, { 3 }) {
	for (char c = '0'; c <= '9'; c++) {
		addTransiction(0, c, 1);
	}

	for (char c = '0'; c <= '9'; c++) {
		addTransiction(1, c, 1);
	}

	addTransiction(1, '.', 2);

	for (char c = '0'; c <= '9'; c++) {
		addTransiction(2, c, 3);
	}

	for (char c = '0'; c <= '9'; c++) {
		addTransiction(3, c, 3);
	}
}