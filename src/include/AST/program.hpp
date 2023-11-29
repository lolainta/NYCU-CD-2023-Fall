#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"

#include <string>
#include <vector>

class ProgramNode final : public AstNode
{
public:
  ~ProgramNode() = default;
  ProgramNode(const uint32_t line,
              const uint32_t col,
              const char *const p_name,
              std::vector<AstNode *> *p_var_decls,
              std::vector<AstNode *> *p_func_decls,
              AstNode *p_body);

  const char *getNameCString() const;

  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  std::string name;
  std::vector<DeclNode *> var_decls;
  std::vector<FunctionNode *> func_decls;
  CompoundStatementNode *body;
};

#endif
