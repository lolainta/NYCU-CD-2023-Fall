#include "AST/BinaryOperator.hpp"

BinaryOperatorNode::BinaryOperatorNode(const uint32_t line,
                                       const uint32_t col,
                                       Operator op,
                                       AstNode *p_left,
                                       AstNode *p_right)

    : ExpressionNode{line, col}, op(op)
{
    this->left = dynamic_cast<ExpressionNode *>(p_left);
    this->right = dynamic_cast<ExpressionNode *>(p_right);
}

Operator BinaryOperatorNode::getOperator() const { return op; }

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    left->accept(p_visitor);
    right->accept(p_visitor);
}