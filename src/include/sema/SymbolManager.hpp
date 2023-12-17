#pragma once

#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "AST/ast.hpp"
class SymbolEntry {
  friend class SymbolTable;
  friend class SymbolManager;
  friend class SemanticAnalyzer;

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
  bool error = false;
};

class SymbolTable {
  friend class SymbolManager;

 public:
  SymbolTable() = default;
  bool addSymbol(SymbolEntry *entry);
  void dump();
  SymbolEntry *getSymbol(const std::string &name) {
    for (auto it = entries.rbegin(); it != entries.rend(); it++) {
      if ((*it)->name == name) {
        return *it;
      }
    }
    return nullptr;
  }

 private:
  std::vector<SymbolEntry *> entries;
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
  SymbolEntry *getSymbol(const std::string &name) {
    for (auto it = tables.rbegin(); it != tables.rend(); it++) {
      auto symbol = (*it)->getSymbol(name);
      if (symbol != nullptr) {
        return symbol;
      }
    }
    return nullptr;
  }
  SymbolEntry *getContext() {
    if (contextStack.empty()) {
      return nullptr;
    }
    return contextStack.top();
  }
  void popContext();
  void setVerbose(bool verbose) { this->verbose = verbose; }

 private:
  bool verbose;
  std::vector<SymbolTable *> tables;
  std::stack<SymbolEntry *> contextStack;
};
