#include "AST/ast.hpp"

const Location &AstNode::getLocation() const { return location; }

AstNode::AstNode(const uint32_t line,
                 const uint32_t col)
    : location(line, col) {}

AstNode::~AstNode() {}
