#include "service.hpp"

class Analyzer {
public:
    inline static std::size_t iter = 0;
    inline static std::size_t cur_scope = 0;

    static void analyze() {
        for (std::size_t n = Service::AST::parser_tree.size(); iter < n; ++iter) {
            analyze_decl();
        }
    }
private:
    static void analyze_decl(Service::AST::Decl*);
    static void analyze_stmt(Service::AST::Stmt*);
    static void analyze_expr(Service::AST::Expr*);
};

void Analyzer::analyze_decl(Service::AST::Decl *decl) {
    // switch (Service::AST::parser_tree[iter]->type) {
        // case Service::AST::Type::Space:
            // Service::Symbols::Scope new_scope = Service::Symbols::Scope(dynamic_cast<Service::AST::SpaceDecl*>(decl)->name, Service::Symbols::Kind::Space, );
            // Service::Symbols::symbols_table.push_back();
        // case Service::AST::Type::Fun:
        // 
        // case Service::AST::Type::Var:
// 
        // case Service::AST::Type::Class:
// 
    // }
}
