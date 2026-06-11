#pragma once

#include <memory>
#include "service.hpp"

enum class TokenType {
    
    Plus, Minus,
    Star, Slash,
    Percent, Caret,

    Pipe, Amp,
    Tilda,

    Assign,

    Equal, Nonequal,
    LchevronEqual, RchevronEqual,
    Lchevron, Rchevron,
    
    Lparen, Rparen,

    Lbracket, Rbracket,

    DoubleColon, Colon,

    Dot, Comma,

    Tab, Newline,
    
    Eof,

    Id, String,

    Number,

};

class Token {
private:

    using TokenList = std::vector<std::unique_ptr<Token>>;
    using TokenTable = std::unordered_map<std::string, TokenType>;

    inline static TokenTable token_table = {
        {"+", TokenType::Plus}, {"-", TokenType::Minus}, {"*", TokenType::Star}, 
        {"/", TokenType::Slash}, {"%", TokenType::Percent}, {"^", TokenType::Caret}, 
        {"|", TokenType::Pipe}, {"&", TokenType::Amp}, {"~", TokenType::Tilda}, 
        {"=", TokenType::Assign}, {"==", TokenType::Equal}, {"!=", TokenType::Nonequal}, 
        {"<=", TokenType::LchevronEqual}, {">=", TokenType::RchevronEqual}, {"<", TokenType::Lchevron}, 
        {">", TokenType::Rchevron}, {"(", TokenType::Lparen}, {")", TokenType::Rparen}, 
        {"{", TokenType::Lbracket}, {"}", TokenType::Rbracket}, {"::", TokenType::DoubleColon}, 
        {":", TokenType::Colon}, {".", TokenType::Dot}, {",", TokenType::Comma}, 
        {"\t", TokenType::Tab}, {"\n", TokenType::Newline}, {"", TokenType::Eof}
    };

    inline static TokenList tokens{};

    static std::optional<TokenType> get_type(std::string str) {
        if (auto type = token_table.find(str); type != token_table.end()) return type->second;
        if (str[0] >= '0' && str[0] <= '9') return TokenType::Number;
        if (Service::Lexer::is_id_start(str[0])) return TokenType::Id;
        if (str[0] == '"' && str.back() == '"') return TokenType::String;
        return std::nullopt;
    }

    ////////////////////////////////////////////////////

    static void print_tokens() {
        std::cout << "[\n";
        for (auto tok = tokens.begin(); tok != tokens.end(); tok++) {
            if (!*tok) continue;

            std::cout << "  Token(";

            switch ((*tok)->token) {
                case TokenType::Plus:           std::cout << "Plus"; break;
                case TokenType::Minus:          std::cout << "Minus"; break;
                case TokenType::Star:           std::cout << "Star"; break;
                case TokenType::Slash:          std::cout << "Slash"; break;
                case TokenType::Percent:        std::cout << "Percent"; break;
                case TokenType::Caret:          std::cout << "Caret"; break;

                case TokenType::Pipe:           std::cout << "Pipe"; break;
                case TokenType::Amp:            std::cout << "Amp"; break;
                case TokenType::Tilda:          std::cout << "Tilda"; break;

                case TokenType::Assign:          std::cout << "Assign"; break;
                case TokenType::Equal:          std::cout << "Equal"; break;
                case TokenType::Nonequal:       std::cout << "Nonequal"; break;

                case TokenType::LchevronEqual:  std::cout << "LchevronEqual"; break;
                case TokenType::RchevronEqual:  std::cout << "RchevronEqual"; break;

                case TokenType::Lchevron:       std::cout << "Lchevron"; break;
                case TokenType::Rchevron:       std::cout << "Rchevron"; break;

                case TokenType::Lparen:         std::cout << "Lparen"; break;
                case TokenType::Rparen:         std::cout << "Rparen"; break;

                case TokenType::Lbracket:       std::cout << "Lbracket"; break;
                case TokenType::Rbracket:       std::cout << "Rbracket"; break;

                case TokenType::DoubleColon:    std::cout << "DoubleColon"; break;
                case TokenType::Colon:          std::cout << "Colon"; break;
                case TokenType::Dot:          std::cout << "Dot"; break;
                case TokenType::Comma:          std::cout << "Comma"; break;

                case TokenType::Tab:            std::cout << "Tab"; break;
                case TokenType::Newline:        std::cout << "Newline"; break;

                case TokenType::Id:             std::cout << "Id"; break;
                case TokenType::Number:         std::cout << "Number"; break;
                case TokenType::String:         std::cout << "String"; break;

                case TokenType::Eof:            std::cout << "Eof"; break;

                default:
                    std::cout << "Unknown";
            }

            if ((*tok)->token != TokenType::Newline && (*tok)->token != TokenType::Eof) std::cout << ", \"" << (*tok)->token_data << "\")\n";
            else std::cout << ")\n";
        }

        std::cout << "]\n";
    }

    ////////////////////////////////////////////////////

    TokenType token;
    std::string_view token_data;

    ////////////////////////////////////////////////////

    friend class std::unique_ptr<Token>;
    friend class Lexer;

    friend class Parser;

public:

    Token(TokenType token, std::size_t start, std::size_t len) : token{token}, token_data{&Service::Global::code[start], len} {}

};