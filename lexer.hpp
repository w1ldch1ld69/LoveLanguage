#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "token.hpp"
#include "service.hpp"



class Lexer {
private:
    static void build_token(std::string str, ssize_t& iter, std::size_t len) {
        if (len == 0) {
            Token *tmp = new Token{TokenType::Eof, iter -1, len};
            Token::tokens.push_back(tmp);
            return;
        }
        if (auto token = Token::get_type(str.substr(0, len)); token) {
            Token *tmp = new Token{*token, iter, len};
            Token::tokens.push_back(tmp);
            if (!(*token == TokenType::Newline || *token == TokenType::Tab)) Service::Lexer::is_newline = false;
            else Service::Lexer::is_newline = true;
            iter += len;
        } else {
            for (auto elem : Token::tokens) delete elem;
            std::cerr << "Invalid lexical construction: > \"" << Service::Global::code.substr(iter, 16) << "...\"" << std::endl;
            exit(1);
        }
    }

public:
    static void lex() {
        
        ssize_t iter = 0;
        const ssize_t end = Service::Global::code.length();
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
                    build_token(&Service::Global::code[iter], iter, offset);
                    continue;
                } else {
                    for (auto elem : Token::tokens) delete elem;
                    std::cerr << "Invalid lexical construction: > \"" << Service::Global::code.substr(iter, 16) << "...\"" << std::endl;
                    exit(1);
                }
            }

            if (Service::Lexer::is_id_start(Service::Global::code[iter])) {
                ssize_t offset = 0;
                do ++offset; while (Service::Lexer::is_id_char(Service::Global::code[iter + offset]));
                build_token(&Service::Global::code[iter], iter, offset);
                continue;                
            }

            if (Service::Lexer::is_digit(Service::Global::code[iter])) {
                if (auto offset = Service::Lexer::is_number(&Service::Global::code[iter]); offset < 0) {
                    for (auto elem : Token::tokens) delete elem;
                    std::cerr << "Number is built incorrectly: > \"" << Service::Global::code.substr(iter, -offset) << "...\"" << std::endl;
                    exit(1);
                } 

                else if (offset > 0) {
                    build_token(&Service::Global::code[iter], iter, offset);
                    continue;
                }

                else {
                    std::cerr << "Undefined behavior in lex() -> is_number()..." << std::endl;
                    exit(1);
                }
            }

            if (Service::Global::code[iter] == '"') {
                ssize_t offset = 1;
                while (Service::Global::code[iter + offset] != '"') ++offset;
                build_token(&Service::Global::code[iter], iter, offset + 1);
                continue;
            }

            if (Service::Global::code[iter] == ' ') {
                std::cerr << "Lexically unaccepted usage of space at: > \"" << Service::Global::code.substr(iter, 6) << "...\"" << std::endl;
                exit(1);
            }
            
        }

        build_token("", iter, 0);

        Token::print_tokens();
    }

};
