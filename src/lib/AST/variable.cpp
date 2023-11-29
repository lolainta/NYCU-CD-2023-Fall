#include "AST/variable.hpp"

VariableNode::VariableNode(const uint32_t line,
                           const uint32_t col,
                           PType type,
                           const char *name)
    : AstNode{line, col}, type(type), name(name), const_val(nullptr){};
std::string VariableNode::getName() const { return this->name; }
const PType &VariableNode::getType() const { return this->type; }
void VariableNode::setType(const PType &type) { this->type = type; }
void VariableNode::setConstVal(ConstantValueNode *const_val) { this->const_val = const_val; }

void VariableNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    if (const_val != nullptr)
    {
        const_val->accept(p_visitor);
    }
}
