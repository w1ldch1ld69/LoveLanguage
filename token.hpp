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

    Comma, 

    Tab, Newline,
    
    Eof,

    Id, String,

    Number,

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
        {"}", TokenType::Rbracket}, {":", TokenType::Colon}, {",", TokenType::Comma},
        {"\t", TokenType::Tab}, {"\n", TokenType::Newline}, {"", TokenType::Eof}
    };

    inline static TokenList tokens{};

    static std::optional<TokenType> get_type(std::string str) {
        if (auto type = token_table.find(str); type != token_table.end()) return type->second;
        if (str[0] >= '0' && str[0] <= '9') return TokenType::Number;
        if (Service::Lexer::is_id_start(str[0])) return TokenType::Id;
        if (str[0] == '"' && str[str.length()] == '"') return TokenType::String;
        return std::nullopt;
    }

    ////////////////////////////////////////////////////

    Token(TokenType token, int start, int end) : token{token} {
        token_data = Service::Global::code.substr(start, end);
    }

    TokenType token;
    std::string_view token_data;

    ////////////////////////////////////////////////////

    friend class Lexer;

};