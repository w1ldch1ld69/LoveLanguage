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
    using TokenTable = std::unordered_map<Service::Lexer::Strings, TokenType>;

    // static TokenTable Token_table = {
    //     {}
    // };

    // static TokenList *tokens = new TokenList{};

    Token(TokenType token, int start, int end) : token{token} {
        token_data = Service::Global::code.substr(start, end);
    }

    TokenType token;
    std::string_view token_data;

    friend class Lexer;

};