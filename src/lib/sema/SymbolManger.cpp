#include <algorithm>
#include <cassert>
#include <cstdio>
#include <typeinfo>

#include "sema/SymbolManager.hpp"
void dumpDemarcation(const char chr, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    printf("%c", chr);
  }
  puts("");
}

// void dumpSymbol(void) {
//   dumpDemarcation('=');
//   printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
//          "Attribute");
//   dumpDemarcation('-');
//   {
//     printf("%-33s", "func");
//     printf("%-11s", "function");
//     printf("%d%-10s", 0, "(global)");
//     printf("%-17s", "boolean");
//     printf("%-11s", "integer, real [2][3]");
//     puts("");
//   }
//   dumpDemarcation('-');
// }

bool SymbolTable::addSymbol(SymbolEntry entry) {
  if (std::find_if(entries.begin(), entries.end(),
                   [&entry](const SymbolEntry &a) {
                     return a.name == entry.name && a.level == entry.level;
                   }) != entries.end()) {
    return false;
  }
  entries.push_back(entry);
  return true;
}

void SymbolTable::dump() {
  dumpDemarcation('=', 110);
  printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
         "Attribute");
  dumpDemarcation('-', 110);
  for (auto entry : entries) {
    printf("%-33s", entry.name.c_str());
    printf("%-11s", entry.kind.c_str());
    printf("%d%-10s", entry.level, (entry.level == 0) ? "(global)" : "(local)");
    printf("%-17s", entry.type.c_str());
    printf("%-11s", entry.attribute.c_str());
    puts("");
  }
  dumpDemarcation('-', 110);
}

bool SymbolTable::semanticCheck() {
  bool is_valid = true;
  // std::vector<std::string> names;
  // for (auto entry : entries) {
  //   if (std::find(names.begin(), names.end(), entry.name) != names.end()) {
  //     fprintf(stderr,
  //             "<Error> Found in line %d, col %d: symbol '%s' is
  //             redeclared\n", entry.node->getLocation().line,
  //             entry.node->getLocation().col, entry.name.c_str());
  //     is_valid = false;
  //   }
  //   names.push_back(entry.name);
  // }
  return is_valid;
}

void SymbolManager::pushScope() { tables.push(new SymbolTable()); }

bool SymbolManager::addSymbol(const std::string &name, const std::string &kind,
                              const std::string &type,
                              const std::string &attribute, AstNode *node) {
  if (std::find_if(loop_vars.begin(), loop_vars.end(),
                   [&name](const std::pair<std::string, int> &a) {
                     return a.first == name;
                   }) != loop_vars.end()) {
    return false;
  }
  if (kind == "loop_var") {
    loop_vars.emplace_back(name, tables.size() - 1);
  }
  return this->tables.top()->addSymbol(
      SymbolEntry(name, kind, tables.size() - 1, type, attribute, node));
}

bool SymbolManager::addSymbol(const std::string &name, const std::string &kind,
                              const std::string &type, AstNode *node) {
  return this->addSymbol(name, kind, type, "", node);
}

void SymbolManager::popScope() {
  int level = tables.size() - 1;
  while (!loop_vars.empty() && loop_vars.back().second == level) {
    loop_vars.pop_back();
  }
  tables.pop();
}

void SymbolManager::dumpLastScope() { tables.top()->dump(); }

void SymbolManager::semanticCheck() { tables.top()->semanticCheck(); }
