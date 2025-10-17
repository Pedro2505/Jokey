// GlobalAFNBuilder.h
#pragma once
#include "Automato.h"
#include <vector>

class GlobalAFNBuilder {
public:
    Automato build(const std::vector<Automato*>& automata);
};
