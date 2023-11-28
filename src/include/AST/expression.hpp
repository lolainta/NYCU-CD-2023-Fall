#ifndef __AST_EXPRESSION_NODE_H
#define __AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"
#include "enums.hpp"

class ExpressionNode : public AstNode
{
public:
  ExpressionNode(const uint32_t line, const uint32_t col);

  ~ExpressionNode() = default;

protected:
  PType type;
};

#endif
