#include "AST/UnaryOperator.hpp"

UnaryOperatorNode::UnaryOperatorNode(const uint32_t line,
                                     const uint32_t col,
                                     Operator op, AstNode *expr)
    : ExpressionNode{line, col}, op(op)
{
    this->expr = dynamic_cast<ExpressionNode *>(expr);
}
Operator UnaryOperatorNode::getOperator() const { return op; }

void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    expr->accept(p_visitor);
}