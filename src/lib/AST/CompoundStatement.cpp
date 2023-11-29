#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector<AstNode *> *decls,
                                             std::vector<AstNode *> *stmts)
    : AstNode{line, col}, stmts(*stmts)
{
    for (auto &decl : *decls)
    {
        this->decls.push_back(dynamic_cast<DeclNode *>(decl));
    }
}

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    for (auto &decl : decls)
    {
        decl->accept(p_visitor);
    }
    for (auto &stmt : stmts)
    {
        stmt->accept(p_visitor);
    }
}
