#ifndef AST_VARIABLE_REFERENCE_NODE_H
#define AST_VARIABLE_REFERENCE_NODE_H

#include <memory>
#include <string>
#include <vector>

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class VariableReferenceNode final : public ExpressionNode {
 public:
  using ExprNodes = std::vector<std::unique_ptr<ExpressionNode>>;

 private:
  std::string m_name;
  ExprNodes m_indices;
  bool m_lvalue = false;

 public:
  ~VariableReferenceNode() = default;

  // normal reference
  VariableReferenceNode(const uint32_t line, const uint32_t col,
                        const char *const p_name)
      : ExpressionNode{line, col}, m_name(p_name) {}

  // array reference
  VariableReferenceNode(const uint32_t line, const uint32_t col,
                        const char *const p_name, ExprNodes &p_indices)
      : ExpressionNode{line, col},
        m_name(p_name),
        m_indices(std::move(p_indices)) {}

  const std::string &getName() const { return m_name; }
  const char *getNameCString() const { return m_name.c_str(); }

  const ExprNodes &getIndices() const { return m_indices; }

  bool isLvalue() const { return m_lvalue; }
  void setLvalue() { m_lvalue = true; }

  void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
  void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
