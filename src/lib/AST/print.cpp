#include "AST/print.hpp"

PrintNode::PrintNode(const uint32_t line, const uint32_t col,
                     AstNode *p_expr)
    : AstNode{line, col}
{
    expr = dynamic_cast<ExpressionNode *>(p_expr);
}

void PrintNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}
void PrintNode::print() {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    expr->accept(p_visitor);
}
