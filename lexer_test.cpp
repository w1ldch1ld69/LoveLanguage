#include "lexer.hpp"

std::string str1 = 
"space global:\n    var x = 1\n    var a = true\n    var y = -x\n    var z = x + y\n    class A:\n        var x = 63\n    fun main():\n        ret 0\nend global";

int main() {
    std::string str = "aksjdl";
    std::string str2 = "*";
    std::string str3 = "<=aksjdl";
    std::cout << Service::Lexer::is_service_str(str3) << std::endl;
    Service::Global::code = "askldjlwkllsa";
    Lexer::test();
}