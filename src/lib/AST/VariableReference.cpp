#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char *p_name,
                                             std::vector<AstNode *> *p_expressions)
    : ExpressionNode{line, col}, name(p_name)
{
    for (auto &expression : *p_expressions)
    {
        expressions.push_back(dynamic_cast<ExpressionNode *>(expression));
    }
}
const char *VariableReferenceNode::getNameCString() const { return name.c_str(); }

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    for (auto &expression : expressions)
    {
        expression->accept(p_visitor);
    }
}
