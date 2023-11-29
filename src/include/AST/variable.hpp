#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "enums.hpp"

#include <string>

class VariableNode : public AstNode
{
public:
  VariableNode(const uint32_t line,
               const uint32_t col,
               PType *type,
               std::string *name);
  ~VariableNode() = default;

  const std::string &getName() const;
  const PType &getType() const;
  void setType(const PType &type);
  void setConstVal(ConstantValueNode *const_val);

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  PType type;
  std::string name;
  ConstantValueNode *const_val;
};

#endif
