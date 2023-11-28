#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             std::vector<AstNode *> *decls,
                                             std::vector<AstNode *> *stmts)
    : AstNode{line, col}
{
    for (auto &decl : *decls)
    {
        this->decls.push_back(dynamic_cast<DeclNode *>(decl));
    }
    for (auto &stmt : *stmts)
    {
        this->stmts.push_back(dynamic_cast<CompoundStatementNode *>(stmt));
    }
}

// TODO: You may use code snippets in AstDumper.cpp

void CompoundStatementNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void CompoundStatementNode::print() {}

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
