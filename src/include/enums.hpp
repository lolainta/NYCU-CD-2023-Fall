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
    int_t = 0,
    real_t = 1,
    bool_t = 2,
    string_t = 3,
    void_t = 4,
    unknown_t = -1
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
