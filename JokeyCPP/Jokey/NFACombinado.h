#ifndef NFACOMBINADO_H
#define NFACOMBINADO_H

#include "Automato.h"
#include <vector>
#include <set>
#include <map>

class NFACombinado : public Automato {
public:
    NFACombinado(std::vector<Automato*> subAutomatos);  // Construtor que recebe os sub-automatos

private:
    void buildCombinedNFA(std::vector<Automato*> subAutomatos);
    int getNextStateOffset(int currentMax, int offsetIncrement);  // Função auxiliar para renumerar estados
};

#endif  // NFACOMBINADO_H