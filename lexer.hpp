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
        if (auto token = Token::get_type(str.substr(0, len)); token) {
            Token *tmp = new Token{*token, 0, len};
            if (Token::tokens.empty() || Token::tokens.back()->token == TokenType::Newline) Service::Lexer::is_newline = false;
            Token::tokens.push_back(tmp);
            iter += len;
            if (*token == TokenType::Newline) Service::Lexer::is_newline = true;
        } else {
            for (auto elem : Token::tokens) delete elem;
            std::cerr << "Invalid lexical construction: > " << Service::Global::code.substr(iter, iter + 16) << "..." << std::endl;
            exit(1);
        }
    }

public:
    static void lex() {
        
        ssize_t iter = 0;
        ssize_t end = Service::Global::code.length();
        while (iter != end) {
            if (!Service::Lexer::is_newline) {
                while (Service::Global::code[iter] == ' ' || Service::Global::code[iter] == '\t') ++iter;
            }

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

            if (Service::Lexer::is_id_start(Service::Global::code[iter])) {
                ssize_t offset = 0;
                do ++offset; while (Service::Lexer::is_id_char(Service::Global::code[iter + offset]));
                build_token(&Service::Global::code[iter], offset + 1, iter);
                continue;                
            }

            if (Service::Lexer::is_digit(Service::Global::code[iter])) {
                if (auto offset = Service::Lexer::is_number(&Service::Global::code[iter]); offset < 0) {
                    for (auto elem : Token::tokens) delete elem;
                    std::cerr << "Number is built incorrectly: > " << Service::Global::code.substr(iter, iter - offset) << "..." << std::endl;
                    exit(1);
                } 

                else if (offset > 0) {
                    build_token(&Service::Global::code[iter], offset, iter);
                    continue;
                }

                else {
                    std::cerr << "Undefined behavior in lex() -> is_number()..." << std::endl;
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
