#include "AST/if.hpp"

IfNode::IfNode(const uint32_t line,
               const uint32_t col,
               AstNode *p_condition,
               AstNode *p_if_statement,
               AstNode *p_else_statement)
    : AstNode{line, col}
{
    condition = dynamic_cast<ExpressionNode *>(p_condition);
    if_statement = dynamic_cast<CompoundStatementNode *>(p_if_statement);
    else_statement = dynamic_cast<CompoundStatementNode *>(p_else_statement);
}

void IfNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    condition->accept(p_visitor);
    if_statement->accept(p_visitor);
    if (else_statement != nullptr)
        else_statement->accept(p_visitor);
}
