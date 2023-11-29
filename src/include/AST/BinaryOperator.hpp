#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "enums.hpp"

class BinaryOperatorNode : public ExpressionNode
{
public:
  BinaryOperatorNode(const uint32_t line,
                     const uint32_t col,
                     Operator op,
                     AstNode *p_left,
                     AstNode *p_right);
  ~BinaryOperatorNode() = default;

  Operator getOperator() const;

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  Operator op;
  ExpressionNode *left;
  ExpressionNode *right;
};

#endif
