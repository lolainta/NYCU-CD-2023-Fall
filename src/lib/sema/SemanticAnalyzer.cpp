#include "sema/SemanticAnalyzer.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "AST/PType.hpp"
#include "visitor/AstNodeInclude.hpp"

SemanticAnalyzer::SemanticAnalyzer(const std::string &p_filename)
    : filename(p_filename) {
  std::ifstream fin(p_filename.c_str(), std::ios::in);
  std::string line;
  while (getline(fin, line)) {
    lines.push_back(line);
  }
  fin.close();
}

void SemanticAnalyzer::printError(const std::string &msg, const uint32_t line,
                                  const uint32_t col) const {
  fprintf(stderr, "<Error> Found in line %d, column %d: %s\n", line, col,
          msg.c_str());
  fprintf(stderr, "    %s\n", lines[line - 1].c_str());
  for (uint32_t i = 0; i < 4 + col - 1; ++i) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^\n");
}

#define ADD_SYBMOL(name, kind, type, attr, node)                               \
  if (!sm.addSymbol(name, kind, type, attr, node)) {                           \
    char error_msg[128];                                                       \
    snprintf(error_msg, sizeof(error_msg), "symbol '%s' is redeclared", name); \
    printError(error_msg, (node)->getLocation().line,                          \
               (node)->getLocation().col);                                     \
    error = true;                                                              \
  }

void SemanticAnalyzer::visit(ProgramNode &p_program) {
  sm.pushScope();
  ADD_SYBMOL(p_program.getNameCString(), "program", "void", "", &p_program);
  p_program.visitChildNodes(*this);
  sm.dumpLastScope();
  sm.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
  for_each(p_decl.getVariables().begin(), p_decl.getVariables().end(),
           [&](auto &var_node) {
             if (var_node->isConstant()) {
               ADD_SYBMOL(var_node->getNameCString(), "constant",
                          var_node->getTypeCString(),
                          var_node->getConstantValueCString(), var_node.get());
             } else {
               ADD_SYBMOL(var_node->getNameCString(), "variable",
                          var_node->getTypeCString(), "", var_node.get());
             }
           });
  p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
  auto dims = p_variable.getTypeSharedPtr()->getDimensions();
  for (auto dim : dims) {
    if (dim == 0) {
      char error_msg[128];
      snprintf(
          error_msg, sizeof(error_msg),
          "'%s' declared as an array with an index that is not greater than 0",
          p_variable.getNameCString());
      printError(error_msg, p_variable.getLocation().line,
                 p_variable.getLocation().col);
      error = true;
    }
  }
  p_variable.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
  ADD_SYBMOL(p_function.getNameCString(), "function",
             p_function.getReturnTypeCString(),
             p_function.getParametersTypeCString(), &p_function);

  sm.pushScope();
  for_each(p_function.getParameters().begin(), p_function.getParameters().end(),
           [&](auto &param_node) {
             for_each(param_node->getVariables().begin(),
                      param_node->getVariables().end(), [&](auto &var_node) {
                        ADD_SYBMOL(var_node->getNameCString(), "parameter",
                                   var_node->getTypeCString(), "",
                                   var_node.get());
                      });
           });
  p_function.getBody()->visitChildNodes(*this);
  sm.dumpLastScope();
  sm.popScope();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
  sm.pushScope();
  p_compound_statement.visitChildNodes(*this);
  sm.dumpLastScope();
  sm.popScope();
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */

  p_variable_ref.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(IfNode &p_if) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}

void SemanticAnalyzer::visit(ForNode &p_for) {
  sm.pushScope();
  for_each(p_for.getLoopVarDecl()->getVariables().begin(),
           p_for.getLoopVarDecl()->getVariables().end(), [&](auto &var_node) {
             ADD_SYBMOL(var_node->getNameCString(), "loop_var",
                        var_node->getTypeCString(), "", var_node.get());
           });
  sm.pushScope();
  p_for.getBody()->visitChildNodes(*this);
  sm.dumpLastScope();
  sm.popScope();
  sm.dumpLastScope();
  sm.popScope();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
  /*
   * TODO:
   *
   * 1. Push a new symbol table if this node forms a scope.
   * 2. Insert the symbol into current symbol table if this node is related to
   *    declaration (ProgramNode, VariableNode, FunctionNode).
   * 3. Travere child nodes of this node.
   * 4. Perform semantic analyses of this node.
   * 5. Pop the symbol table pushed at the 1st step.
   */
}
