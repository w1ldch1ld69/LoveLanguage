#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

#include "service.hpp"
#include "token.hpp"

namespace AST {

    enum class Type {
        Space, Fun, Var, Class,
        Cond, While, Assign, Logor,
        Logand, Bitor, Bitxor, Bitand,
        Eqop, Neqop, Lop, Gop,
        Leop, Geop, Add, Sub,
        Mul, Div, Lognot, Bitnot,
        Call, Index, Access, 
        Id, Group, Num, Str,
        Bool, Pass,
    };

    class Node {
    public:
        
    
    private:
        std::string_view data;
        Type type;
    };

    class SpaceNode : public Node {
    public:

    private:
        std::vector<Node*> body;
    };

    class FunNode : public Node {
    public:

    private:
        std::vector<Node*> params;
        Node *body;
    };

    class VarNode : public Node {
    public:

    private:
        Node *value;
    };

    class ClassNode : public Node {
    public:

    private:
        std::vector<Node*> fields;
        std::vector<Node*> methods;
    };

    class CondNode : public Node {
    public:

    private:
        std::vector<std::pair<Node*, Node*>> cond_blocks;
    };

    class WhileNode : public Node {
        Node *condition;
        Node *body;
    };

    class Binary : public Node {
        Node *left, *right;
    };
    
    class Prefix : public Node {
        Node *arg;
    };

    class Postfix : public Node {
        Node *arg;
    };

    class Id : public Node {
        
    };

    class Number : public Node {
        std::variant<std::size_t, double> value;
    };

    class String : public Node {

    };

    class Bool : public Node {
        bool value;
    };

    class Group : public Node {
        Node *expr;
    };

    class Pass : public Node {
        
    };

};