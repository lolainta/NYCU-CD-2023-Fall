#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <vector>

class AssignmentNode : public AstNode
{
public:
  AssignmentNode(const uint32_t line,
                 const uint32_t col,
                 AstNode *p_variable_ref,
                 AstNode *p_expression);
  ~AssignmentNode() = default;

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  VariableReferenceNode *variable_ref;
  ExpressionNode *expression;
};

#endif
