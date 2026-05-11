#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <string_view>

#include "token.hpp"
#include "service.hpp"


class Lexer {
private:
    static void build_token(std::string str, std::size_t len, ssize_t& iter) {
        Token *tmp = new Token{Token::get_type(str), 0, len};
        Token::tokens.push_back(tmp);
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
                } else {
                    for (auto elem : Token::tokens) delete elem;
                    std::cerr << "Invalid lexical construction: > " << Service::Global::code.substr(iter, iter + 16) << "..." << std::endl;
                    exit(1);
                }
            }

            
        }
    }

    // static void test() {
    //     Token *tmp = new Token{TokenType::Id, 0, 3};
    //     Token::tokens.push_back(tmp);
    //     std::cout << Token::tokens[0]->token_data << std::endl;
    // }
};
