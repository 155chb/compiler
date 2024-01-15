#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include <string>
#include <map>
#include "Type.h"
#include "Ast.h"

class SymbolEntry
{
private:
    int kind;
protected:
    Type *type;
    int iValue;
    float fValue;
    ListNode* arrValue;
    enum {CONSTANT, VARIABLE, TEMPORARY};

public:
    SymbolEntry(int kind, Type *type, int iValue, float fValue, ListNode* arrValue) : kind(kind), type(type), iValue(iValue), fValue(fValue), arrValue(arrValue) {};
    virtual ~SymbolEntry() {};
    bool isConstant() const {return kind == CONSTANT;};
    bool isTemporary() const {return kind == TEMPORARY;};
    bool isVariable() const {return kind == VARIABLE;};
    Type* getType() {return type;};
    int getIntValue() const {return iValue;};
    float getFloatValue() const {return fValue;};
    ListNode* getArrValue() const {return arrValue;};
    virtual std::string toStr() = 0;
    // You can add any function you need here.
};


/*  
    Symbol entry for literal constant. Example:

    int a = 1;

    Compiler should create constant symbol entry for literal constant '1'.
*/
class ConstantSymbolEntry : public SymbolEntry
{
public:
    ConstantSymbolEntry(Type *type, int ivalue, float fValue) : SymbolEntry(SymbolEntry::CONSTANT, type, ivalue, fValue, {}) {};
    virtual ~ConstantSymbolEntry() {};
    std::string toStr();
    // You can add any function you need here.
};


/* 
    Symbol entry for identifier. Example:

    int a;
    int b;
    void f(int c)
    {
        int d;
        {
            int e;
        }
    }

    Compiler should create identifier symbol entries for variables a, b, c, d and e:

    | variable | scope    |
    | a        | GLOBAL   |
    | b        | GLOBAL   |
    | c        | PARAM    |
    | d        | LOCAL    |
    | e        | LOCAL +1 |
*/
class IdentifierSymbolEntry : public SymbolEntry
{
private:
    enum {GLOBAL, PARAM, LOCAL};
    std::string name;
    int scope;
    // You can add any field you need here.

public:
    IdentifierSymbolEntry(Type *type, std::string name, int scope, int iValue, float fValue, ListNode* arrValue) : SymbolEntry(SymbolEntry::VARIABLE, type, iValue, fValue, arrValue), name(name), scope(scope) {};
    virtual ~IdentifierSymbolEntry() {};
    std::string toStr() {return name;};
    int getScope() const {return scope;};
    // You can add any function you need here.
};


/* 
    Symbol entry for temporary variable created by compiler. Example:

    int a;
    a = 1 + 2 + 3;

    The compiler would generate intermediate code like:

    t1 = 1 + 2
    t2 = t1 + 3
    a = t2

    So compiler should create temporary symbol entries for t1 and t2:

    | temporary variable | label |
    | t1                 | 1     |
    | t2                 | 2     |
*/
class TemporarySymbolEntry : public SymbolEntry
{
private:
    int label;
public:
    TemporarySymbolEntry(Type *type, int label, int iValue, float fValue) : SymbolEntry(SymbolEntry::TEMPORARY, type, iValue, fValue, {}), label(label) {};
    virtual ~TemporarySymbolEntry() {};
    std::string toStr();
    // You can add any function you need here.
};

// symbol table managing identifier symbol entries
class SymbolTable
{
private:
    std::map<std::string, SymbolEntry*> symbolTable;
    SymbolTable *prev;
    int level;
    static int counter;
public:
    SymbolTable();
    SymbolTable(SymbolTable *prev) : prev(prev), level(prev->level + 1) {};
    void install(std::string name, SymbolEntry* entry);
    SymbolEntry* lookup(std::string name);
    SymbolTable* getPrev() {return prev;};
    int getLevel() {return level;};
    static int getLabel() {return counter++;};
};

extern SymbolTable *identifiers;
extern SymbolTable *globals;

#endif
