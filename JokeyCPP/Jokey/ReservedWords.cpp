#include "ReservedWords.h"

ReservedWords::ReservedWords() {
    reservedWords = {
        "Keep","as","static","show","if","elsif","then","end",
        "during","ForEach","to","repeat","until","define","regress",
        "Class","property","component",
        "integer","floatingPoint","string","boolean","null",
        "and","or","not"
    };
}

bool ReservedWords::isReserved(const std::string& word) const
{
    return reservedWords.count(word) > 0;
}