#pragma once

#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace Service {
    
    namespace Global {
        std::string code;
    };

    namespace Lexer {

        ssize_t start = 0, cur = 0;
        bool is_newline = true;

        using Chars = std::array<char, 22>;
        using Strings = std::vector<std::string>;


        static Chars service_chars{
            '+', '-', '*', 
            '/', '%', '^', 
            '|', '&', '~', 
            '=', '!', '<', 
            '>', '(', ')', 
            '{', '}', ':', 
            ',', '\t', '\n', 
            '\0'
        };

        static Strings service_strs{
            {"+"}, {"-"}, {"*"}, 
            {"/"}, {"%"}, {"^"}, 
            {"|"}, {"&"}, {"~"}, 
            {"="}, {"!="}, {"<="}, 
            {">="}, {"<"}, {">"}, 
            {"("}, {")"}, {"{"},
            {"}"}, {"::"}, {":"}, 
            {","}, {"\t"}, {"\n"}, 
            {""}
        };

        bool is_service_char(char c) {
            for (auto elem : service_chars) {
                if (elem == c) return true;
            }
            return false;
        }

        std::size_t is_service_str(std::string str) {
            for (auto elem : service_strs) {
                if (std::size_t len = elem.length(); elem == str.substr(0, len)) return len;
            }
            return 0;
        }

        bool is_id_start(char c) {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
        }

        bool is_id_char(char c) {
            return is_id_start(c) || (c >= '0' && c <= '9');
        }

        bool is_digit(char c) {
            return (c >= '0' && c <= '9');
        }

        bool is_nonzero_digit(char c) {
            return (c >= '1' && c <= '9');
        }

        std::size_t is_number(std::string str) {
            if (auto iter = 0; str[iter] == '0') {
                if (++iter, !(is_digit(str[iter]) || is_id_start(str[iter]))) return iter;
                else if (str[iter] == '.') {
                    ++iter;
                    while (is_digit(str[iter])) ++iter;
                    return iter;
                } 
                return -iter;
            } 
            
            else if (is_nonzero_digit(str[iter])) {
                while (is_digit(str[iter])) ++iter;
                if (str[iter] == '.') {
                    do ++iter; while (is_digit(str[iter]));
                }

                else if (!is_id_start(str[iter]))
                    return iter;
                
                return -iter;
            }
                
            return 0;
        }

    };

    namespace Parser {
        
    };

};

