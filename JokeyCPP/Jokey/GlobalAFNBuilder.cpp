// GlobalAFNBuilder.cpp
#include "GlobalAFNBuilder.h"
#include <map>
#include <set>

Automato GlobalAFNBuilder::build(const std::vector<Automato*>& automata) {
    int globalInitial = 0;
    int offset = 1;

    // guardamos novas transições aqui antes de criar o Automato final
    std::map<std::pair<int, char>, int> transitions;
    std::set<int> finalStates;

    const int OFFSET_STEP = 1000; // garante não colisão entre IDs de autômatos

    for (const Automato* a : automata) {
        int aInitial = a->getInitialState() + offset;

        // epsilon (uso '\0' para representar epsilon)
        transitions[{globalInitial, '\0'}] = aInitial;

        // copia transições do automato a ajustando pelo offset
        const auto& tmap = a->transictions;
        for (const auto& kv : tmap) {
            int from = kv.first.first + offset;
            char sym = kv.first.second;
            int to = kv.second + offset;
            transitions[{from, sym}] = to;
        }

        // finais
        const auto& fins = a->finalStates;
        for (int f : fins) finalStates.insert(f + offset);

        offset += OFFSET_STEP;
    }

    Automato global(globalInitial, finalStates);
    for (const auto& kv : transitions) {
        int from = kv.first.first;
        char sym = kv.first.second;
        int to = kv.second;
        global.addTransiction(from, sym, to);
    }

    return global;
}
