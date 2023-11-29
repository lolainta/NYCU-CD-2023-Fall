#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "enums.hpp"

class UnaryOperatorNode : public ExpressionNode
{
public:
  UnaryOperatorNode(const uint32_t line, const uint32_t col, Operator op, AstNode *expr);
  ~UnaryOperatorNode() = default;

  Operator getOperator() const;
  void accept(AstNodeVisitor &p_visitor) override;
  void print() override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  Operator op;
  ExpressionNode *expr;
};

#endif
