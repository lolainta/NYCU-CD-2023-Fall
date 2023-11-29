#include "AST/assignment.hpp"

AssignmentNode::AssignmentNode(const uint32_t line,
                               const uint32_t col,
                               AstNode *p_variable_ref,
                               AstNode *p_expression)
    : AstNode{line, col}
{
    variable_ref = dynamic_cast<VariableReferenceNode *>(p_variable_ref);
    expression = dynamic_cast<ExpressionNode *>(p_expression);
}

void AssignmentNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    variable_ref->accept(p_visitor);
    expression->accept(p_visitor);
}
