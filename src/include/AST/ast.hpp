#ifndef __AST_AST_NODE_H
#define __AST_AST_NODE_H

#include <cstdint>

class AstNodeVisitor;

struct Location
{
  uint32_t line;
  uint32_t col;

  Location(const uint32_t line,
           const uint32_t col) : line(line), col(col) {}
  ~Location() = default;
};

class AstNode
{
protected:
  Location location;

public:
  AstNode(const uint32_t line,
          const uint32_t col);
  virtual ~AstNode() = 0;

  const Location &getLocation() const;

  virtual void accept(AstNodeVisitor &p_visitor) = 0;
};

#endif
