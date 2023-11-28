#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include <vector>

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "enums.hpp"

class DeclNode : public AstNode
{
public:
  // variable declaration
  DeclNode(const uint32_t line, const uint32_t col, std::vector<VariableNode *> *p_var_list);

  // constant variable declaration
  // DeclNode(const uint32_t, const uint32_t col
  //         /* TODO: identifiers, constant */);

  ~DeclNode() = default;
  void accept(AstNodeVisitor &p_visitor) override;
  void print() override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  // TODO: variables
  std::vector<VariableNode *> *var_list;
};

#endif
