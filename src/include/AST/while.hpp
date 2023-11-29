#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class WhileNode : public AstNode
{
public:
  WhileNode(const uint32_t line,
            const uint32_t col,
            AstNode *p_expression,
            AstNode *p_compound_statement);
  ~WhileNode() = default;

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  AstNode *condition;
  AstNode *body;
};

#endif
