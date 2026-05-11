#include "service.hpp"

enum class TokenType {
    
    Plus, Minus,
    Star, Slash,
    Percent, Caret,

    Pipe, Amp,
    Tilda,

    Equal, Nonequal,
    LchevronEqual, RchevronEqual,
    Lchevron, Rchevron,
    
    Lparen, Rparen,

    Lbracket, Rbracket,

    Colon,

    Tab, Newline,
    
    Eof,

    Id,

};

class Token {
private:

    using TokenList = std::vector<Token*>;
    using TokenTable = std::unordered_map<std::string, TokenType>;

    inline static TokenTable token_table = {
        {"+", TokenType::Plus}, {"-", TokenType::Minus}, {"*", TokenType::Star}, 
        {"/", TokenType::Slash}, {"%", TokenType::Percent}, {"^", TokenType::Caret}, 
        {"|", TokenType::Pipe}, {"&", TokenType::Amp}, {"~", TokenType::Tilda}, 
        {"=", TokenType::Equal}, {"!=", TokenType::Nonequal}, {"<=", TokenType::LchevronEqual}, 
        {">=", TokenType::RchevronEqual}, {"<", TokenType::Lchevron}, {">", TokenType::Rchevron}, 
        {"(", TokenType::Lparen}, {")", TokenType::Rparen}, {"{", TokenType::Lbracket},
        {"}", TokenType::Rbracket}, {":", TokenType::Colon}, {"\t", TokenType::Tab},
        {"\n", TokenType::Newline}, {"", TokenType::Eof}
    };

    inline static TokenList tokens{};

    static TokenType get_type(std::string str) {
        return token_table[str];
    }

    ////////////////////////////////////////////////////

    Token(TokenType token, int start, int end) : token{token} {
        token_data = Service::Global::code.substr(start, end);
    }

    TokenType token;
    std::string_view token_data;

    friend class Lexer;

};