#include "LiteralString.h"

LiteralString::LiteralString() : Automato(0, {2}), quot_mark_open('\0')
{
	addTransiction(0, '\"', 1);
	addTransiction(0, '\'', 1);

	for (char c = '0'; c <= 'z'; c++)
	{
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

bool LiteralString::accept(const std::string &str)
{
	int state = initialState;
	quot_mark_open = '\0';

	for (char c : str)
	{
		auto it = transictions.find({state, c});

		if (it != transictions.end())
		{
			state = it->second;
			if (state == 1 && quot_mark_open == '\0')
			{
				quot_mark_open = c;
			}
			continue;
		}

		if (state == 1)
		{
			if (c == quot_mark_open)
			{
				state = 2;
			}
			else if (c == '\n' || c == '\r')
			{
				return false;
			}
			else
			{
				state = 1;
			}
			continue;
		}

		return false;
	}

	return finalStates.count(state) > 0;
}