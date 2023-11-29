#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/assignment.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ForNode : public AstNode
{
public:
  ForNode(const uint32_t line, const uint32_t col,
          AstNode *p_decl,
          AstNode *p_assign,
          AstNode *p_expr,
          AstNode *p_stmt);
  ~ForNode() = default;
  void accept(AstNodeVisitor &p_visitor) override;
  void print() override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  // TODO: declaration, assignment, expression, compound statement
  DeclNode *decl;
  AssignmentNode *assign;
  ExpressionNode *expr;
  CompoundStatementNode *stmt;
};

#endif
