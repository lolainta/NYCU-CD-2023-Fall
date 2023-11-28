#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name,
                         std::vector<AstNode *> *p_var_decls,
                         std::vector<AstNode *> *p_func_decls,
                         AstNode *p_body)
    : AstNode{line, col}, name(p_name)
{
    for (auto &decl : *p_var_decls)
    {
        var_decls.push_back(dynamic_cast<DeclNode *>(decl));
    }
    for (auto &decl : *p_func_decls)
    {
        func_decls.push_back(dynamic_cast<FunctionNode *>(decl));
    }
    body = dynamic_cast<CompoundStatementNode *>(p_body);
}

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::accept(AstNodeVisitor &p_visitor)
{
    p_visitor.visit(*this);
}

void ProgramNode::print()
{
    // TODO
    // outputIndentationSpace();
    // std::printf("program <line: %u, col: %u> %s %s\n",
    //             location.line, location.col,
    //             name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}

const char *ProgramNode::getNameCString() const
{
    return name.c_str();
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor)
{ // visitor pattern version
    for (auto &decl : var_decls)
    {
        decl->accept(p_visitor);
    }
    for (auto &decl : func_decls)
    {
        decl->accept(p_visitor);
    }
    body->accept(p_visitor);

    /* TODO
     *
     * for (auto &decl : var_decls) {
     *     decl->accept(p_visitor);
     * }
     *
     * // functions
     *
     * body->accept(p_visitor);
     */
}
