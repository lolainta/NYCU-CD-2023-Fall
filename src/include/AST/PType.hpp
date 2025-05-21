#ifndef AST_P_TYPE_H
#define AST_P_TYPE_H

#include <memory>
#include <string>
#include <vector>

class PType;

using PTypeSharedPtr = std::shared_ptr<PType>;

class PType {
 public:
  enum class PrimitiveTypeEnum : uint8_t {
    kVoidType,
    kIntegerType,
    kRealType,
    kBoolType,
    kStringType
  };

 private:
  PrimitiveTypeEnum m_type;
  std::vector<uint64_t> m_dimensions;
  mutable std::string m_type_string;
  mutable bool m_type_string_is_valid = false;

 public:
  ~PType() = default;
  PType(const PrimitiveTypeEnum type) : m_type(type) {}
  PType(const PrimitiveTypeEnum type, std::vector<uint64_t> &p_dims)
      : m_type(type), m_dimensions(std::move(p_dims)) {}
  PType(const PTypeSharedPtr &p_type)
      : m_type(p_type->getPrimitiveType()),
        m_dimensions(p_type->getDimensions()) {}

  void setPrimitiveType(const PrimitiveTypeEnum type) {
    m_type = type;
    m_type_string_is_valid = false;
  }
  void setDimensions(std::vector<uint64_t> &p_dims) {
    m_dimensions = std::move(p_dims);
  }
  const std::vector<uint64_t> &getDimensions() const { return m_dimensions; }
  PrimitiveTypeEnum getPrimitiveType() const { return m_type; }
  const char *getPTypeCString() const;
};

#endif
