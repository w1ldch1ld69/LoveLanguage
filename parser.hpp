#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <exception>

#include "service.hpp"
#include "token.hpp"



class Parser {
private:

    inline static std::size_t pos = 0;
    inline static std::size_t depth = 0;
    inline static std::size_t line = 1;

    static std::variant<std::size_t, double> strton() {
        std::string str(current()->token_data);
        if (str.contains('.')) {
            double num = std::stod(str);
            ++pos;
            return num;
        } else {
            size_t num = std::stol(str);
            ++pos;
            return num;
        }
    }

    static std::size_t align() {
        std::size_t iter = 0;

        while (1) {
            Token* tok = peek(iter);

            if (!tok || tok->token != TokenType::Tab)
                break;

            ++iter;
        }

        return iter;
    }

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

    template <class... Strs>
    requires (std::is_convertible_v<Strs, std::string> && ...)
    static bool match(Strs... ids) {
        Token* tok = current();

        if (!tok) {
            return false;
        }

        return ((tok->token_data == ids) || ...);
    }

    template <class... Ts>
    static bool match(Ts... types) {
        Token* tok = current();

        if (!tok) {
            return false;
        }

        bool ret_val = ((tok->token == types) || ...);

        return ret_val;
    }

    static std::string_view consume(TokenType type) {
        Token* tok = current();

        if (!tok) {
            std::cerr << "Unexpected end of file" << std::endl;
            exit(1);
        }

        if (tok->token != type) {
            std::cerr << "Line " << line << ": Unexpected token: expected "
               << static_cast<int>(type)
               << ", got "
               << static_cast<int>(tok->token) 
               << std::endl;

            exit(1);
        }

        ++pos;

        if (type == TokenType::Newline) ++line;

        return tok->token_data;
    }

    template <class... Ts>
    static bool consume_if(Ts... types) {
        if (!match(types...)) {
            return false;
        }

        ++pos;

        return true;
    }

    template <class... Strs>
    requires (std::is_convertible_v<Strs, std::string> && ...)
    static bool consume_if(Strs... ids) {
        if (!match(ids...)) {
            return false;
        }

        ++pos;

        return true;
    }
    

    static TokenType advance() {
        Token* tok = current();

        if (pos < Token::tokens.size()) {
            ++pos;
        }

        return tok->token;
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
    static Service::AST::Expr_ptr parse_mul();
    static Service::AST::Expr_ptr parse_prefix();
    static Service::AST::Expr_ptr parse_postfix();
    static Service::AST::Expr_ptr parse_primary();

    

    static Service::AST::Stmt_ptr parse_stmt() {
        if (Token::tokens[pos]->token == TokenType::Eof) {
            return nullptr;
        }
        std::size_t alignment = align();
        if (alignment < depth) {
            std::cout << alignment << " < " << depth << std::endl;
            return nullptr;
        }
        else if (alignment != depth) {
            // error
            exit(0);
        }
        pos += alignment;

        
        Service::AST::Stmt_ptr stmt;
        std::size_t cur_line = line;
        
        if (consume_if("var")) {
            std::string_view id = consume(TokenType::Id);
            Service::AST::Expr_ptr expr = nullptr;
            if (consume_if(TokenType::Assign)) {
                expr = parse_expr();
            }
            Service::AST::decl_stmt var_decl = std::make_unique<Service::AST::VarDecl>(
                Service::AST::Type::Var,
                cur_line,
                std::move(id),
                std::move(expr)
            );
            stmt = std::make_unique<Service::AST::DeclStmt>(
                Service::AST::Type::Var,
                line,
                std::move(var_decl)
            );
            consume(TokenType::Newline);
            return stmt;
        }

        else if (consume_if("class")) {
            std::string_view id = consume(TokenType::Id);
            consume(TokenType::Colon);
            consume(TokenType::Newline);
            ++depth;
            std::vector<Service::AST::Decl_ptr> decl_list = parse_decl_list();
            --depth;
            Service::AST::decl_stmt class_decl = std::make_unique<Service::AST::ClassDecl>(
                Service::AST::Type::Class,
                cur_line,
                id, 
                std::move(decl_list)
            );
            stmt = std::make_unique<Service::AST::DeclStmt>(
                Service::AST::Type::Class,
                line,
                std::move(class_decl)
            );
            return stmt;
        }

        else if (consume_if("if")) {
            std::vector<Service::AST::cond_then_block> blocks;
            auto cond = parse_expr();
            consume(TokenType::Colon);
            consume(TokenType::Newline);
            ++depth;
            auto body = parse_stmt_list();
            --depth;
            blocks.emplace_back(std::move(cond), std::move(body));
            alignment = align();
            if (alignment < depth) {
                std::cout << alignment << " < " << depth << std::endl;
                return stmt;
            }
            else if (alignment != depth) {
                // error
                exit(0);
            }
            pos += alignment;
            while (consume_if("elif")) {
                auto cond = parse_expr();
                consume(TokenType::Colon);
                consume(TokenType::Newline);
                ++depth;
                auto body = parse_stmt_list();
                --depth;
                blocks.emplace_back(std::move(cond), std::move(body));
            }
            if (alignment < depth) {
                std::cout << alignment << " < " << depth << std::endl;
                return stmt;
            }
            else if (alignment != depth) {
                // error
                exit(0);
            }
            pos += alignment;
            if (consume_if("else")) {
                consume(TokenType::Colon);
                consume(TokenType::Newline);
                ++depth;
                auto body = parse_stmt_list();
                --depth;
                blocks.emplace_back(nullptr, std::move(body));
            }

            stmt = std::make_unique<Service::AST::CondStmt>(
                Service::AST::Type::Cond,
                cur_line,
                std::move(blocks)
            );

            return stmt;
        }

        else if (consume_if("while")) {
            Service::AST::Expr_ptr cond_expr = parse_expr();
            consume(TokenType::Colon);
            consume(TokenType::Newline);
            ++depth;
            std::vector<Service::AST::Stmt_ptr> stmt_list = parse_stmt_list();
            --depth;
            stmt = std::make_unique<Service::AST::WhileStmt>(
                Service::AST::Type::While,
                cur_line,
                std::move(cond_expr),
                std::move(stmt_list)
            );
            return stmt;
        }

        else if (consume_if("ret")) {
            Service::AST::Expr_ptr ret_expr = nullptr;
            if (!match(TokenType::Newline)) ret_expr = parse_expr();
            stmt = std::make_unique<Service::AST::RetStmt>(
                Service::AST::Type::Ret,
                cur_line,
                std::move(ret_expr)
            );
            consume(TokenType::Newline);
            return stmt;
        }
        
        else if (consume_if("break")) {
            stmt = std::make_unique<Service::AST::BreakStmt>(
                Service::AST::Type::Break,
                line
            );
            consume(TokenType::Newline);
            return stmt;
        }

        else if (consume_if("cont")) {
            stmt = std::make_unique<Service::AST::ContStmt>(
                Service::AST::Type::Cont,
                cur_line
            );
            consume(TokenType::Newline);
            return stmt;
        }

        else {
            Service::AST::Expr_ptr expr = parse_expr();
            stmt = std::make_unique<Service::AST::ExprStmt>(
                Service::AST::Type::Expr,
                cur_line,
                std::move(expr)
            );
            consume(TokenType::Newline);
            return stmt;
        }
    }

    static Service::AST::Decl_ptr parse_decl() {
        if (Token::tokens[pos]->token == TokenType::Eof) {
            return nullptr;
        }
        std::size_t alignment = align();
        if (alignment < depth) {
            std::cout << alignment << " < " << depth << std::endl;
            return nullptr;
        }
        else if (alignment != depth) {
            // error
            std::cout << "here" << std::endl;
            exit(0);
        }
        pos += alignment;

        Service::AST::Decl_ptr decl;
        std::size_t cur_line = line;
        
        if (consume_if("var")) {
            std::string_view id = consume(TokenType::Id);
            Service::AST::Expr_ptr expr = nullptr;
            if (consume_if(TokenType::Assign)) {
                expr = parse_expr();
            }
            decl = std::make_unique<Service::AST::VarDecl>(
                Service::AST::Type::Var,
                cur_line,
                std::move(id),
                std::move(expr)
            );
            consume(TokenType::Newline);
            return decl;
        }
        else if (consume_if("fun")) {
            std::string_view id = consume(TokenType::Id);
            
            consume(TokenType::Lparen);
            std::vector<std::string_view> params;
            
            if (match(TokenType::Id)) {
                auto param = consume(TokenType::Id);
                params.push_back(param);
            }
                
            while (consume_if(TokenType::Comma)) {
                auto param = consume(TokenType::Id);
                params.push_back(param);
            }
            consume(TokenType::Rparen);
            consume(TokenType::Colon);
            consume(TokenType::Newline);
            ++depth;
            Service::AST::body_t body = parse_stmt_list();
            --depth;
            decl = std::make_unique<Service::AST::FunDecl>(
                Service::AST::Type::Fun,
                cur_line,
                id,
                std::move(params),
                std::move(body)
            );
            return decl;
        }
        else if (consume_if("class")) {
            std::string_view id = consume(TokenType::Id);
            consume(TokenType::Colon);
            consume(TokenType::Newline);
            ++depth;
            std::vector<Service::AST::Decl_ptr> decl_list = parse_decl_list();
            --depth;
            decl = std::make_unique<Service::AST::ClassDecl>(
                Service::AST::Type::Class,
                cur_line,
                id, 
                std::move(decl_list)
            );
            return decl;
        }
        else if (consume_if("space")) {
            std::string_view id = consume(TokenType::Id);
            consume(TokenType::Colon);
            consume(TokenType::Newline);
            ++depth;
            std::vector<Service::AST::Decl_ptr> decl_list = parse_decl_list();
            --depth;
            decl = std::make_unique<Service::AST::SpaceDecl>(
                Service::AST::Type::Space,
                cur_line,
                id, 
                std::move(decl_list)
            );
            return decl;
        }

        else {
            std::cout << "no decls found" << std::endl;
            return nullptr;
        } 

        
    }

    static std::vector<Service::AST::Stmt_ptr> parse_stmt_list() {
        std::vector<Service::AST::Stmt_ptr> stmt_list;
        while (auto stmt = parse_stmt()) {
            stmt_list.push_back(std::move(stmt));
        }
        return stmt_list;
    }

    static std::vector<Service::AST::Decl_ptr> parse_decl_list() {
        std::vector<Service::AST::Decl_ptr> decl_list;
        while (auto decl = parse_decl()) {
            decl_list.push_back(std::move(decl));
        }
        return decl_list;
    }

public: 

    static void parse() {
        Service::AST::parser_tree = parse_decl_list();
    }

};

Service::AST::Expr_ptr Parser::parse_expr() {
    return parse_assign();
}

Service::AST::Expr_ptr Parser::parse_assign() {
    auto left = parse_logor();
    if (consume_if(TokenType::Assign)) {
        Service::AST::Expr_ptr right = parse_assign();
        return make_unique<Service::AST::BinaryExpr>(
            Service::AST::Type::Assign,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_logor() {
    auto left = parse_logand();
    while (consume_if("or")) {
        Service::AST::Expr_ptr right = parse_logand();
        left = make_unique<Service::AST::BinaryExpr>(
            Service::AST::Type::Logor,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_logand() {
    auto left = parse_bitor();
    while (consume_if("and")) {
        Service::AST::Expr_ptr right = parse_bitor();
        left = make_unique<Service::AST::BinaryExpr>(
            Service::AST::Type::Logand,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_bitor() {
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
Service::AST::Expr_ptr Parser::parse_bitxor() {
    auto left = parse_bitand();
    while (consume_if(TokenType::Caret)) {
        Service::AST::Expr_ptr right = parse_bitand();
        left = make_unique<Service::AST::BinaryExpr>(
            Service::AST::Type::Bitxor,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_bitand() {
    auto left = parse_eq();
    while (consume_if(TokenType::Amp)) {
        Service::AST::Expr_ptr right = parse_eq();
        left = make_unique<Service::AST::BinaryExpr>(
            Service::AST::Type::Bitand,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_eq() {
    auto left = parse_rel();
    while (match(TokenType::Equal, TokenType::Nonequal)) {
        
        Service::AST::Type op;
        switch (advance()) {
            case TokenType::Equal:      op = Service::AST::Type::Eqop; break;
            case TokenType::Nonequal:   op = Service::AST::Type::Neqop; break; 
        }
        Service::AST::Expr_ptr right = parse_rel();
        left = make_unique<Service::AST::BinaryExpr>(
            op,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_rel() {
    auto left = parse_add();
    while (match(TokenType::Lchevron, TokenType::Rchevron, TokenType::LchevronEqual, TokenType::RchevronEqual)) {
        
        Service::AST::Type op;
        switch (advance()) {
            case TokenType::Lchevron:       op = Service::AST::Type::Lop; break;
            case TokenType::Rchevron:       op = Service::AST::Type::Gop; break;
            case TokenType::LchevronEqual:  op = Service::AST::Type::Leop; break;
            case TokenType::RchevronEqual:  op = Service::AST::Type::Geop; break; 
        }
        Service::AST::Expr_ptr right = parse_add();
        left = make_unique<Service::AST::BinaryExpr>(
            op,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_add() {
    auto left = parse_mul();
    while (match(TokenType::Plus, TokenType::Minus)) {
        
        Service::AST::Type op;
        switch (advance()) {
            case TokenType::Plus:   op = Service::AST::Type::Add; break;
            case TokenType::Minus:  op = Service::AST::Type::Sub; break; 
        }
        Service::AST::Expr_ptr right = parse_mul();
        left = make_unique<Service::AST::BinaryExpr>(
            op,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_mul() {
    auto left = parse_prefix();
    while (match(TokenType::Star, TokenType::Slash, TokenType::Percent)) {
        
        Service::AST::Type op;
        switch (advance()) {
            case TokenType::Star:       op = Service::AST::Type::Mul; break;
            case TokenType::Slash:      op = Service::AST::Type::Div; break; 
            case TokenType::Percent:    op = Service::AST::Type::Mod; break;
        }
        Service::AST::Expr_ptr right = parse_prefix();
        left = make_unique<Service::AST::BinaryExpr>(
            op,
            std::move(left),
            std::move(right)
        );
    }
    return left;
}
Service::AST::Expr_ptr Parser::parse_prefix() {
    Service::AST::Expr_ptr arg;
    if (match(TokenType::Tilda) || match("not")) {
        
        Service::AST::Type op;
        if (advance() == TokenType::Tilda) {
            op = Service::AST::Type::Bitnot;
        } else {
            op = Service::AST::Type::Lognot;
        }
        
        arg = parse_prefix();
        arg = make_unique<Service::AST::PrefixExpr>(
            op,
            std::move(arg)
        );
    } else {
        arg = parse_postfix();
    }
    return arg;
}
Service::AST::Expr_ptr Parser::parse_postfix() {
    Service::AST::Expr_ptr expr = parse_primary();
    while (true) {
        if (consume_if(TokenType::Lparen)) {
            std::vector<Service::AST::Expr_ptr> args;
            if (!match(TokenType::Rparen)) {
                do {
                    args.push_back(parse_expr());
                } while (consume_if(TokenType::Comma));
            }
            consume(TokenType::Rparen);
            expr = make_unique<Service::AST::CallExpr>(
                Service::AST::Type::Call,
                std::move(expr),
                std::move(args)
            );
            continue;
        }
        if (consume_if(TokenType::Lbracket)) {
            Service::AST::Expr_ptr index = parse_expr();
            consume(TokenType::Rbracket);
            expr = make_unique<Service::AST::IndexExpr>(
                Service::AST::Type::Index,
                std::move(expr),
                std::move(index)
            );
            continue;
        }
        if (match(TokenType::Dot, TokenType::DoubleColon)) {
            Service::AST::Type type = (match(TokenType::Dot)) ? Service::AST::Type::DotAccess : Service::AST::Type::DcolonAccess;
            advance();
            std::string_view tmp = consume(TokenType::Id);
            expr = make_unique<Service::AST::AccessExpr>(
                type,
                std::move(expr),
                tmp
            );
            continue;
        }
        break;
    }
    return expr;
}   
Service::AST::Expr_ptr Parser::parse_primary() {
    Service::AST::Expr_ptr expr;
    if (match(TokenType::Id)) {
        std::string_view tmp = consume(TokenType::Id);
        if (tmp == "pass") {
            expr = std::make_unique<Service::AST::PassExpr>(
                Service::AST::Type::Pass
            );    
        } else if (tmp == "true") {
            expr = std::make_unique<Service::AST::BoolExpr>(
                Service::AST::Type::Bool,
                true
            );
        } else if (tmp == "false") {
            expr = std::make_unique<Service::AST::BoolExpr>(
                Service::AST::Type::Bool,
                false
            );
        } else {
            expr = std::make_unique<Service::AST::IdExpr>(
                Service::AST::Type::Id,
                tmp
            );
        }
    } else if (match(TokenType::Number)) {
        std::variant<std::size_t, double> num = strton();
        expr = std::make_unique<Service::AST::NumberExpr>(
            Service::AST::Type::Num,
            std::move(num)
        );
    } else if (match(TokenType::String)) {
        std::string_view str(consume(TokenType::String));
        expr = std::make_unique<Service::AST::StringExpr>(
            Service::AST::Type::Str,
            std::move(str)
        );
    } else if (consume_if(TokenType::Lparen)) {
        expr = parse_expr();
        consume(TokenType::Rparen);
        expr = std::make_unique<Service::AST::GroupExpr>(
            Service::AST::Type::Group,
            std::move(expr)
        );
    }
    return expr;
}
