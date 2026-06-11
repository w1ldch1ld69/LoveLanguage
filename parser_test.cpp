#include "lexer.hpp"
#include "parser.hpp"

std::string str1 = 
"space global:\n\n\tvar x = 1 + 4.18 * 7 ^ 8\n\tclass A:\n\t\tvar b\n\tfun main():\n\t\tif x == 3:\n\t\t\tx = x + 3\n\t\telif x == 2:\n\t\t\tx = x * 2\n\t\telse:\n\t\t\tx = 4\n\t\tret x\n";

class ASTPrinter {
private:
    static void indent(std::size_t level) {
        for (std::size_t i = 0; i < level; i++) {
            std::cout << "  ";
        }
    }

    static void print_expr(const Service::AST::Expr* expr, std::size_t level);
    static void print_stmt(const Service::AST::Stmt* stmt, std::size_t level);
    static void print_decl(const Service::AST::Decl* decl, std::size_t level);

    static const char* type_name(Service::AST::Type type) {
        using Type = Service::AST::Type;

        switch (type) {
            case Type::Space:         return "Space";
            case Type::Fun:           return "Fun";
            case Type::Var:           return "Var";
            case Type::Class:         return "Class";

            case Type::Body:          return "Body";
            case Type::Cond:          return "Cond";
            case Type::While:         return "While";
            case Type::Ret:           return "Ret";
            case Type::Break:         return "Break";
            case Type::Cont:          return "Cont";

            case Type::Expr:          return "Expr";
            case Type::Assign:        return "=";

            case Type::Logor:         return "or";
            case Type::Logand:        return "and";

            case Type::Bitor:         return "|";
            case Type::Bitxor:        return "^";
            case Type::Bitand:        return "&";

            case Type::Eqop:          return "==";
            case Type::Neqop:         return "!=";

            case Type::Lop:           return "<";
            case Type::Gop:           return ">";

            case Type::Leop:          return "<=";
            case Type::Geop:          return ">=";

            case Type::Add:           return "+";
            case Type::Sub:           return "-";

            case Type::Mul:           return "*";
            case Type::Div:           return "/";
            case Type::Mod:           return "%";

            case Type::Lognot:        return "not";
            case Type::Bitnot:        return "~";

            case Type::Call:          return "Call";
            case Type::Index:         return "Index";

            case Type::DotAccess:     return ".";
            case Type::DcolonAccess:  return "::";

            case Type::Id:            return "Id";
            case Type::Group:         return "Group";

            case Type::Num:           return "Number";
            case Type::Str:           return "String";
            case Type::Bool:          return "Bool";

            case Type::Pass:          return "Pass";
        }

        return "<unknown>";
    }

public:
    static void print() {
        for (const auto& decl : Service::AST::parser_tree) {
            print_decl(decl.get(), 0);
        }
    }
};

void ASTPrinter::print_expr(
    const Service::AST::Expr* expr,
    std::size_t level
) {
    using namespace Service::AST;

    if (!expr) {
        indent(level);
        std::cout << "<null expr>\n";
        return;
    }

    switch (expr->type) {

    case Type::Id: {
        auto e = type_name(expr->type);

        indent(level);
        std::cout << "Id(" << e << ")\n";
        break;
    }

    case Type::Num: {
        auto e = static_cast<const NumberExpr*>(expr);

        indent(level);

        std::cout << "Number(";

        std::visit(
            [](const auto& v) {
                std::cout << v;
            },
            e->value
        );

        std::cout << ")\n";
        break;
    }

    case Type::Str: {
        auto e = type_name(expr->type);

        indent(level);
        std::cout << "String(\"" << e << "\")\n";
        break;
    }

    case Type::Bool: {
        auto e = type_name(expr->type);

        indent(level);
        std::cout << "Bool("
                  << (e ? "true" : "false")
                  << ")\n";
        break;
    }

    case Type::Group: {
        auto e = static_cast<const GroupExpr*>(expr);

        indent(level);
        std::cout << "Group\n";

        print_expr(e->expr.get(), level + 1);
        break;
    }

    case Type::Call: {
        auto e = static_cast<const CallExpr*>(expr);

        indent(level);
        std::cout << "Call\n";

        indent(level + 1);
        std::cout << "callee:\n";

        print_expr(e->name.get(), level + 2);

        indent(level + 1);
        std::cout << "args:\n";

        for (auto const& arg : e->args) {
            print_expr(arg.get(), level + 2);
        }

        break;
    }

    case Type::Index: {
        auto e = static_cast<const IndexExpr*>(expr);

        indent(level);
        std::cout << "Index\n";

        print_expr(e->name.get(), level + 1);
        print_expr(e->index.get(), level + 1);

        break;
    }

    case Type::DotAccess:
    case Type::DcolonAccess: {
        auto e = static_cast<const AccessExpr*>(expr);

        indent(level);
        std::cout << "Access(" << e->access << ")\n";

        print_expr(e->name.get(), level + 1);

        break;
    }

    case Type::Lognot:
    case Type::Bitnot: {
        auto e = static_cast<const PrefixExpr*>(expr);

        indent(level);
        std::cout << "Prefix\n";

        print_expr(e->arg.get(), level + 1);

        break;
    }

    case Type::Assign:
    case Type::Logor:
    case Type::Logand:
    case Type::Bitor:
    case Type::Bitxor:
    case Type::Bitand:
    case Type::Eqop:
    case Type::Neqop:
    case Type::Lop:
    case Type::Gop:
    case Type::Leop:
    case Type::Geop:
    case Type::Add:
    case Type::Sub:
    case Type::Mul:
    case Type::Div:
    case Type::Mod: {
        auto e = static_cast<const BinaryExpr*>(expr);

        indent(level);
        std::cout << type_name(expr->type) << '\n';

        print_expr(e->left.get(), level + 1);
        print_expr(e->right.get(), level + 1);

        break;
    }

    case Type::Pass: {
        indent(level);
        std::cout << "Pass\n";
        break;
    }

    default:
        indent(level);
        std::cout << "<unknown expr>\n";
    }
}

void ASTPrinter::print_stmt(
    const Service::AST::Stmt* stmt,
    std::size_t level
) {
    using namespace Service::AST;

    switch (stmt->type) {

    case Type::Expr: {
        auto s = static_cast<const ExprStmt*>(stmt);

        indent(level);
        std::cout << "ExprStmt\n";

        print_expr(s->expr.get(), level + 1);

        break;
    }

    case Type::While: {
        auto s = static_cast<const WhileStmt*>(stmt);

        indent(level);
        std::cout << "While\n";

        indent(level + 1);
        std::cout << "condition:\n";

        print_expr(s->condition.get(), level + 2);

        indent(level + 1);
        std::cout << "body:\n";

        for (auto const& x : s->body) {
            print_stmt(x.get(), level + 2);
        }

        break;
    }

    case Type::Cond: {
        auto s = static_cast<const CondStmt*>(stmt);

        indent(level);
        std::cout << "If\n";

        for (auto const& [cond, body] : s->cond_blocks) {

            indent(level + 1);
            std::cout << "branch:\n";

            print_expr(cond.get(), level + 2);

            for (auto const& stmt : body) {
                print_stmt(stmt.get(), level + 2);
            }
        }

        break;
    }

    case Type::Ret: {
        auto s = static_cast<const RetStmt*>(stmt);

        indent(level);
        std::cout << "Return\n";

        if (s->value) {
            print_expr(s->value.get(), level + 1);
        }

        break;
    }

    case Type::Break:
        indent(level);
        std::cout << "Break\n";
        break;

    case Type::Cont:
        indent(level);
        std::cout << "Continue\n";
        break;

    default:
        indent(level);
        std::cout << "<stmt>\n";
    }
}

void ASTPrinter::print_decl(
    const Service::AST::Decl* decl,
    std::size_t level
) {
    using namespace Service::AST;

    switch (decl->type) {

    case Type::Var: {
        auto d = static_cast<const VarDecl*>(decl);

        indent(level);
        std::cout << "VarDecl(" << d->name << ")\n";

        if (d->value) {
            print_expr(d->value.get(), level + 1);
        }

        break;
    }

    case Type::Fun: {
        auto d = static_cast<const FunDecl*>(decl);

        indent(level);
        std::cout << "FunDecl(" << d->name << ")\n";

        for (auto const& stmt : d->body) {
            print_stmt(stmt.get(), level + 1);
        }

        break;
    }

    case Type::Class: {
        auto d = static_cast<const ClassDecl*>(decl);

        indent(level);
        std::cout << "ClassDecl(" << d->name << ")\n";

        for (auto const& decl : d->body) {
            print_decl(decl.get(), level + 1);
        }

        break;
    }

    case Type::Space: {
        auto d = static_cast<const SpaceDecl*>(decl);

        indent(level);
        std::cout << "SpaceDecl(" << d->name << ")\n";

        for (auto const& decl : d->body) {
            print_decl(decl.get(), level + 1);
        }

        break;
    }

    default:
        indent(level);
        std::cout << "<decl>\n";
    }
}

int main() {
    Service::Global::code = str1;
    Lexer::lex();
    Parser::parse();
    std::cout << std::endl;
    ASTPrinter::print();
}