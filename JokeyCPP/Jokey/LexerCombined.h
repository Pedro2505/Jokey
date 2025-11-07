#pragma once
#include "Global_AFD.h"
#include "Automato.h"
#include <vector>
#include <string>

class LexerCombined : public Global_AFD
{
public:
	LexerCombined(std::vector<std::pair<std::string, Automato*>> automatos);
	static LexerCombined* buildDefault();
};

