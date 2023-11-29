#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "enums.hpp"

#include <vector>

class DeclNode : public AstNode
{
public:
  DeclNode(const uint32_t line,
           const uint32_t col,
           std::vector<AstNode *> *p_var_list);
  ~DeclNode() = default;

  std::vector<PType> getTypes();

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  std::vector<VariableNode *> *var_list;
};

#endif
