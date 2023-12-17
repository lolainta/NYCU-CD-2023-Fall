#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include <cassert>

#include "AST/PType.hpp"
#include "AST/ast.hpp"
class ExpressionNode : public AstNode {
 public:
  ~ExpressionNode() = default;
  ExpressionNode(const uint32_t line, const uint32_t col)
      : AstNode{line, col} {}
  const PTypeSharedPtr &getTypeSharedPtr() const { return m_type; }
  const char *getTypeCString() const {
    return m_type ? m_type->getPTypeCString() : "unknown";
  }
  void setType(const PTypeSharedPtr &p_type) {
    // assert(m_type == nullptr);
    m_type = p_type;
  }
  bool isError() const { return error; }
  void setError() { error = true; }

 protected:
  // for carrying type of result of an expression
  // TODO: for next assignment
  PTypeSharedPtr m_type;
  bool error = false;
};

#endif
