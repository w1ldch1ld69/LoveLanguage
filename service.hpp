#pragma once

#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

namespace Service {
    
    namespace Global {
        std::string code;
    };

    namespace Lexer {

        ssize_t start = 0, cur = 0;

        using Chars = std::array<char, 21>;
        using Strings = std::vector<std::string>;


        static Chars service_chars{
            '+', '-', '*', 
            '/', '%', '^', 
            '|', '&', '~', 
            '=', '!', '<', 
            '>', '(', ')', 
            '{', '}', ':', 
            '\t', '\n', '\0'
        };

        static Strings service_strs{
            {"+"}, {"-"}, {"*"}, 
            {"/"}, {"%"}, {"^"}, 
            {"|"}, {"&"}, {"~"}, 
            {"="}, {"!="}, {"<="}, 
            {">="}, {"<"}, {">"}, 
            {"("}, {")"}, {"{"},
            {"}"}, {":"}, {"\t"},
            {"\n"}, {""}
        };

        bool is_service_char(char c) {
            for (auto elem : service_chars) {
                if (elem == c) return true;
            }
            return false;
        }

        std::size_t is_service_str(std::string_view str) {
            for (auto elem : service_strs) {
                if (std::size_t len = elem.length(); elem == str.substr(0, len)) return len;
            }
            return 0;
        }

    };

};

