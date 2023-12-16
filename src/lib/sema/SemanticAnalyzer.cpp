#include "sema/SemanticAnalyzer.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
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
      sm.getSymbol(p_variable.getNameCString())->error = true;
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
  p_print.visitChildNodes(*this);
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
  auto sym = sm.getSymbol(p_variable_ref.getNameCString());
  if (sym == nullptr) {
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "use of undeclared symbol '%s'",
             p_variable_ref.getNameCString());
    printError(error_msg, p_variable_ref.getLocation().line,
               p_variable_ref.getLocation().col);
    error = true;
  } else if (sym->error) {
  } else if (sym->kind == "function" || sym->kind == "program") {
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "use of non-variable symbol '%s'",
             p_variable_ref.getNameCString());
    printError(error_msg, p_variable_ref.getLocation().line,
               p_variable_ref.getLocation().col);
    error = true;
  } else {
    auto &idxs = p_variable_ref.getIndices();
    for (auto &_idx : idxs) {
      auto idx = dynamic_cast<ConstantValueNode *>(_idx.get());
      if (strncmp(idx->getTypeSharedPtr()->getPTypeCString(), "integer", 7)) {
        char error_msg[128];
        snprintf(error_msg, sizeof(error_msg),
                 "index of array reference must be an integer");
        printError(error_msg, idx->getLocation().line, idx->getLocation().col);
        error = true;
        return;
      }
    }

    auto var_node = dynamic_cast<VariableNode *>(sym->node);
    auto dims = var_node->getTypeSharedPtr()->getDimensions();
    if (dims.size() < idxs.size()) {
      char error_msg[128];
      snprintf(error_msg, sizeof(error_msg),
               "there is an over array subscript on '%s'",
               p_variable_ref.getNameCString());
      printError(error_msg, p_variable_ref.getLocation().line,
                 p_variable_ref.getLocation().col);
      error = true;
      return;
    }
  }
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
