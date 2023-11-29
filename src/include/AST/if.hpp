#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class IfNode : public AstNode
{
public:
  IfNode(const uint32_t line,
         const uint32_t col,
         AstNode *p_expression,
         AstNode *p_compound_statement1,
         AstNode *p_compound_statement2);
  ~IfNode() = default;

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  ExpressionNode *condition;
  CompoundStatementNode *if_statement;
  CompoundStatementNode *else_statement;
};

#endif
