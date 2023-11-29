#include "AST/for.hpp"

ForNode::ForNode(const uint32_t line,
                 const uint32_t col,
                 AstNode *p_decl,
                 AstNode *p_assign,
                 AstNode *p_expr,
                 AstNode *p_stmt)
    : AstNode{line, col}
{
    decl = dynamic_cast<DeclNode *>(p_decl);
    assign = dynamic_cast<AssignmentNode *>(p_assign);
    expr = dynamic_cast<ExpressionNode *>(p_expr);
    stmt = dynamic_cast<CompoundStatementNode *>(p_stmt);
}

void ForNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    decl->accept(p_visitor);
    assign->accept(p_visitor);
    expr->accept(p_visitor);
    stmt->accept(p_visitor);
}