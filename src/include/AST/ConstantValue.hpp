#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "enums.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <cstring>

class ConstantValueNode : public ExpressionNode
{
public:
  ConstantValueNode(const uint32_t line,
                    const uint32_t col,
                    PType type,
                    const char *str_val,
                    int32_t int_val,
                    double real_val,
                    bool bool_val);
  ~ConstantValueNode() = default;

  const PType getType() const;
  const ConstantValue getValue() const;

  void accept(AstNodeVisitor &p_visitor) override;

private:
  ConstantValue value;
};

#endif
