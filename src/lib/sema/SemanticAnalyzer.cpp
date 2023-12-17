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

void SemanticAnalyzer::printError(const std::string &msg,
                                  const Location &loc) const {
  fprintf(stderr, "<Error> Found in line %d, column %d: %s\n", loc.line,
          loc.col, msg.c_str());
  fprintf(stderr, "    %s\n", lines[loc.line - 1].c_str());
  for (uint32_t i = 0; i < 4 + loc.col - 1; ++i) {
    fprintf(stderr, " ");
  }
  fprintf(stderr, "^\n");
  this->error = true;
}

#define ADD_SYMBOL(name, kind, type, attr, node)                               \
  if (!sm.addSymbol(name, kind, type, attr, node)) {                           \
    char error_msg[128];                                                       \
    snprintf(error_msg, sizeof(error_msg), "symbol '%s' is redeclared", name); \
    printError(error_msg, (node)->getLocation());                              \
  }

void SemanticAnalyzer::visit(ProgramNode &p_program) {
  sm.pushScope();
  ADD_SYMBOL(p_program.getNameCString(), "program", "void", "", &p_program);
  p_program.visitChildNodes(*this);
  sm.dumpLastScope();
  sm.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
  for_each(p_decl.getVariables().begin(), p_decl.getVariables().end(),
           [&](auto &var_node) {
             if (var_node->isConstant()) {
               ADD_SYMBOL(var_node->getNameCString(), "constant",
                          var_node->getTypeCString(),
                          var_node->getConstantValueCString(), var_node.get());
             } else {
               ADD_SYMBOL(var_node->getNameCString(), "variable",
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
      printError(error_msg, p_variable.getLocation());
    }
  }
  p_variable.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
  p_constant_value.setType(
      std::make_shared<PType>(p_constant_value.getTypeSharedPtr()));
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
  ADD_SYMBOL(p_function.getNameCString(), "function",
             p_function.getReturnTypeCString(),
             p_function.getParametersTypeCString(), &p_function);

  sm.pushScope();
  for_each(p_function.getParameters().begin(), p_function.getParameters().end(),
           [&](auto &param_node) {
             for_each(param_node->getVariables().begin(),
                      param_node->getVariables().end(), [&](auto &var_node) {
                        ADD_SYMBOL(var_node->getNameCString(), "parameter",
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
  p_print.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
#define PRINT_ERROR(op, lhs_type, rhs_type)                                \
  char error_msg[128];                                                     \
  snprintf(error_msg, sizeof(error_msg),                                   \
           "invalid operands to binary operator '%s' ('%s' and '%s')", op, \
           lhs_type, rhs_type);                                            \
  printError(error_msg, p_bin_op.getLocation());                           \
  p_bin_op.setError();

  p_bin_op.visitChildNodes(*this);
  const auto &[lhs, rhs] = p_bin_op.getOperands();
  if (lhs->isError() or rhs->isError()) {
    p_bin_op.setError();
    return;
  }
  const PTypeSharedPtr &lhs_type = lhs->getTypeSharedPtr();
  const PTypeSharedPtr &rhs_type = rhs->getTypeSharedPtr();
  if (lhs_type == nullptr or rhs_type == nullptr) {
    p_bin_op.setError();
    return;
  }
  auto org_lhs_type = strdup(lhs_type.get()->getPTypeCString());
  auto org_rhs_type = strdup(rhs_type.get()->getPTypeCString());
  auto op = p_bin_op.getOp();
  if (lhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType and
      rhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType) {
    rhs_type->setPrimitiveType(PType::PrimitiveTypeEnum::kRealType);
  }
  if (lhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kIntegerType and
      rhs_type->getPrimitiveType() == PType::PrimitiveTypeEnum::kRealType) {
    lhs_type->setPrimitiveType(PType::PrimitiveTypeEnum::kRealType);
  }
  if (op == Operator::kPlusOp or op == Operator::kMinusOp or
      op == Operator::kMultiplyOp or op == Operator::kDivideOp) {
    if (strcmp(lhs_type->getPTypeCString(), rhs_type->getPTypeCString()) ||
        strcmp(lhs_type->getPTypeCString(), "integer") &&
            strcmp(lhs_type->getPTypeCString(), "real") &&
            (strcmp(lhs_type->getPTypeCString(), "string") ||
             op != Operator::kPlusOp)) {
      PRINT_ERROR(p_bin_op.getOpCString(), org_lhs_type, org_rhs_type);
    } else {
      p_bin_op.setType(lhs_type);
    }
  } else if (op == Operator::kModOp) {
    if (strcmp(lhs_type->getPTypeCString(), "integer") or
        strcmp(rhs_type->getPTypeCString(), "integer")) {
      PRINT_ERROR(p_bin_op.getOpCString(), org_lhs_type, org_rhs_type);
    } else {
      p_bin_op.setType(lhs_type);
    }
  } else if (op == Operator::kAndOp or op == Operator::kOrOp) {
    if (strcmp(lhs_type->getPTypeCString(), rhs_type->getPTypeCString()) or
        strcmp(lhs_type->getPTypeCString(), "boolean")) {
      PRINT_ERROR(p_bin_op.getOpCString(), org_lhs_type, org_rhs_type);
    } else {
      p_bin_op.setType(lhs_type);
    }
  } else if (op == Operator::kEqualOp or op == Operator::kNotEqualOp or
             op == Operator::kLessOp or op == Operator::kLessOrEqualOp or
             op == Operator::kGreaterOp or op == Operator::kGreaterOrEqualOp) {
    if (strcmp(lhs_type->getPTypeCString(), rhs_type->getPTypeCString()) or
        strcmp(lhs_type->getPTypeCString(), "integer") &&
            strcmp(lhs_type->getPTypeCString(), "real")) {
      PRINT_ERROR(p_bin_op.getOpCString(), org_lhs_type, org_rhs_type);
    } else {
      p_bin_op.setType(
          std::make_shared<PType>(PType::PrimitiveTypeEnum::kBoolType));
    }
  }
#undef PRINT_ERROR
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
  p_un_op.visitChildNodes(*this);
  auto operand = p_un_op.getOperand();
  if (operand->isError()) {
    p_un_op.setError();
    return;
  }
  auto op = p_un_op.getOp();
  if (op == Operator::kNegOp) {
    if (strcmp(operand->getTypeCString(), "integer") &&
        strcmp(operand->getTypeCString(), "real")) {
      char error_msg[128];
      snprintf(error_msg, sizeof(error_msg),
               "invalid operand to unary operator '%s' ('%s')",
               p_un_op.getOpCString(), operand->getTypeCString());
      printError(error_msg, p_un_op.getLocation());
      p_un_op.setError();
    } else {
      p_un_op.setType(operand->getTypeSharedPtr());
    }
  } else if (op == Operator::kNotOp) {
    if (strcmp(operand->getTypeCString(), "boolean")) {
      char error_msg[128];
      snprintf(error_msg, sizeof(error_msg),
               "invalid operand to unary operator '%s' ('%s')",
               p_un_op.getOpCString(), operand->getTypeCString());
      printError(error_msg, p_un_op.getLocation());
      p_un_op.setError();
    } else {
      p_un_op.setType(operand->getTypeSharedPtr());
    }
  } else {
    assert(false);
  }
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
  p_func_invocation.visitChildNodes(*this);
  auto sym = sm.getSymbol(p_func_invocation.getNameCString());
  if (sym == nullptr) {
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "use of undeclared symbol '%s'",
             p_func_invocation.getNameCString());
    printError(error_msg, p_func_invocation.getLocation());
    p_func_invocation.setError();
    return;
  } else if (sym->kind != "function") {
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "call of non-function symbol '%s'",
             p_func_invocation.getNameCString());
    printError(error_msg, p_func_invocation.getLocation());
    p_func_invocation.setError();
    return;
  } else {
    auto node = dynamic_cast<FunctionNode *>(sym->node);
    p_func_invocation.setType(
        std::make_shared<PType>(node->getReturnType()->getPrimitiveType()));
    auto &args = p_func_invocation.getArgs();
    auto &param_groups = node->getParameters();
    std::vector<VariableNode *> params;
    for_each(param_groups.begin(), param_groups.end(), [&](auto &param_group) {
      for_each(param_group->getVariables().begin(),
               param_group->getVariables().end(),
               [&](auto &param) { params.push_back(param.get()); });
    });
    if (args.size() != params.size()) {
      char error_msg[128];
      // snprintf(error_msg, sizeof(error_msg),
      //          "too %s arguments to function '%s' ('%d' expected, '%d'
      //          given)", args.size() > params.size() ? "many" : "few",
      //          p_func_invocation.getNameCString(), params.size(),
      //          args.size());
      snprintf(error_msg, sizeof(error_msg),
               "too %s arguments provided for function '%s'", "few/much",
               p_func_invocation.getNameCString());
      printError(error_msg, p_func_invocation.getLocation());
      p_func_invocation.setError();
      return;
    } else {
      for (int i = 0; i < args.size(); i++) {
        auto arg = args[i].get();
        auto param = params[i];
        if (arg->isError()) {
          p_func_invocation.setError();
          return;
        }
        if (strcmp(arg->getTypeCString(), param->getTypeCString())) {
          char error_msg[128];
          snprintf(error_msg, sizeof(error_msg),
                   "incompatible type passing '%s' to parameter of type '%s'",
                   arg->getTypeCString(), param->getTypeCString());
          printError(error_msg, arg->getLocation());
          p_func_invocation.setError();
          return;
        }
      }
    }
  }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
  p_variable_ref.visitChildNodes(*this);
  auto sym = sm.getSymbol(p_variable_ref.getNameCString());
  if (sym == nullptr) {
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "use of undeclared symbol '%s'",
             p_variable_ref.getNameCString());
    printError(error_msg, p_variable_ref.getLocation());
    p_variable_ref.setError();
    return;
  } else if (sym->error) {
  } else if (sym->kind == "function" || sym->kind == "program") {
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "use of non-variable symbol '%s'",
             p_variable_ref.getNameCString());
    printError(error_msg, p_variable_ref.getLocation());
    p_variable_ref.setError();
    return;
  } else {
    auto &idxs = p_variable_ref.getIndices();
    for (auto &_idx : idxs) {
      auto idx = dynamic_cast<ConstantValueNode *>(_idx.get());
      if (strncmp(idx->getTypeSharedPtr()->getPTypeCString(), "integer", 7)) {
        char error_msg[128];
        snprintf(error_msg, sizeof(error_msg),
                 "index of array reference must be an integer");
        printError(error_msg, idx->getLocation());
        p_variable_ref.setError();
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
      printError(error_msg, p_variable_ref.getLocation());
      p_variable_ref.setError();
      return;
    }
  }
  auto &idxs = p_variable_ref.getIndices();
  auto var_node = dynamic_cast<VariableNode *>(sym->node);
  auto dims = var_node->getTypeSharedPtr()->getDimensions();
  p_variable_ref.setType(std::make_shared<PType>(
      var_node->getTypeSharedPtr().get()->getPrimitiveType(),
      *(new std::vector<uint64_t>(dims.begin() + idxs.size(), dims.end()))));
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
             ADD_SYMBOL(var_node->getNameCString(), "loop_var",
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
