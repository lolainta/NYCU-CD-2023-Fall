#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ReadNode : public AstNode
{
public:
  ReadNode(const uint32_t line, const uint32_t col,
           AstNode *p_variable_ref);
  ~ReadNode() = default;

  void accept(AstNodeVisitor &p_visitor) override;
  void print() override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  VariableReferenceNode *variable_ref;
};

#endif
