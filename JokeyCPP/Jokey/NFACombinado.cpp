#include "NFACombinado.h"
#include <algorithm>  // Para std::max

NFACombinado::NFACombinado(std::vector<Automato*> subAutomatos)
    : Automato(0, std::set<int>())  // Inicializa com estado inicial 0 (ser� sobrescrito)
{
    buildCombinedNFA(subAutomatos);
}

void NFACombinado::buildCombinedNFA(std::vector<Automato*> subAutomatos) {
    std::set<int> allStates;  // Todos os estados combinados
    std::map<std::pair<int, char>, std::set<int>> allTransitions;  // Todas as transi��es
    std::set<int> allFinalStates;  // Todos os estados finais
    int newInitialState = 0;  // Novo estado inicial (vamos definir como 0 e renumerar tudo)

    int offset = 0;  // Offset inicial para renumerar estados

    for (auto& subAuto : subAutomatos) {
        int subInitial = subAuto->getInitial();
        std::set<int> subFinals = subAuto->getFinals();
        auto subTransitions = subAuto->getTransictions();

        // Renumerar estados do sub-automato
        int subOffset = offset;  // Offset para este sub-automato
        std::map<std::pair<int, char>, std::set<int>> renumberedTransitions;

        for (const auto& trans : subTransitions) {
            int originalOrigin = trans.first.first;
            char symbol = trans.first.second;
            std::set<int> destinations = trans.second;

            int renumberedOrigin = originalOrigin + subOffset;  // Renumerar origem
            std::set<int> renumberedDestinations;
            for (int dest : destinations) {
                renumberedDestinations.insert(dest + subOffset);  // Renumerar destinos
            }

            renumberedTransitions[{renumberedOrigin, symbol}] = renumberedDestinations;
            allTransitions[{renumberedOrigin, symbol}].insert(renumberedDestinations.begin(), renumberedDestinations.end());  // Adicionar ao global
        }

        // Adicionar estados renumerados
        for (int state : subAuto->getEstados()) {  // Assumindo que voc� tem um m�todo getEstados(), ou use os de transi��es
            allStates.insert(state + subOffset);
        }

        // Adicionar estados finais renumerados
        for (int finalState : subFinals) {
            allFinalStates.insert(finalState + subOffset);
        }

        // Adicionar transi��o epsilon do novo estado inicial para o estado inicial renumerado
        allTransitions[{newInitialState, '\0'}].insert(subInitial + subOffset);  // Epsilon para o estado inicial renumerado

        // Atualizar offset para o pr�ximo sub-automato (baseado no n�mero de estados no atual)
        offset += *subAuto->getEstados().rbegin() + 1;  // Exemplo: soma o maior estado +1; ajuste se necess�rio
    }

    // Agora, configure o NFACombinado com os dados combinados
    this->initialState = newInitialState;  // Define o estado inicial
    this->finalStates = allFinalStates;  // Define os estados finais
    this->transictions = allTransitions;  // Define as transi��es

    // Se voc� tiver um m�todo setEstados, use-o; caso contr�rio, adicione os estados conforme necess�rio
}

int NFACombinado::getNextStateOffset(int currentMax, int offsetIncrement) {
    return currentMax + offsetIncrement;  // Simples incremento
}