#include <iostream>
#include "Lexer.h"

int main() {
    Lexer lexer("input.jk");
    lexer.analyse();

    Token t = lexer.nextToken();     
    Token look = lexer.peek(1);      
    const auto& all = lexer.getAllTokens(); 
    lexer.printTokens();
    return 0;

}
