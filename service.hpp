#pragma once

#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <variant>
#include <memory>

namespace Service {
    
    namespace Global {
        std::string code;
    };

    namespace Lexer {

        bool is_newline = true;

        using Chars = std::array<char, 23>;
        using Strings = std::vector<std::string>;


        static Chars service_chars{
            '+', '-', '*', 
            '/', '%', '^', 
            '|', '&', '~', 
            '=', '!', '<', 
            '>', '(', ')', 
            '{', '}', ':', 
            '.', ',', '\t', 
            '\n', '\0'
        };

        static Strings service_strs{
            {"+"}, {"-"}, {"*"}, 
            {"/"}, {"%"}, {"^"}, 
            {"|"}, {"&"}, {"~"}, 
            {"=="}, {"="}, {"!="}, 
            {"<="}, {">="}, {"<"}, 
            {">"}, {"("}, {")"}, 
            {"{"}, {"}"}, {"::"}, 
            {":"}, {"."}, {","}, 
            {"\t"}, {"\n"}, {""}
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

        ssize_t is_number(std::string str) {
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
                    return iter;
                }

                else if (!is_id_start(str[iter]))
                    return iter;
                
                return -iter;
            }
                
            return 0;
        }

    };

    namespace AST {

        using param_list = std::vector<std::string_view>;

        enum class Type {
            Space, Fun, Var, Class,
            Body, Cond, While, Ret, 
            Break, Cont, Expr, Assign, 
            Logor, Logand, Bitor, 
            Bitxor, Bitand, Eqop, 
            Neqop, Lop, Gop,
            Leop, Geop, Add, Sub,
            Mul, Div, Mod, 
            Lognot, Bitnot, Call, 
            Index, DotAccess, DcolonAccess, 
            Id, Group, Num, 
            Str, Bool, Pass,
        };

        using Line = std::size_t;


        struct Node {
            Type type;
            Line line;

            Node(Type _type, Line _line) : type(_type), line(_line) {}

            virtual ~Node() = default;
        };

        ////////////////////////////////////////////////////

        struct Decl : public Node {
            Decl(Type _type, Line _line) : Node(_type, _line) {}
        };

        struct Stmt : public Node {
            Stmt(Type _type, Line _line) : Node(_type, _line) {}
        };

        struct Expr : public Node {
            Expr(Type _type, Line _line) : Node(_type, _line) {}
        };

        ////////////////////////////////////////////////////

        using body_t = std::vector<std::unique_ptr<Stmt>>;

        using Node_ptr = std::unique_ptr<Node>;

        using Expr_ptr = std::unique_ptr<Expr>;

        using Stmt_ptr = std::unique_ptr<Stmt>;

        using Decl_ptr = std::unique_ptr<Decl>;

        using cond_then_block =
            std::pair<Expr_ptr, body_t>;

        using name_t = std::string_view;

        ////////////////////////////////////////////////////

        // forward declarations

        struct VarDecl;
        struct FunDecl;
        struct ClassDecl;

        ////////////////////////////////////////////////////

        ////////////////////////////////////////////////////

        using decl_stmt =
            std::variant<
                std::unique_ptr<VarDecl>,
                std::unique_ptr<ClassDecl>
            >;

        ////////////////////////////////////////////////////

        struct SpaceDecl : public Decl {
            name_t name;
            std::vector<Decl_ptr> body;
        
            SpaceDecl(
                Type _type, 
                Line _line,
                name_t _name,
                std::vector<Decl_ptr> _body
            )
                : Decl(_type, _line),
                  name(_name),
                  body(std::move(_body)) {}
        };

        ////////////////////////////////////////////////////

        struct FunDecl : public Decl {
            name_t name;
        
            param_list params;
        
            body_t body;
        
            FunDecl(
                Type _type, 
                Line _line,
                name_t _name,
                param_list _params,
                body_t _body
            )
                : Decl(_type, _line),
                  name(_name),
                  params(std::move(_params)),
                  body(std::move(_body)) {}
        };

        ////////////////////////////////////////////////////

        struct VarDecl : public Decl {
            name_t name;
        
            Expr_ptr value;
        
            VarDecl(
                Type _type, 
                Line _line,
                name_t _name,
                Expr_ptr _value
            )
                : Decl(_type, _line),
                  name(_name),
                  value(std::move(_value)) {}
        };

        ////////////////////////////////////////////////////

        struct ClassDecl : public Decl {
            name_t name;
        
            std::vector<Decl_ptr> body;
        
            ClassDecl(
                Type _type, 
                Line _line,
                name_t _name,
                std::vector<Decl_ptr> _body
            )
                : Decl(_type, _line),
                  name(_name),
                  body(std::move(_body)) {}
        };

        ////////////////////////////////////////////////////

        struct CondStmt : public Stmt {
            std::vector<cond_then_block> cond_blocks;
        
            CondStmt(
                Type _type, 
                Line _line,
                std::vector<cond_then_block> _cond_blocks
            )
                : Stmt(_type, _line),
                  cond_blocks(std::move(_cond_blocks)) {}
        };

        ////////////////////////////////////////////////////

        struct WhileStmt : public Stmt {
            Expr_ptr condition;
        
            body_t body;
        
            WhileStmt(
                Type _type, 
                Line _line,
                Expr_ptr _condition,
                body_t _body
            )
                : Stmt(_type, _line),
                  condition(std::move(_condition)),
                  body(std::move(_body)) {}
        };

        ////////////////////////////////////////////////////

        struct JumpStmt : public Stmt {
            JumpStmt(Type _type, Line _line)
                : Stmt(_type, _line) {}
        };

        ////////////////////////////////////////////////////

        struct ExprStmt : public Stmt {
            Expr_ptr expr;
        
            ExprStmt(
                Type _type, 
                Line _line,
                Expr_ptr _expr
            )
                : Stmt(_type, _line),
                  expr(std::move(_expr)) {}
        };

        ////////////////////////////////////////////////////

        struct DeclStmt : public Stmt {
            decl_stmt decl;
        
            DeclStmt(
                Type _type, 
                Line _line,
                decl_stmt _decl
            )
                : Stmt(_type, _line),
                  decl(std::move(_decl)) {}
        };

        ////////////////////////////////////////////////////

        struct RetStmt : public JumpStmt {
            Expr_ptr value;
        
            RetStmt(
                Type _type, 
                Line _line,
                Expr_ptr _value
            )
                : JumpStmt(_type, _line),
                  value(std::move(_value)) {}
        };

        ////////////////////////////////////////////////////

        struct BreakStmt : public JumpStmt {
            BreakStmt(Type _type, Line _line)
                : JumpStmt(_type, _line) {}
        };

        ////////////////////////////////////////////////////

        struct ContStmt : public JumpStmt {
            ContStmt(Type _type, Line _line)
                : JumpStmt(_type, _line) {}
        };

        ////////////////////////////////////////////////////

        struct BinaryExpr : public Expr {
            Expr_ptr left;
            Expr_ptr right;
        
            BinaryExpr(
                Type _type, 
                Line _line,
                Expr_ptr _left,
                Expr_ptr _right
            )
                : Expr(_type, _line),
                  left(std::move(_left)),
                  right(std::move(_right)) {}
        };

        ////////////////////////////////////////////////////

        struct PrefixExpr : public Expr {
            Expr_ptr arg;
        
            PrefixExpr(
                Type _type, 
                Line _line,
                Expr_ptr _arg
            )
                : Expr(_type, _line),
                  arg(std::move(_arg)) {}
        };

        ////////////////////////////////////////////////////

        struct CallExpr : public Expr {
            Expr_ptr name;
            std::vector<Expr_ptr> args;  

            CallExpr(
                Type _type, 
                Line _line,
                Expr_ptr _name,
                std::vector<Expr_ptr> _args
            )
                : Expr(_type, _line),
                  name(std::move(_name)),
                  args(std::move(_args)) {}
        };

        ////////////////////////////////////////////////////

        struct IndexExpr : public Expr {
            Expr_ptr name;
            Expr_ptr index;

            IndexExpr(
                Type _type, 
                Line _line,
                Expr_ptr _name,
                Expr_ptr _index
            ) 
                : Expr(_type, _line),
                  name(std::move(_name)),
                  index(std::move(_index)) {}
        };

        ////////////////////////////////////////////////////

        struct AccessExpr : public Expr {
            Expr_ptr name;
            std::string_view access;

            AccessExpr(
                Type _type, 
                Line _line,
                Expr_ptr _name,
                std::string_view _access
            ) 
                : Expr(_type, _line),
                  name(std::move(_name)),
                  access(_access) {}
        };

        ////////////////////////////////////////////////////

        struct IdExpr : public Expr {
            name_t name;
        
            IdExpr(
                Type _type, 
                Line _line,
                name_t _name
            )
                : Expr(_type, _line),
                  name(_name) {}
        };

        ////////////////////////////////////////////////////

        struct NumberExpr : public Expr {
            std::variant<std::size_t, double> value;
        
            NumberExpr(
                Type _type, 
                Line _line,
                std::variant<std::size_t, double> _value
            )
                : Expr(_type, _line),
                  value(_value) {}
        
        };

        ////////////////////////////////////////////////////

        struct StringExpr : public Expr {
            std::string_view value;
        
            StringExpr(
                Type _type, 
                Line _line,
                std::string_view _value
            )
                : Expr(_type, _line),
                  value(std::move(_value)) {}
        };

        ////////////////////////////////////////////////////

        struct BoolExpr : public Expr {
            bool value;
        
            BoolExpr(
                Type _type, 
                Line _line,
                bool _value
            )
                : Expr(_type, _line),
                  value(_value) {}
        };

        ////////////////////////////////////////////////////

        struct GroupExpr : public Expr {
            Expr_ptr expr;
        
            GroupExpr(
                Type _type, 
                Line _line,
                Expr_ptr _expr
            )
                : Expr(_type, _line),
                  expr(std::move(_expr)) {}
        };

        ////////////////////////////////////////////////////

        struct PassExpr : public Expr {
            PassExpr(Type _type, Line _line)
                : Expr(_type, _line) {}
        };

        inline static std::vector<Service::AST::Decl_ptr> parser_tree;

    };

    namespace Symbols {

        enum class Kind {
            Variable,
            Function,
            Class,
            Space
        };

        struct Symbol {
            Kind kind;
            std::string_view name;

            Symbol(
                Kind _kind,
                std::string_view _name
            ) :
                kind(_kind),
                name(_name)
            {}

        };

        using Scope = std::unordered_map<Symbol, Symbol>;

        using ProgSpace = std::vector<Scope>;

        inline static ProgSpace symbols_table; 

        // bool contains(std::string_view str) {
            // if (symbol_table.contains(str)) return true;
            // else return false;
        // }

        // bool push
    };

    namespace Analyzer {

    };

};

