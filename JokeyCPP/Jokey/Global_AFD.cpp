#include "Global_AFD.h"
#include "Automato.h"
#include <queue>
#include <sstream>
#include <iostream>

Global_AFD::Global_AFD(std::vector<std::pair<std::string, Automato*>> tokens) {
    automata.reserve(tokens.size());
    tokenNames.reserve(tokens.size());
    for (auto &p : tokens) {
        tokenNames.push_back(p.first);
        automata.push_back(p.second);
    }
    
    std::vector<int> init;
    for (auto* a : automata) init.push_back(a->getInitial());
    startId = getOrCreateState(init);
    build();
}

std::string Global_AFD::reprKey(const std::vector<int>& v) {
    std::ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ',';
        oss << v[i];
    }
    return oss.str();
}

int Global_AFD::getOrCreateState(const std::vector<int>& repr) {
    std::string key = reprKey(repr);
    auto it = reprToId.find(key);
    if (it != reprToId.end()) return it->second;
    int nid = (int)idToRepr.size();
    reprToId[key] = nid;
    idToRepr.push_back(repr);
    return nid;
}

void Global_AFD::build() {
    std::queue<int> q;
    q.push(startId);
    std::set<int> visited;
    visited.insert(startId);

    while (!q.empty()) {
        int sid = q.front(); q.pop();
        const std::vector<int> repr = idToRepr[sid];

        for (int ci = 1; ci < 128; ++ci) {
            char c = (char)ci;
            std::vector<int> next;
            next.reserve(repr.size());
            bool anyValid = false;
            for (size_t i = 0; i < repr.size(); ++i) {
                int st = repr[i];
                if (st == -1) { next.push_back(-1); continue; }
                int nx = automata[i]->step(st, c);
                if (nx == -1) next.push_back(-1);
                else {
                    next.push_back(nx);
                    anyValid = true;
                }
            }
            if (!anyValid) continue;
            int nid = getOrCreateState(next);
            trans[sid][c] = nid;
            if (!visited.count(nid)) {
                visited.insert(nid);
                q.push(nid);
            }
        }
    }
}

int Global_AFD::transiction(int stateId, char c) {
    auto it = trans.find(stateId);
    if (it == trans.end()) return -1;
    auto it2 = it->second.find(c);
    if (it2 == it->second.end()) return -1;
    return it2->second;
}

int Global_AFD::getStart() {
    return startId;
}

std::string Global_AFD::finalToken(int stateId) {
    if (stateId < 0 || stateId >= (int)idToRepr.size()) return "";
    const auto &repr = idToRepr[stateId];
    
    for (size_t i = 0; i < repr.size(); ++i) {
        int st = repr[i];
        if (st == -1) continue;
        const std::set<int>& finals = automata[i]->getFinals();
        if (finals.count(st)) return tokenNames[i];
    }
    return "";
}

void Global_AFD::print_summary() {
    std::cout << "Estados combinados: " << idToRepr.size() << "\n";
    std::cout << "Inicial: " << startId << "\n";
    std::cout << "Prioridade:\n";
    for (size_t i = 0; i < tokenNames.size(); ++i) {
        std::cout << "  " << i << " : " << tokenNames[i] << "\n";
    }
}
