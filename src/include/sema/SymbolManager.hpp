#pragma once

#include <stack>
#include <string>
#include <vector>

class SymbolEntry {
  friend class SymbolTable;

 public:
  SymbolEntry(std::string name, std::string kind, int level, std::string type)
      : name(name), kind(kind), level(level), type(type), attribute("") {}
  SymbolEntry(std::string name, std::string kind, int level, std::string type,
              std::string attribute)
      : name(name),
        kind(kind),
        level(level),
        type(type),
        attribute(attribute) {}

 private:
  std::string name;
  std::string kind;
  int level;
  std::string type;
  std::string attribute;
};

class SymbolTable {
 public:
  SymbolTable() = default;
  void addSymbol(SymbolEntry entry);
  void dump();

 private:
  std::vector<SymbolEntry> entries;
};

class SymbolManager {
 public:
  void pushScope();
  void popScope();
  void addSymbol(const std::string &, const std::string &, const std::string &,
                 const std::string &);
  void addSymbol(const std::string &, const std::string &, const std::string &);
  void dumpLastScope();

 private:
  std::stack<SymbolTable *> tables;
};
