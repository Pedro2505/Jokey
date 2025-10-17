#pragma once
#include <map>
#include <set>
#include <string>
#include <utility>

class Automato {
public:
    int initialState;
    std::set<int> finalStates;
    std::map<std::pair<int, char>, int> transictions;
    std::map<int, std::string> finalToken; // nome do token em cada estado final

    Automato() = default;
    Automato(int initial, std::set<int> finals);

    void addTransiction(int origin, char symbol, int destination);
    bool accept(const std::string& str);

    // métodos auxiliares públicos
    int getInitialState() const { return initialState; }
    bool isFinal(int state) const { return finalStates.count(state) > 0; }

    int move(int state, char c) const {
        auto it = transictions.find({ state, c });
        return it != transictions.end() ? it->second : -1;
    }

    std::string getTokenName(int state) const {
        auto it = finalToken.find(state);
        return it != finalToken.end() ? it->second : "DESCONHECIDO";
    }
};
