#include "Operator.h"

Operator::Operator() : Automato(0, {1, 2, 3, 4, 5, 6}) {
	addTransiction(0, '@', 1);
	addTransiction(0, '#', 1);
	addTransiction(0, '.', 1);
	addTransiction(0, ',', 1);
	addTransiction(0, '\\', 1);
	addTransiction(0, '|', 1);
	addTransiction(0, '}', 1);
	addTransiction(0, '{', 1);
	addTransiction(0, ']', 1);
	addTransiction(0, '[', 1);
	addTransiction(0, ')', 1);
	addTransiction(0, '(', 1);
	addTransiction(0, '$', 1);
	addTransiction(0, '%', 1);

	addTransiction(0, '<', 2);
	addTransiction(0, '+', 2);
	addTransiction(0, '-', 2);
	addTransiction(0, '*', 2);
	addTransiction(0, '/', 2);
	addTransiction(0, '>', 2);
	addTransiction(0, '!', 2);
	addTransiction(0, '=', 2);

	addTransiction(2, '=', 3);

	addTransiction(0, '&', 4);
	addTransiction(0, '|', 5);

	addTransiction(4, '&', 5);
	addTransiction(5, '|', 6);
}
