#pragma once
#include <vector>

union ConstantValue
{
    int int_val;
    float real_val;
    bool bool_val;
    char *str_val;
};

enum SType
{
    int_t,
    real_t,
    bool_t,
    string_t,
    void_t,
    unknown_t,
};

typedef struct PType
{
    SType stype;
    std::vector<int> dim;
    void addDim(int dim) { this->dim.push_back(dim); }
    void setBaseType(SType stype) { this->stype = stype; }
    PType() : stype(SType::unknown_t) {}
    PType(SType stype) : stype(stype) {}
    PType(SType stype, std::vector<int> dim) : stype(stype), dim(dim) {}
} PType;

enum Operator
{
    NEGATIVE_op,
    MULTIPLY_op,
    DIVIDE_op,
    MOD_op,
    PLUS_op,
    MINUS_op,
    LESS_op,
    LESS_OR_EQUAL_op,
    GREATER_op,
    GREATER_OR_EQUAL_op,
    EQUAL_op,
    NOT_EQUAL_op,
    AND_op,
    OR_op,
    NOT_op,
};