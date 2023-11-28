#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char *p_name,
                                               std::vector<AstNode *> *p_expressions)
    : ExpressionNode{line, col}
{
    name = std::string(p_name);
    for (auto &expr : *p_expressions)
    {
        expressions.push_back(dynamic_cast<ExpressionNode *>(expr));
    }
}

const char *FunctionInvocationNode::getNameCString() const
{
    return name.c_str();
}
// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}
void FunctionInvocationNode::print() {}

// void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }
