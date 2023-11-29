#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include "enums.hpp"

#include <string>
#include <vector>

class FunctionNode : public AstNode
{
public:
  FunctionNode(const uint32_t line,
               const uint32_t col,
               std::string *name,
               std::vector<AstNode *> *var_decls,
               PType *return_type);
  ~FunctionNode() = default;

  const char *getNameCString() const;
  const PType &getReturnType() const;
  const std::vector<DeclNode *> &getVarDecls() const;
  void setBody(AstNode *body);

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  std::string name;
  PType return_type;
  std::vector<DeclNode *> var_decls;
  CompoundStatementNode *body;
};

#endif
