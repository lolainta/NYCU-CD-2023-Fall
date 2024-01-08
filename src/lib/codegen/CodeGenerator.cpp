#include "codegen/CodeGenerator.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>

#include "visitor/AstNodeInclude.hpp"

std::string genRandString(const size_t len) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  std::string s(len, ' ');
  for (size_t i = 0; i < len; ++i) {
    s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  return s;
}

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
  constexpr const char *const comment =
      "    # declare var \"%s\", level: %ld\n";
  dumpInstructions(m_output_file.get(), comment, p_variable.getNameCString(),
                   var->getLevel());
  if (var->getLevel() == 0) {
    constexpr const char *const comment = "    # declare global var \"%s\"\n";
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
      constexpr const char *const comment = "    # declare local var \"%s\"\n";
      dumpInstructions(m_output_file.get(), comment,
                       p_variable.getNameCString());
      if (p_variable.isFunctionParam()) {
        constexpr const char *const pop_args =
            "    lw t0, %d(s0)\n"
            "    sw t0, %d(s0)\n";
        dumpInstructions(m_output_file.get(), pop_args, var->getParamIdx() * 4,
                         -var->getOffset());
      }
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
  constexpr const char *const comment = "    # declare function %s\n";
  dumpInstructions(m_output_file.get(), comment, p_function.getNameCString());
  constexpr const char *const functino_decl =
      ".section    .text\n"
      "    .globl %s\n"
      "    .type %s, @function\n"
      "%s:\n";

  dumpInstructions(m_output_file.get(), functino_decl,
                   p_function.getNameCString(), p_function.getNameCString(),
                   p_function.getNameCString());

  constexpr const char *const function_prologue =
      "    # function prologue\n"
      "    addi sp, sp, -128\n"
      "    sw ra, 124(sp)\n"
      "    sw s0, 120(sp)\n"
      "    addi s0, sp, 128\n";
  dumpInstructions(m_output_file.get(), function_prologue,
                   p_function.getNameCString(), p_function.getNameCString(),
                   p_function.getNameCString());

  for_each(p_function.getParameters().begin(), p_function.getParameters().end(),
           [&](auto &decl) {
             for_each(decl->getVariables().begin(), decl->getVariables().end(),
                      [&](auto &var) { var->setFunctionParam(); });
           });
  p_function.visitChildNodes(*this);

  constexpr const char *const function_epilogue =
      "    # function epilogue\n"
      "    lw ra, 124(sp)\n"
      "    lw s0, 120(sp)\n"
      "    addi sp, sp, 128\n"
      "    jr ra\n"
      "    .size %s, .-%s\n";
  dumpInstructions(m_output_file.get(), function_epilogue,
                   p_function.getNameCString(), p_function.getNameCString());

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
        "    addi sp, sp, 4\n"
        "    jal ra, printInt\n";
    dumpInstructions(m_output_file.get(), print_integer);
  } else if (expr_type_kind == PType::PrimitiveTypeEnum::kBoolType) {
    constexpr const char *const print_boolean =
        "    # print boolean\n"
        "    lw a0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    jal ra, printInt\n";
    dumpInstructions(m_output_file.get(), print_boolean);
  } else if (expr_type_kind == PType::PrimitiveTypeEnum::kStringType) {
    // constexpr const char *const print_string =
    //     "    # print string\n"
    //     "    mv a0, t0\n"
    //     "    li a7, 4\n"
    //     "    ecall\n";
    // dumpInstructions(m_output_file.get(), print_string);
  } else {
    assert(false && "Invalid type");
  }
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
  p_bin_op.visitChildNodes(*this);
  if (p_bin_op.getOp() == Operator::kPlusOp) {
    constexpr const char *const comment = "    # add\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const add =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    add t2, t0, t1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), add);
  } else if (p_bin_op.getOp() == Operator::kMinusOp) {
    constexpr const char *const comment = "    # sub\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const sub =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    sub t2, t1, t0\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), sub);
  } else if (p_bin_op.getOp() == Operator::kMultiplyOp) {
    constexpr const char *const comment = "    # mul\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const mul =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    mul t2, t0, t1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), mul);
  } else if (p_bin_op.getOp() == Operator::kDivideOp) {
    constexpr const char *const comment = "    # div\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const div =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    div t2, t1, t0\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), div);
  } else if (p_bin_op.getOp() == Operator::kLessOp) {
    constexpr const char *const comment = "    # less\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const less =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    slt t2, t1, t0\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), less);
  } else if (p_bin_op.getOp() == Operator::kLessOrEqualOp) {
    constexpr const char *const comment = "    # less or equal\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const less_or_equal =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    slt t2, t0, t1\n"
        "    xori t2, t2, 1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), less_or_equal);
  } else if (p_bin_op.getOp() == Operator::kGreaterOp) {
    constexpr const char *const comment = "    # greater\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const greater =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    slt t2, t0, t1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), greater);
  } else if (p_bin_op.getOp() == Operator::kGreaterOrEqualOp) {
    constexpr const char *const comment = "    # greater or equal\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const greater_or_equal =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    slt t2, t1, t0\n"
        "    xori t2, t2, 1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), greater_or_equal);
  } else if (p_bin_op.getOp() == Operator::kEqualOp) {
    constexpr const char *const comment = "    # equal\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const equal =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    xor t2, t0, t1\n"
        "    sltiu t2, t2, 1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), equal);
  } else if (p_bin_op.getOp() == Operator::kNotEqualOp) {
    constexpr const char *const comment = "    # not equal\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const not_equal =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    snez t2, t0, t1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), not_equal);
  } else if (p_bin_op.getOp() == Operator::kAndOp) {
    constexpr const char *const comment = "    # and\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const and_op =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    and t2, t0, t1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), and_op);
  } else if (p_bin_op.getOp() == Operator::kOrOp) {
    constexpr const char *const comment = "    # or\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const or_op =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    or t2, t0, t1\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), or_op);
  } else if (p_bin_op.getOp() == Operator::kModOp) {
    constexpr const char *const comment = "    # mod\n";
    dumpInstructions(m_output_file.get(), comment);
    constexpr const char *const mod_op =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    rem t2, t1, t0\n"
        "    addi sp, sp, -4\n"
        "    sw t2, 0(sp)\n";
    dumpInstructions(m_output_file.get(), mod_op);
  } else {
    printf("Invalid operator: %s\n", p_bin_op.getOpCString());
    assert(false && "Invalid operator");
  }
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
  p_un_op.visitChildNodes(*this);
  constexpr const char *const comment = "    # unary op\n";
  dumpInstructions(m_output_file.get(), comment);
  if (p_un_op.getOp() == Operator::kNegOp) {
    constexpr const char *const neg_op =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    neg t1, t0\n"
        "    addi sp, sp, -4\n"
        "    sw t1, 0(sp)\n";
    dumpInstructions(m_output_file.get(), neg_op);
  } else if (p_un_op.getOp() == Operator::kNotOp) {
    constexpr const char *const not_op =
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    seqz t1, t0\n"
        "    addi sp, sp, -4\n"
        "    sw t1, 0(sp)\n";
    dumpInstructions(m_output_file.get(), not_op);
  } else {
    printf("Invalid operator: %s\n", p_un_op.getOpCString());
    assert(false && "Invalid operator");
  }
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
  auto func = m_symbol_manager_ptr->lookup(p_func_invocation.getName());
  p_func_invocation.visitChildNodes(*this);
  constexpr const char *const comment = "    # call function %s\n";
  dumpInstructions(m_output_file.get(), comment,
                   p_func_invocation.getNameCString());
  // // move parameters to a0-a7
  // constexpr const char *const push_parameters =
  //     "    lw t0, 0(sp)\n"
  //     "    addi sp, sp, 4\n"
  //     "    mv a%d, t0\n";
  // for (size_t i = 0; i < p_func_invocation.getArguments().size(); ++i) {
  //   dumpInstructions(m_output_file.get(), push_parameters, i);
  // }
  constexpr const char *const call_function =
      "    jal ra, %s\n"
      "    mv t0, a0\n"
      "    addi sp, sp, -4\n"
      "    addi sp, sp, %d\n"
      "    sw t0, 0(sp)\n";
  dumpInstructions(m_output_file.get(), call_function,
                   p_func_invocation.getNameCString(),
                   p_func_invocation.getArguments().size() * 4);
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
  auto var = m_symbol_manager_ptr->lookup(p_variable_ref.getName());
  if (var->getLevel() == 0) {
    constexpr const char *const comment = "    # push global varref \"%s\"\n";
    dumpInstructions(m_output_file.get(), comment,
                     p_variable_ref.getNameCString());

    // constexpr const char *const global_variable;
    if (p_variable_ref.isLvalue()) {
      constexpr const char *const global_variable =
          "    la t0, %s\n"
          "    addi sp, sp, -4\n"
          "    sw t0, 0(sp)\n";
      dumpInstructions(m_output_file.get(), global_variable,
                       p_variable_ref.getNameCString());
    } else {
      constexpr const char *const global_variable =
          "    la t0, %s\n"
          "    lw t0, 0(t0)\n"
          "    addi sp, sp, -4\n"
          "    sw t0, 0(sp)\n";
      dumpInstructions(m_output_file.get(), global_variable,
                       p_variable_ref.getNameCString());
    }
  } else {
    constexpr const char *const comment = "    # push local varref \"%s\"\n";
    dumpInstructions(m_output_file.get(), comment,
                     p_variable_ref.getNameCString());
    if (p_variable_ref.isLvalue()) {
      constexpr const char *const local_variable =
          "    addi t0, s0, -%d\n"
          "    addi sp, sp, -4\n"
          "    sw t0, 0(sp)\n";
      dumpInstructions(m_output_file.get(), local_variable, var->getOffset());
    } else {
      constexpr const char *const local_variable =
          "    addi t0, s0, -%d\n"
          "    lw t0, 0(t0)\n"
          "    addi sp, sp, -4\n"
          "    sw t0, 0(sp)\n";
      dumpInstructions(m_output_file.get(), local_variable, var->getOffset());
    }
  }
  // p_variable_ref.accept(*this);
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
  p_assignment.getLvalue().setLvalue();
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

void CodeGenerator::visit(IfNode &p_if) {
  constexpr const char *const comment = "    # ifStatement\n";
  dumpInstructions(m_output_file.get(), comment);
  p_if.getCondition().accept(*this);
  auto label = genRandString(10);
  constexpr const char *const if_prologue =
      "    lw t0, 0(sp)\n"
      "    addi sp, sp, 4\n"
      "    beqz t0, %s_else\n";
  dumpInstructions(m_output_file.get(), if_prologue, label.c_str());
  p_if.getBody().accept(*this);
  constexpr const char *const if_epilogue =
      "    j %s_if_end\n"
      "%s_else:\n";
  dumpInstructions(m_output_file.get(), if_epilogue, label.c_str(),
                   label.c_str());
  if (p_if.getElseBody()) {
    p_if.getElseBody()->accept(*this);
  }
  constexpr const char *const if_end = "%s_if_end:\n";
  dumpInstructions(m_output_file.get(), if_end, label.c_str());
}

void CodeGenerator::visit(WhileNode &p_while) {
  constexpr const char *const comment = "    # whileStatement\n";
  dumpInstructions(m_output_file.get(), comment);
  auto label = genRandString(10);
  constexpr const char *const while_prologue = "%s_while_begin:\n";
  dumpInstructions(m_output_file.get(), while_prologue, label.c_str());
  p_while.getCondition().accept(*this);
  constexpr const char *const while_body_prologue =
      "    lw t0, 0(sp)\n"
      "    addi sp, sp, 4\n"
      "    beqz t0, %s_while_end\n";
  dumpInstructions(m_output_file.get(), while_body_prologue, label.c_str());
  p_while.getBody().accept(*this);
  constexpr const char *const while_epilogue =
      "    j %s_while_begin\n"
      "%s_while_end:\n";
  dumpInstructions(m_output_file.get(), while_epilogue, label.c_str(),
                   label.c_str());
}

void CodeGenerator::visit(ForNode &p_for) {
  // Reconstruct the hash table for looking up the symbol entry
  m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
      p_for.getSymbolTable());
  constexpr const char *const comment = "    # forStatement\n";
  dumpInstructions(m_output_file.get(), comment);
  // unrolling the loop
  for (int i = p_for.getLowerBound().getConstantPtr()->integer();
       i < p_for.getUpperBound().getConstantPtr()->integer(); i++) {
    auto label = genRandString(10);
    constexpr const char *const assign_loop_var =
        "    li t0, %d\n"
        "    sw t0, -%d(s0)\n";
    dumpInstructions(
        m_output_file.get(), assign_loop_var, i,
        m_symbol_manager_ptr
            ->lookup(p_for.getLoopVarDecl().getVariables().front()->getName())
            ->getOffset());
    p_for.getBody().accept(*this);
  }
  // Remove the entries in the hash table
  m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {
  p_return.visitChildNodes(*this);
  constexpr const char *const comment = "    # return\n";
  dumpInstructions(m_output_file.get(), comment);
  constexpr const char *const return_val =
      "    lw a0, 0(sp)\n"
      "    addi sp, sp, 4\n";
  dumpInstructions(m_output_file.get(), return_val);
}
