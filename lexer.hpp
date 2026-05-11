#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>

#include "service.hpp"


class Lexer {
private:
    static void build_token(std::string str, std::size_t len, ssize_t& iter) {
        // auto token = new Token(); 
        iter += len;
    }

public:
    static void lex() {
        
        ssize_t iter = 0;
        ssize_t end = Service::Global::code.length();
        while (iter != end) {
            
            if (Service::Global::code[iter] == '#') {
                do ++iter; while (Service::Global::code[iter] != '\n');
                continue;
            }

            if (Service::Lexer::is_service_char(Service::Global::code[iter])) {
                if (auto offset = Service::Lexer::is_service_str(&Service::Global::code[iter]); offset > 0) {
                    build_token(&Service::Global::code[iter], offset, iter);
                    continue;
                }
            }
        }
    }

    static void test() {
        // Token::tokens;
    }
};
