#pragma once
#include "Automato.h"

class LiteralString: public Automato
{
	public: 
		LiteralString();

		virtual bool accept(const std::string& str);

	protected:
		char quot_mark_open;
};

