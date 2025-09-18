#include "Comments.h"

Comments::Comments() : Automato(0, {2, 5}) {
	addTransiction(0, '/', 1);

	addTransiction(1, '/', 2);
	addTransiction(1, '*', 3);

	for (char c = '0'; c <= 'z'; c++) {
		addTransiction(2, c, 2);
	}

	addTransiction(2, ' ', 2);
	addTransiction(2, '@', 2);
	addTransiction(2, '!', 2);
	addTransiction(2, '#', 2);
	addTransiction(2, '<', 2);
	addTransiction(2, '>', 2);
	addTransiction(2, '.', 2);
	addTransiction(2, ',', 2);
	addTransiction(2, ';', 2);
	addTransiction(2, ':', 2);
	addTransiction(2, '+', 2);
	addTransiction(2, '-', 2);
	addTransiction(2, '=', 2);
	addTransiction(2, '\\',2);
	addTransiction(2, '/', 2);
	addTransiction(2, '|', 2);
	addTransiction(2, '}', 2);
	addTransiction(2, '{', 2);
	addTransiction(2, ']', 2);
	addTransiction(2, '[', 2);
	addTransiction(2, ')', 2);
	addTransiction(2, '(', 2);
	addTransiction(2, '*', 2);
	addTransiction(2, '$', 2);
	addTransiction(2, '%', 2);

	for (char c = '0'; c <= 'z'; c++) {
		addTransiction(3, c, 3);
	}

	addTransiction(3, '\n', 3);
	addTransiction(3, ' ', 3);
	addTransiction(3, '@', 3);
	addTransiction(3, '!', 3);
	addTransiction(3, '#', 3);
	addTransiction(3, '<', 3);
	addTransiction(3, '>', 3);
	addTransiction(3, '.', 3);
	addTransiction(3, ',', 3);
	addTransiction(3, ';', 3);
	addTransiction(3, ':', 3);
	addTransiction(3, '+', 3);
	addTransiction(3, '-', 3);
	addTransiction(3, '=', 3);
	addTransiction(3, '\\',3);
	addTransiction(3, '/', 3);
	addTransiction(3, '|', 3);
	addTransiction(3, '}', 3);
	addTransiction(3, '{', 3);
	addTransiction(3, ']', 3);
	addTransiction(3, '[', 3);
	addTransiction(3, ')', 3);
	addTransiction(3, '(', 3);
	addTransiction(3, '*', 3);
	addTransiction(3, '$', 3);
	addTransiction(3, '%', 3);

	addTransiction(3, '*', 4);
	addTransiction(4, '/', 5);
}