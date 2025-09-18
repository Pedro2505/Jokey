#pragma once
#include "Automato.h"
#include <set>
#include <string>

class Boolean
{
	public:
		Boolean();

		bool isBoolean(std::string& str);

	protected:
		std::set<std::string> booleans;
};

