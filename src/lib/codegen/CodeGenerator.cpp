#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path)
    : m_source_file_path(source_file_name) {
    // FIXME: assume that the source file is always xxxx.p
    const auto &real_path =
        save_path.empty() ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind("/");
    auto dot_pos = source_file_name.rfind(".");

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    auto output_file_path{
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S"};
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n"
        ".section    .text\n"
        "    .align 2\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Hint: Maintain a table to lookup symbols that are visible in the current scope, 
    // TODO: Entering scope (Add symbols in the current scope to the 'visible table')

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    // TODO: Leaving scope (Remove symbols added by the current scope from the 'visible table')
}

void CodeGenerator::visit(DeclNode &p_decl) {}

void CodeGenerator::visit(VariableNode &p_variable) {}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {}

void CodeGenerator::visit(FunctionNode &p_function) {
    // Hint: Maintain a table to lookup symbols that are visible in the current scope, 
    // TODO: Entering scope (Add symbols in the current scope to the 'visible table')

    p_function.visitChildNodes(*this);

    // TODO: Leaving scope (Remove symbols added by the current scope from the 'visible table')
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Hint: Maintain a table to lookup symbols that are visible in the current scope, 
    // TODO: Entering scope (Add symbols in the current scope to the 'visible table')

    p_compound_statement.visitChildNodes(*this);

    // TODO: Leaving scope (Remove symbols added by the current scope from the 'visible table')
}

void CodeGenerator::visit(PrintNode &p_print) {}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {}

void CodeGenerator::visit(AssignmentNode &p_assignment) {}

void CodeGenerator::visit(ReadNode &p_read) {}

void CodeGenerator::visit(IfNode &p_if) {}

void CodeGenerator::visit(WhileNode &p_while) {}

void CodeGenerator::visit(ForNode &p_for) {
    // Hint: Maintain a table to lookup symbols that are visible in the current scope, 
    // TODO: Entering scope (Add symbols in the current scope to the 'visible table')

    p_for.visitChildNodes(*this);

    // TODO: Leaving scope (Remove symbols added by the current scope from the 'visible table')
}

void CodeGenerator::visit(ReturnNode &p_return) {}
