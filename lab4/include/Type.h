#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type
{
private:
    int kind;
    bool is_const;
protected:
    enum {INT, FLOAT, VOID, FUNC, ARRAY};
public:
    Type(int kind, bool is_const) : kind(kind), is_const(is_const) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isFloat() const {return kind == FLOAT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isArray() const {return kind == ARRAY;};
    bool isConst() const {return is_const;};
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size, bool isConst) : Type(Type::INT, isConst), size(size){};
    std::string toStr();
};

class FloatType : public Type
{
private:
    int size;
public:
    FloatType(int size, bool isConst) : Type(Type::FLOAT, isConst), size(size){};
    std::string toStr();
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID, false){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC, false), returnType(returnType), paramsType(paramsType){};
    Type* getRetType() const {return returnType;};
    std::vector<Type*> getParamsType() const {return paramsType;};
    std::string toStr();
};

class ArrayType : public Type
{
private:
    Type *baseType;
    int size;
public:
    ArrayType(Type *baseType, int size) : Type(Type::ARRAY, false), baseType(baseType), size(size){};
    void setBaseType(Type *baseType);
    std::string toStr();
    std::string baseTypeToStr();
    std::string sizeToStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static FloatType commonFloat;
    static VoidType commonVoid;
    static IntType commonBool;
    static IntType commonConstInt;
    static FloatType commonConstFloat;
    static IntType commonConstBool;
public:
    static Type *intType;
    static Type *floatType;
    static Type *voidType;
    static Type *boolType;
    static Type *constIntType;
    static Type *constFloatType;
    static Type *constBoolType;
};

#endif
