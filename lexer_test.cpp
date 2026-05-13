#include "lexer.hpp"

std::string str1 = 
"var x = 1\nfun f(x, y):\n\t\"abc\"\n\t\tret x + y";

int main() {
    Service::Global::code = str1;
    Lexer::lex();
}