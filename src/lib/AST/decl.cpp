#include "AST/decl.hpp"
#include <cassert>

DeclNode::DeclNode(const uint32_t line,
                   const uint32_t col,
                   std::vector<VariableNode *> *p_var_list)
    : AstNode{line, col}, var_list(p_var_list) {}

std::vector<PType> DeclNode::getTypes()
{
    std::vector<PType> types;
    for (auto &id : *var_list)
    {
        types.push_back(id->getType());
    }
    return types;
}

void DeclNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

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
