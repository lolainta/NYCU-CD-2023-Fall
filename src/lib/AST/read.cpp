#include "AST/read.hpp"

ReadNode::ReadNode(const uint32_t line,
                   const uint32_t col,
                   AstNode *p_variable_ref)
    : AstNode{line, col}
{
    variable_ref = dynamic_cast<VariableReferenceNode *>(p_variable_ref);
}

void ReadNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    variable_ref->accept(p_visitor);
}
