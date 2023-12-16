#pragma once

#include <stack>
#include <string>
#include <vector>

#include "AST/ast.hpp"
class SymbolEntry {
  friend class SymbolTable;

 public:
  SymbolEntry(const std::string &name, const std::string &kind, int level,
              const std::string &type, AstNode *node)
      : name(name),
        kind(kind),
        level(level),
        type(type),
        attribute(""),
        node(node) {}
  SymbolEntry(const std::string &name, const std::string &kind, int level,
              const std::string &type, const std::string &attribute,
              AstNode *node)
      : name(name),
        kind(kind),
        level(level),
        type(type),
        attribute(attribute),
        node(node) {}

 private:
  std::string name;
  std::string kind;
  int level;
  std::string type;
  std::string attribute;
  AstNode *node;
};

class SymbolTable {
  friend class SymbolManager;

 public:
  SymbolTable() = default;
  bool addSymbol(SymbolEntry entry);
  void dump();
  bool semanticCheck();

 private:
  std::vector<SymbolEntry> entries;
};

class SymbolManager {
 public:
  void pushScope();
  void popScope();
  bool addSymbol(const std::string &, const std::string &, const std::string &,
                 const std::string &, AstNode *);
  bool addSymbol(const std::string &, const std::string &, const std::string &,
                 AstNode *);
  void dumpLastScope();
  void semanticCheck();

 private:
  std::stack<SymbolTable *> tables;
  std::vector<std::pair<std::string, int>> loop_vars;
};
