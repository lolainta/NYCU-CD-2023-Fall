#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       AstNode *p_expression)
    : AstNode{line, col}
{
    expression = dynamic_cast<ExpressionNode *>(p_expression);
}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void ReturnNode::print() {}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    expression->accept(p_visitor);
}
