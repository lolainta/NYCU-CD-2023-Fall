#include "AST/FunctionInvocation.hpp"

FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               std::string *p_name,
                                               std::vector<AstNode *> *p_expressions)
    : ExpressionNode{line, col}, name(*p_name)
{
    for (auto &expr : *p_expressions)
    {
        expressions.push_back(dynamic_cast<ExpressionNode *>(expr));
    }
}

const char *FunctionInvocationNode::getNameCString() const
{
    return name.c_str();
}

void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    for (auto &expr : expressions)
    {
        expr->accept(p_visitor);
    }
}