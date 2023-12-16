#include <cstdio>

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

void SymbolTable::addSymbol(SymbolEntry entry) { entries.push_back(entry); }

void SymbolManager::pushScope() { tables.push(new SymbolTable()); }

void SymbolManager::addSymbol(const std::string &name, const std::string &kind,
                              const std::string &type,
                              const std::string &attribute) {
  tables.top()->addSymbol(
      SymbolEntry(name, kind, tables.size() - 1, type, attribute));
}

void SymbolManager::addSymbol(const std::string &name, const std::string &kind,
                              const std::string &type) {
  tables.top()->addSymbol(SymbolEntry(name, kind, tables.size() - 1, type, ""));
}

void SymbolManager::dumpLastScope() { tables.top()->dump(); }

void SymbolManager::popScope() { tables.pop(); }
