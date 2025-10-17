// GlobalAFD.cpp
#include "GlobalAFD.h"
#include <queue>
#include <map>
#include <set>
#include <vector>

// calcula epsilon-closure (usando '\0' como epsilon)
static std::set<int> epsilonClosure(const Automato& afn, int start) {
    std::set<int> closure;
    std::queue<int> q;
    closure.insert(start);
    q.push(start);

    const auto& trans = afn.transictions;

    while (!q.empty()) {
        int s = q.front(); q.pop();
        for (const auto& kv : trans) {
            int from = kv.first.first;
            char sym = kv.first.second;
            int to = kv.second;
            if (from == s && sym == '\0') {
                if (!closure.count(to)) {
                    closure.insert(to);
                    q.push(to);
                }
            }
        }
    }
    return closure;
}

Automato GlobalAFD::convert(const Automato& afn) {
    std::map<std::set<int>, int> stateMap;
    std::map<std::pair<int,char>, int> transitions;
    std::set<int> finalStates;
    std::queue<std::set<int>> pending;

    int nextStateId = 0;

    std::set<int> startClosure = epsilonClosure(afn, afn.getInitialState());
    stateMap[startClosure] = nextStateId++;
    pending.push(startClosure);

    const auto& allTrans = afn.transictions;

    while (!pending.empty()) {
        std::set<int> currentSet = pending.front(); pending.pop();
        int currentId = stateMap[currentSet];

        // moves: símbolo -> conjunto de estados alcançados diretamente (sem epsilon)
        std::map<char, std::set<int>> moves;

        // para cada estado s no conjunto atual, procura transições saindo dele
        for (int s : currentSet) {
            for (const auto& kv : allTrans) {
                int from = kv.first.first;
                char sym = kv.first.second;
                int to = kv.second;

                if (from == s && sym != '\0') {
                    moves[sym].insert(to);
                }
            }
        }

        // para cada símbolo em moves, calcula closure dos destinos e registra novo estado AFD
        for (const auto& mv : moves) {
            char symbol = mv.first;
            const std::set<int>& dests = mv.second;

            // closureSet reúne epsilon-closures de cada destino
            std::set<int> closureSet;
            for (int d : dests) {
                std::set<int> c = epsilonClosure(afn, d);
                closureSet.insert(c.begin(), c.end());
            }

            // se novo conjunto de estados, atribui id e enfileira
            if (!stateMap.count(closureSet)) {
                stateMap[closureSet] = nextStateId++;
                pending.push(closureSet);
            }

            transitions[{currentId, symbol}] = stateMap[closureSet];
        }
    }

    // define estados finais do AFD: qualquer estado cujo conjunto contém um estado final do AFN
    for (const auto& kv : stateMap) {
        const std::set<int>& setStates = kv.first;
        int id = kv.second;
        for (int s : setStates) {
            if (afn.finalStates.count(s)) {
                finalStates.insert(id);
                break;
            }
        }
    }

    Automato afd(0, finalStates);
    for (const auto& kv : transitions) {
        int from = kv.first.first;
        char sym = kv.first.second;
        int to = kv.second;
        afd.addTransiction(from, sym, to);
    }

    return afd;
}
