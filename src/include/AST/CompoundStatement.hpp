#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include <vector>
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"

class CompoundStatementNode : public AstNode
{
public:
  CompoundStatementNode(const uint32_t line, const uint32_t col,
                        std::vector<AstNode *> *decls,
                        std::vector<AstNode *> *stmts);
  ~CompoundStatementNode() = default;

  void accept(AstNodeVisitor &p_visitor) override;
  void print() override;
  void visitChildNodes(AstNodeVisitor &p_visitor);

private:
  std::vector<DeclNode *> decls;
  std::vector<AstNode *> stmts;
};

#endif
