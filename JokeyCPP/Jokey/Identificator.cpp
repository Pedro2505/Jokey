#include "Identificator.h"
#include <cctype>

Identificator::Identificator()
    : Automato(0, { 1 })
{
    for (char c = 'a'; c <= 'z'; c++) {
        addTransiction(0, c, 1);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        addTransiction(0, c, 1);
    }


    for (char c = 'a'; c <= 'z'; c++) {
        addTransiction(1, c, 1);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        addTransiction(1, c, 1);
    }
    for (char c = '0'; c <= '9'; c++) {
        addTransiction(1, c, 1);
    }
}
