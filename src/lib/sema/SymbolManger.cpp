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

void SymbolManager::pushScope() { tables.emplace_back(new SymbolTable()); }

bool SymbolManager::addSymbol(const std::string &name, const std::string &kind,
                              const std::string &type,
                              const std::string &attribute, AstNode *node) {
  if (std::find_if(tables.rbegin(), tables.rend(), [&name](const auto &table) {
        auto sym = table->getSymbol(name);
        return sym != nullptr && sym->kind == "loop_var";
      }) != tables.rend()) {
    return false;
  }

  if (this->tables.back()->addSymbol(
          SymbolEntry(name, kind, tables.size() - 1, type, attribute, node))) {
    return true;
  }
  return false;
}

bool SymbolManager::addSymbol(const std::string &name, const std::string &kind,
                              const std::string &type, AstNode *node) {
  return this->addSymbol(name, kind, type, "", node);
}

void SymbolManager::popScope() {
  int level = tables.size() - 1;
  tables.pop_back();
}

void SymbolManager::dumpLastScope() { tables.back()->dump(); }
