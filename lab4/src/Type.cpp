#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4, false);
FloatType TypeSystem::commonFloat = FloatType(4, false);
VoidType TypeSystem::commonVoid = VoidType();
IntType TypeSystem::commonBool = IntType(1, false);
IntType TypeSystem::commonConstInt = IntType(4, true);
FloatType TypeSystem::commonConstFloat = FloatType(4, true);
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
    if (isConst())
        buffer << "const ";
    buffer << "int" << size;
    return buffer.str();
}

std::string FloatType::toStr()
{
    std::ostringstream buffer;
    if (isConst())
        buffer << "const ";
    buffer << "float" << size;
    return buffer.str();
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    for (int i = 0; i < (int)paramsType.size(); i++)
    {
        if (i != 0)
            buffer << ", ";
        buffer << paramsType[i]->toStr();
    }
    buffer << " -> " << returnType->toStr();
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
    buffer << baseTypeToStr();
    buffer << sizeToStr();
    return buffer.str();
}

std::string ArrayType::baseTypeToStr()
{
    if (baseType->isArray())
        return dynamic_cast<ArrayType*>(baseType)->baseTypeToStr();
    else
        return baseType->toStr();
}

std::string ArrayType::sizeToStr()
{
    std::ostringstream buffer;
    if (size == -1)
        buffer << "[]";
    else
        buffer << "[" << size << "]";
    if (baseType->isArray())
        buffer << dynamic_cast<ArrayType*>(baseType)->sizeToStr();
    return buffer.str();
}