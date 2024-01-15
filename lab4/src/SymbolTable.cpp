#include "SymbolTable.h"
#include <iostream>
#include <sstream>

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    if (type->isInt())
        buffer << iValue;
    else
        buffer << fValue;
    return buffer.str();
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
    // 添加运行时库
    Type *intType = new IntType(4, false);
    Type *floatType = new FloatType(4, false);
    Type *voidType = new VoidType();
    Type *intArrType = new ArrayType(intType, -1);
    Type *floatArrType = new ArrayType(floatType, -1);

    Type *none2intType = new FunctionType(intType, {});
    Type *intArr2intType = new FunctionType(intType, {intArrType});
    Type *none2floatType = new FunctionType(floatType, {});
    Type *floatArr2intType = new FunctionType(intType, {floatArrType});
    Type *int2voidType = new FunctionType(voidType, {intType});
    Type *intXintArr2voidType = new FunctionType(voidType, {intType, intArrType});
    Type *float2voidType = new FunctionType(voidType, {floatType});
    Type *intXfloatArr2voidType = new FunctionType(voidType, {intType, floatArrType});

    install("getint", new IdentifierSymbolEntry(none2intType, "getint", 0, 0, 0, nullptr));
    install("getch", new IdentifierSymbolEntry(none2intType, "getch", 0, 0, 0, nullptr));
    install("getarray", new IdentifierSymbolEntry(intArr2intType, "getarray", 0, 0, 0, nullptr));
    install("getfloat", new IdentifierSymbolEntry(none2floatType, "getfloat", 0, 0, 0, nullptr));
    install("getfarray", new IdentifierSymbolEntry(floatArr2intType, "getfarray", 0, 0, 0, nullptr));
    install("putint", new IdentifierSymbolEntry(int2voidType, "putint", 0, 0, 0, nullptr));
    install("putch", new IdentifierSymbolEntry(int2voidType, "putch", 0, 0, 0, nullptr));
    install("putarray", new IdentifierSymbolEntry(intXintArr2voidType, "putarray", 0, 0, 0, nullptr));
    install("putfloat", new IdentifierSymbolEntry(float2voidType, "putfloat", 0, 0, 0, nullptr));
    install("putfarray", new IdentifierSymbolEntry(intXfloatArr2voidType, "putfarray", 0, 0, 0, nullptr));
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    // Todo
    SymbolTable *cur = this;
    while (cur != nullptr) {
        if (cur->symbolTable.find(name) != cur->symbolTable.end()) {
            return cur->symbolTable[name];
        }
        cur = cur->prev;
    }
    return nullptr;
}

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
