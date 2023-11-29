#include "AST/while.hpp"

WhileNode::WhileNode(const uint32_t line,
                     const uint32_t col,
                     AstNode *p_condition,
                     AstNode *p_body)
    : AstNode{line, col}
{
    condition = dynamic_cast<ExpressionNode *>(p_condition);
    body = dynamic_cast<CompoundStatementNode *>(p_body);
}

void WhileNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    condition->accept(p_visitor);
    body->accept(p_visitor);
}