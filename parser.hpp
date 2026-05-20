#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "service.hpp"
#include "token.hpp"



class Parser {
private:

    inline static std::size_t pos = 0;

    static Token* current() {
        if (pos >= Token::tokens.size()) {
            return nullptr;
        }

        return Token::tokens[pos].get();
    }

    static Token* peek(int offset = 0) {
        std::size_t index = pos + offset;

        if (index >= Token::tokens.size()) {
            return nullptr;
        }

        return Token::tokens[index].get();
    }

    static bool match(TokenType type) {
        Token* tok = current();

        if (!tok) {
            return false;
        }

        return tok->token == type;
    }

    static Token* consume(TokenType type) {
        Token* tok = current();

        if (!tok) {
            std::cerr << "Unexpected end of file" << std::endl;
            exit(1);
        }

        if (tok->token != type) {
            std::cerr << "Unexpected token: expected "
               << static_cast<int>(type)
               << ", got "
               << static_cast<int>(tok->token) 
               << std::endl;

            exit(1);
        }

        ++pos;

        return tok;
    }

    static bool consume_if(TokenType type) {
        if (!match(type)) {
            return false;
        }

        pos++;

        return true;
    }

    static Token* advance() {
        Token* tok = current();

        if (pos < Token::tokens.size()) {
            ++pos;
        }

        return tok;
    }

    static bool eof() {
        return
            pos >= Token::tokens.size() ||
            match(TokenType::Eof);
    }

    static Service::AST::Expr_ptr parse_expr();
    static Service::AST::Expr_ptr parse_assign();
    static Service::AST::Expr_ptr parse_logor();
    static Service::AST::Expr_ptr parse_logand();
    static Service::AST::Expr_ptr parse_bitor();
    static Service::AST::Expr_ptr parse_bitxor();
    static Service::AST::Expr_ptr parse_bitand();
    static Service::AST::Expr_ptr parse_eq();
    static Service::AST::Expr_ptr parse_rel();
    static Service::AST::Expr_ptr parse_add();
    static Service::AST::Expr_ptr parse_mult();
    static Service::AST::Expr_ptr parse_prefix();
    static Service::AST::Expr_ptr parse_postfix();
    static Service::AST::Expr_ptr parse_primary();

    static Service::AST::Expr_ptr parse_expr() {
        return parse_assign();
    }

    static Service::AST::Expr_ptr parse_assign() {
        auto left = parse_logor();

        if (consume_if(TokenType::Equal)) {
            Service::AST::Expr_ptr right = parse_assign();

            return make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Assign,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

    static Service::AST::Expr_ptr parse_logor() {
        auto left = parse_logand();

        while (Token::tokens[pos]->token_data == "or") {
            ++pos;
            Service::AST::Expr_ptr right = parse_logor();

            left = make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Logor,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

    static Service::AST::Expr_ptr parse_logand() {
        auto left = parse_bitor();

        while (Token::tokens[pos]->token_data == "and") {
            Service::AST::Expr_ptr right = parse_logand();

            left = make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Logand,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

    static Service::AST::Expr_ptr parse_bitor() {
        auto left = parse_bitxor();

        while (consume_if(TokenType::Pipe)) {
            Service::AST::Expr_ptr right = parse_bitor();

            left = make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Bitor,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

    static Service::AST::Expr_ptr parse_bitxor() {
        auto left = parse_bitand();

        while (consume_if(TokenType::Caret)) {
            Service::AST::Expr_ptr right = parse_bitxor();

            left = make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Bitxor,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

    static Service::AST::Expr_ptr parse_bitand() {
        auto left = parse_eq();

        while (consume_if(TokenType::Amp)) {
            Service::AST::Expr_ptr right = parse_bitxor();

            left = make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Bitxor,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

    static Service::AST::Expr_ptr parse_eq() {
        auto left = parse_rel();

        while (consume_if(TokenType::)) {
            Service::AST::Expr_ptr right = parse_bitxor();

            left = make_unique<Service::AST::BinaryExpr>(
                Service::AST::Type::Bitxor,
                std::move(left),
                std::move(right)
            );
        }

        return left;
    }

public: 

    static Service::AST::Node *parse() {

    }


};
