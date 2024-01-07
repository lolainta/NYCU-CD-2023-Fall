#include "codegen/CodeGenerator.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>

#include "visitor/AstNodeInclude.hpp"

CodeGenerator::CodeGenerator(const std::string &source_file_name,
                             const std::string &save_path,
                             const SymbolManager *const p_symbol_manager)
    : m_symbol_manager_ptr(p_symbol_manager),
      m_source_file_path(source_file_name) {
  // FIXME: assume that the source file is always xxxx.p
  const auto &real_path = save_path.empty() ? std::string{"."} : save_path;
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
  setvbuf(m_output_file.get(), nullptr, _IONBF, 0);
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
      "    .option nopic\n";
      // ".section    .text\n"
      // "    .align 2\n";
  // clang-format on
  dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                   m_source_file_path.c_str());

  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_program.getSymbolTable());
  // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
  this->m_is_global_scope = true;
  auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
  for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
           visit_ast_node);
  for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
           visit_ast_node);
  this->m_is_global_scope = false;

  constexpr const char *const main_prologue =
      ".section    .text\n"
      "    .globl main\n"
      "    .type main, @function\n"
      "main:\n"
      "    # main prologue\n"
      "    addi sp, sp, -128\n"
      "    sw ra, 124(sp)\n"
      "    sw s0, 120(sp)\n"
      "    addi s0, sp, 128\n";
  dumpInstructions(m_output_file.get(), main_prologue);

  const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

  constexpr const char *const main_epilogue =
      "    # main epilogue\n"
      "    lw ra, 124(sp)\n"
      "    lw s0, 120(sp)\n"
      "    addi sp, sp, 128\n"
      "    jr ra\n";
  dumpInstructions(m_output_file.get(), main_epilogue);

  constexpr const char *const riscv_assembly_file_epilogue =
      ".section    .note.GNU-stack,\"\",@progbits\n";
  dumpInstructions(m_output_file.get(), riscv_assembly_file_epilogue);

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) { p_decl.visitChildNodes(*this); }

void CodeGenerator::visit(VariableNode &p_variable) {
  auto var = m_symbol_manager_ptr->lookup(p_variable.getName());
  if (var->getLevel() == 0) {
    constexpr const char *const comment = "    # declare global var %s\n";
    dumpInstructions(m_output_file.get(), comment, p_variable.getNameCString());
    if (p_variable.getConstantPtr()) {
      constexpr const char *const global_constant =
          ".section    .rodata\n"
          "    .align 2\n"
          "    .globl %s\n"
          "    .type %s, @object\n"
          "%s:\n"
          "    .word %s\n";
      dumpInstructions(m_output_file.get(), global_constant,
                       p_variable.getNameCString(), p_variable.getNameCString(),
                       p_variable.getNameCString(),
                       p_variable.getConstantPtr()->getConstantValueCString());
    } else {
      constexpr const char *const global_variable = ".comm %s, 4, 4\n";
      dumpInstructions(m_output_file.get(), global_variable,
                       p_variable.getNameCString());
    }
  } else {
    if (p_variable.getConstantPtr()) {
      constexpr const char *const comment = "    # declare local const %s\n";
      dumpInstructions(m_output_file.get(), comment,
                       p_variable.getNameCString());
      constexpr const char *const local_constant =
          "    li t0, %s\n"
          "    sw t0, -%d(s0)\n";
      dumpInstructions(m_output_file.get(), local_constant,
                       p_variable.getConstantPtr()->getConstantValueCString(),
                       var->getOffset());
    } else {
      constexpr const char *const comment = "    # declare local var %s\n";
      // dumpInstructions(m_output_file.get(), comment,
      //                  p_variable.getNameCString());
      // constexpr const char *const local_variable =
      //     "    addi sp, sp, -4\n"
      //     "    sw s0, 0(sp)\n";
      // dumpInstructions(m_output_file.get(), local_variable);
    }
  }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
  constexpr const char *const comment = "    # push constant value %s\n";
  dumpInstructions(m_output_file.get(), comment,
                   p_constant_value.getConstantValueCString());
  constexpr const char *const constant_value =
      "    li t0, %s\n"
      "    addi sp, sp, -4\n"
      "    sw t0, 0(sp)\n";
  dumpInstructions(m_output_file.get(), constant_value,
                   p_constant_value.getConstantValueCString());
}

void CodeGenerator::visit(FunctionNode &p_function) {
  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_function.getSymbolTable());

  p_function.visitChildNodes(*this);

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_function.getSymbolTable());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_compound_statement.getSymbolTable());

  p_compound_statement.visitChildNodes(*this);

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(
      p_compound_statement.getSymbolTable());
}

void CodeGenerator::visit(PrintNode &p_print) {
  p_print.visitChildNodes(*this);
  const auto &expr = p_print.getTarget();
  const auto &expr_type = expr.getInferredType();
  const auto &expr_type_kind = expr_type->getPrimitiveType();
  const auto &expr_type_size = expr_type->getByteSize();

  if (expr_type_kind == PType::PrimitiveTypeEnum::kIntegerType) {
    constexpr const char *const comment = "    # print %s\n";
    dumpInstructions(m_output_file.get(), comment,
                     expr_type->getPTypeCString());
    constexpr const char *const print_integer =
        "    lw a0, 0(sp)\n"
        "    lw a0, 0(a0)\n"
        "    addi sp, sp, 4\n"
        "    jal ra, printInt\n";
    dumpInstructions(m_output_file.get(), print_integer);
  }
  // else if (expr_type_kind == PType::PrimitiveTypeEnum::kBoolean) {
  //   constexpr const char *const print_boolean =
  //       "    # print boolean\n"
  //       "    mv a0, t0\n"
  //       "    li a7, 1\n"
  //       "    ecall\n";
  //   dumpInstructions(m_output_file.get(), print_boolean);
  // } else if (expr_type_kind == PType::PrimitiveTypeEnum::kString) {
  //   constexpr const char *const print_string =
  //       "    # print string\n"
  //       "    mv a0, t0\n"
  //       "    li a7, 4\n"
  //       "    ecall\n";
  //   dumpInstructions(m_output_file.get(), print_string);
  // } else {
  //   assert(false && "Invalid type");
  // }

  // constexpr const char *const print_epilogue =
  //     "    # print epilogue\n"
  //     "    li a0, 0\n"
  //     "    li a7, 0\n"
  //     "    ecall\n";
  // dumpInstructions(m_output_file.get(), print_epilogue);
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
  p_bin_op.visitChildNodes(*this);
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
  p_un_op.visitChildNodes(*this);
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
  printf("visit variable reference node\n");
  auto var = m_symbol_manager_ptr->lookup(p_variable_ref.getName());
  if (var->getLevel() == 0) {
    constexpr const char *const comment = "    # push global varref %s\n";
    dumpInstructions(m_output_file.get(), comment,
                     p_variable_ref.getNameCString());
    constexpr const char *const global_variable =
        "    la t0, %s\n"
        "    addi sp, sp, -4\n"
        "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), global_variable,
                     p_variable_ref.getNameCString());
  } else {
    constexpr const char *const comment = "    # push local varref %s\n";
    dumpInstructions(m_output_file.get(), comment,
                     p_variable_ref.getNameCString());
    constexpr const char *const local_variable =
        "    addi t0, s0, -%d\n"
        "    addi sp, sp, -4\n"
        "    sw t0, 0(sp)\n";
    dumpInstructions(m_output_file.get(), local_variable, var->getOffset());
  }
  // p_variable_ref.accept(*this);
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
  p_assignment.visitChildNodes(*this);
  constexpr const char *const comment = "    # assign %s\n";
  dumpInstructions(m_output_file.get(), comment,
                   p_assignment.getLvalue().getNameCString());
  constexpr const char *const assign =
      "    lw t0, 0(sp)\n"
      "    addi sp, sp, 4\n"
      "    lw t1, 0(sp)\n"
      "    addi sp, sp, 4\n"
      "    sw t0, 0(t1)\n";
  dumpInstructions(m_output_file.get(), assign);
}

void CodeGenerator::visit(ReadNode &p_read) {}

void CodeGenerator::visit(IfNode &p_if) {}

void CodeGenerator::visit(WhileNode &p_while) {}

void CodeGenerator::visit(ForNode &p_for) {
  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_for.getSymbolTable());

  p_for.visitChildNodes(*this);

  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {}
