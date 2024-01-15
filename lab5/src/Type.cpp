#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(32, false);
FloatType TypeSystem::commonFloat = FloatType(32, false);
VoidType TypeSystem::commonVoid = VoidType();
IntType TypeSystem::commonBool = IntType(1, false);
IntType TypeSystem::commonConstInt = IntType(32, true);
FloatType TypeSystem::commonConstFloat = FloatType(32, true);
IntType TypeSystem::commonConstBool = IntType(1, true);

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::constIntType = &commonConstInt;
Type* TypeSystem::constFloatType = &commonConstFloat;
Type* TypeSystem::constBoolType = &commonConstBool;

std::string IntType::toStr()
{
    std::ostringstream buffer;
    buffer << "i" << size;
    return buffer.str();
}

std::string FloatType::toStr()
{
    std::ostringstream buffer;
    buffer << "float";
    return buffer.str();
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "(";
    for (int i = 0; i < (int)paramsType.size(); i++)
    {
        if (i != 0)
            buffer << ", ";
        buffer << paramsType[i]->toStr();
    }
    buffer << ")";
    return buffer.str();
}

void ArrayType::setBaseType(Type *baseType)
{
    if (this->baseType->isArray())
        dynamic_cast<ArrayType*>(this->baseType)->setBaseType(baseType);
    else
        this->baseType = baseType;
}

std::string ArrayType::toStr()
{
    std::ostringstream buffer;
    buffer << "[" << size << " x " << baseType->toStr() << "]";
    return buffer.str();
}

Type* ArrayType::getBaseType(int depth) const
{
    if (depth == 0)
        return baseType;
    else
        return dynamic_cast<ArrayType*>(baseType)->getBaseType(depth - 1);
}

std::vector<int> ArrayType::getSizeList()
{
    std::vector<int> sizeList;
    Type *type = this;
    while (type->isArray())
    {
        sizeList.push_back(dynamic_cast<ArrayType*>(type)->getSize());
        type = dynamic_cast<ArrayType*>(type)->getBaseType();
    }
    return sizeList;
}

PointerType* ArrayType::toPtrType()
{
    if (baseType->isArray())
        return new PointerType(dynamic_cast<ArrayType*>(baseType)->toPtrType());
    else
        return new PointerType(baseType);
}

std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}