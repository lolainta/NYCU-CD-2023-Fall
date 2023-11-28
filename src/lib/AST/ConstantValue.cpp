#include "AST/ConstantValue.hpp"
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     PType type,
                                     const char *str_val,
                                     int32_t int_val,
                                     double real_val,
                                     bool bool_val)
    : ExpressionNode{line, col}
{
    this->type = type;
    if (type.stype == SType::int_t)
    {
        value.int_val = int_val;
    }
    else if (type.stype == SType::real_t)
    {
        value.real_val = real_val;
    }
    else if (type.stype == SType::bool_t)
    {
        value.bool_val = bool_val;
    }
    else if (type.stype == SType::string_t)
    {
        value.str_val = strdup(str_val);
    }
    else
    {
        value.int_val = 0;
    }
}

#include <cstdio>
const PType ConstantValueNode::getType() const { return type; }
const ConstantValue ConstantValueNode::getValue() const { return value; }
// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void ConstantValueNode::print() {}
