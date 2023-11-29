#include "AST/decl.hpp"
#include <cassert>

DeclNode::DeclNode(const uint32_t line,
                   const uint32_t col,
                   std::vector<AstNode *> *p_var_list)
    : AstNode{line, col}
{
    var_list = new std::vector<VariableNode *>();
    for (auto &var : *p_var_list)
    {
        var_list->push_back(dynamic_cast<VariableNode *>(var));
    }
}

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
    for (auto &id : *var_list)
    {
        id->accept(p_visitor);
    }
}
