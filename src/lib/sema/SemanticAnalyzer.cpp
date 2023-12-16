#include "sema/SemanticAnalyzer.hpp"

#include <algorithm>

#include "visitor/AstNodeInclude.hpp"
void SemanticAnalyzer::visit(ProgramNode &p_program) {
  sm.pushScope();
  sm.addSymbol(p_program.getNameCString(), "program", "void");
  p_program.visitChildNodes(*this);
  sm.dumpLastScope();
  sm.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
  for_each(p_decl.getVariables().begin(), p_decl.getVariables().end(),
           [&](auto &var_node) {
             if (var_node->isConstant()) {
               sm.addSymbol(var_node->getNameCString(), "constant",
                            var_node->getTypeCString(),
                            var_node->getConstantValueCString());
             } else {
               sm.addSymbol(var_node->getNameCString(), "variable",
                            var_node->getTypeCString());
             }
           });
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
  sm.addSymbol(p_function.getNameCString(), "function",
               p_function.getReturnTypeCString(),
               p_function.getParametersTypeCString());
  sm.pushScope();
  for_each(p_function.getParameters().begin(), p_function.getParameters().end(),
           [&](auto &param_node) {
             for_each(param_node->getVariables().begin(),
                      param_node->getVariables().end(), [&](auto &var_node) {
                        sm.addSymbol(var_node->getNameCString(), "parameter",
                                     var_node->getTypeCString());
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
             sm.addSymbol(var_node->getNameCString(), "loop_var",
                          var_node->getTypeCString());
           });
  sm.pushScope();
  p_for.getInitStmt()->visitChildNodes(*this);
  p_for.getEndCondition()->visitChildNodes(*this);
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
