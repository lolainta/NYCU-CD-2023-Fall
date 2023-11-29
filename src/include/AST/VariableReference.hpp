#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>

class VariableReferenceNode : public ExpressionNode
{
public:
  VariableReferenceNode(const uint32_t line, const uint32_t col,
                        const char *p_name,
                        std::vector<AstNode *> *p_expressions);
  ~VariableReferenceNode() = default;

  const char *getNameCString() const;

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  std::string name;
  std::vector<ExpressionNode *> expressions;
};

#endif
