#pragma once

#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <memory>

namespace Service {
    
    namespace Global {
        std::string code;
    };

    namespace Lexer {

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

    namespace AST {

        using param_list = std::vector<std::string_view>;

        enum class Type {
            Space, Fun, Var, Class,
            Body, Cond, While, Assign, 
            Logor, Logand, Bitor, 
            Bitxor, Bitand, Eqop, 
            Neqop, Lop, Gop,
            Leop, Geop, Add, Sub,
            Mul, Div, Lognot, Bitnot, Call, 
            Index, Access, Id, 
            Group, Num, Str,
            Bool, Pass,
        };



        struct Node {
            Type type;

            Node(Type _type) : type(_type) {}

            virtual ~Node() = default;
        };

        ////////////////////////////////////////////////////

        struct Decl : public Node {
            Decl(Type _type) : Node(_type) {}
        };

        struct Stmt : public Node {
            Stmt(Type _type) : Node(_type) {}
        };

        struct Expr : public Node {
            Expr(Type _type) : Node(_type) {}
        };

        ////////////////////////////////////////////////////

        using body_t = std::vector<std::unique_ptr<Stmt>>;

        using Expr_ptr = std::unique_ptr<Expr>;

        using cond_then_block =
            std::pair<Expr_ptr, body_t>;

        using name_t = std::string_view;

        ////////////////////////////////////////////////////

        // forward declarations

        struct VarDecl;
        struct FunDecl;
        struct ClassDecl;

        ////////////////////////////////////////////////////

        using decl_stmt =
            std::variant<
                std::unique_ptr<VarDecl>,
                std::unique_ptr<ClassDecl>
            >;

        using space =
            std::variant<
                std::unique_ptr<VarDecl>,
                std::unique_ptr<FunDecl>
            >;

        ////////////////////////////////////////////////////

        struct SpaceDecl : public Decl {
            name_t name;
            std::vector<space> body;
        
            SpaceDecl(
                Type _type,
                name_t _name,
                std::vector<space> _body
            )
                : Decl(_type),
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
                name_t _name,
                param_list _params,
                body_t _body
            )
                : Decl(_type),
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
                name_t _name,
                Expr_ptr _value
            )
                : Decl(_type),
                  name(_name),
                  value(std::move(_value)) {}
        };

        ////////////////////////////////////////////////////

        struct ClassDecl : public Decl {
            name_t name;
        
            std::vector<space> body;
        
            ClassDecl(
                Type _type,
                name_t _name,
                std::vector<space> _body
            )
                : Decl(_type),
                  name(_name),
                  body(std::move(_body)) {}
        };

        ////////////////////////////////////////////////////

        struct CondStmt : public Stmt {
            std::vector<cond_then_block> cond_blocks;
        
            CondStmt(
                Type _type,
                std::vector<cond_then_block> _cond_blocks
            )
                : Stmt(_type),
                  cond_blocks(std::move(_cond_blocks)) {}
        };

        ////////////////////////////////////////////////////

        struct WhileStmt : public Stmt {
            Expr_ptr condition;
        
            body_t body;
        
            WhileStmt(
                Type _type,
                Expr_ptr _condition,
                body_t _body
            )
                : Stmt(_type),
                  condition(std::move(_condition)),
                  body(std::move(_body)) {}
        };

        ////////////////////////////////////////////////////

        struct JumpStmt : public Stmt {
            JumpStmt(Type _type)
                : Stmt(_type) {}
        };

        ////////////////////////////////////////////////////

        struct ExprStmt : public Stmt {
            Expr_ptr expr;
        
            ExprStmt(
                Type _type,
                Expr_ptr _expr
            )
                : Stmt(_type),
                  expr(std::move(_expr)) {}
        };

        ////////////////////////////////////////////////////

        struct DeclStmt : public Stmt {
            decl_stmt decl;
        
            DeclStmt(
                Type _type,
                decl_stmt _decl
            )
                : Stmt(_type),
                  decl(std::move(_decl)) {}
        };

        ////////////////////////////////////////////////////

        struct RetStmt : public JumpStmt {
            Expr_ptr value;
        
            RetStmt(
                Type _type,
                Expr_ptr _value
            )
                : JumpStmt(_type),
                  value(std::move(_value)) {}
        };

        ////////////////////////////////////////////////////

        struct BreakStmt : public JumpStmt {
            BreakStmt(Type _type)
                : JumpStmt(_type) {}
        };

        ////////////////////////////////////////////////////

        struct ContStmt : public JumpStmt {
            ContStmt(Type _type)
                : JumpStmt(_type) {}
        };

        ////////////////////////////////////////////////////

        struct BinaryExpr : public Expr {
            Expr_ptr left;
            Expr_ptr right;
        
            BinaryExpr(
                Type _type,
                Expr_ptr _left,
                Expr_ptr _right
            )
                : Expr(_type),
                  left(std::move(_left)),
                  right(std::move(_right)) {}
        };

        ////////////////////////////////////////////////////

        struct PrefixExpr : public Expr {
            Expr_ptr arg;
        
            PrefixExpr(
                Type _type,
                Expr_ptr _arg
            )
                : Expr(_type),
                  arg(std::move(_arg)) {}
        };

        ////////////////////////////////////////////////////

        struct PostfixExpr : public Expr {
            Expr_ptr arg;
        
            PostfixExpr(
                Type _type,
                Expr_ptr _arg
            )
                : Expr(_type),
                  arg(std::move(_arg)) {}
        };

        ////////////////////////////////////////////////////

        struct IdExpr : public Expr {
            name_t name;
        
            IdExpr(
                Type _type,
                name_t _name
            )
                : Expr(_type),
                  name(_name) {}
        };

        ////////////////////////////////////////////////////

        struct NumberExpr : public Expr {
            std::variant<std::size_t, double> value;
        
            NumberExpr(
                Type _type,
                std::size_t _value
            )
                : Expr(_type),
                  value(_value) {}
        
            NumberExpr(
                Type _type,
                double _value
            )
                : Expr(_type),
                  value(_value) {}
        };

        ////////////////////////////////////////////////////

        struct StringExpr : public Expr {
            std::string value;
        
            StringExpr(
                Type _type,
                std::string _value
            )
                : Expr(_type),
                  value(std::move(_value)) {}
        };

        ////////////////////////////////////////////////////

        struct BoolExpr : public Expr {
            bool value;
        
            BoolExpr(
                Type _type,
                bool _value
            )
                : Expr(_type),
                  value(_value) {}
        };

        ////////////////////////////////////////////////////

        struct GroupExpr : public Expr {
            Expr_ptr expr;
        
            GroupExpr(
                Type _type,
                Expr_ptr _expr
            )
                : Expr(_type),
                  expr(std::move(_expr)) {}
        };

        ////////////////////////////////////////////////////

        struct PassExpr : public Expr {
            PassExpr(Type _type)
                : Expr(_type) {}
        };

    };

};

