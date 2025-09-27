#include "Lexer.h"

int main() {
    Lexer lexer("input.txt"); 
    lexer.analyse();
    lexer.printTokens();
    return 0;
}
