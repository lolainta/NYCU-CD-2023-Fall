#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode *> *p_var_list)
    : AstNode{line, col}
{
    this->var_list = p_var_list;
}

// TODO
// DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    // TODO
    for (auto &id : *var_list)
    {
        id->accept(p_visitor);
    }
    /* TODO
     *
     * for (auto &decl : var_decls) {
     *     decl->accept(p_visitor);
     * }
     *
     * // functions
     *
     * body->accept(p_visitor);
     */
}
