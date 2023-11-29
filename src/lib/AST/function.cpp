#include "AST/function.hpp"

FunctionNode::FunctionNode(const uint32_t line,
                           const uint32_t col,
                           std::string *name,
                           std::vector<AstNode *> *var_decls,
                           PType *return_type)
    : AstNode{line, col}, name(*name), return_type(*return_type)
{
    for (auto &decl : *var_decls)
    {
        this->var_decls.push_back(dynamic_cast<DeclNode *>(decl));
    }
}

const char *FunctionNode::getNameCString() const { return name.c_str(); }
const PType &FunctionNode::getReturnType() const { return return_type; }
const std::vector<DeclNode *> &FunctionNode::getVarDecls() const { return var_decls; }
void FunctionNode::setBody(AstNode *body) { this->body = dynamic_cast<CompoundStatementNode *>(body); }

void FunctionNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor)
{
    for (auto &decl : var_decls)
    {
        decl->accept(p_visitor);
    }
    if (body != nullptr)
    {
        body->accept(p_visitor);
    }
}