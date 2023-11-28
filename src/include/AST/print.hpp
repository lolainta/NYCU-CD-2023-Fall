#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class PrintNode : public AstNode
{
public:
  PrintNode(const uint32_t line,
            const uint32_t col,
            AstNode *p_expr);
  ~PrintNode() = default;

  void accept(AstNodeVisitor &p_visitor) override;
  void print() override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  ExpressionNode *expr;
};

#endif
