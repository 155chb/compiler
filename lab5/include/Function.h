#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include "BasicBlock.h"

class Unit;

class Function
{
    typedef std::vector<BasicBlock *>::iterator iterator;
    typedef std::vector<BasicBlock *>::reverse_iterator reverse_iterator;

private:
    std::vector<BasicBlock *> block_list;
    SymbolEntry *sym_ptr;
    BasicBlock *entry;
    Unit *parent;
    std::map<SymbolEntry*, Operand*> params_table;
    std::vector<SymbolEntry*> params_list;

public:
    Function(Unit *, SymbolEntry *);
    ~Function();
    void insertBlock(BasicBlock *bb) { block_list.push_back(bb); };
    BasicBlock *getEntry() { return entry; };
    void remove(BasicBlock *bb);
    void output() const;
    std::vector<BasicBlock *> &getBlockList(){return block_list;};
    iterator begin() { return block_list.begin(); };
    iterator end() { return block_list.end(); };
    reverse_iterator rbegin() { return block_list.rbegin(); };
    reverse_iterator rend() { return block_list.rend(); };
    SymbolEntry *getSymPtr() { return sym_ptr; };
    void paramsInstall(SymbolEntry *se, Operand *op) {
        params_table[se] = op;
        params_list.push_back(se);
    };
    void paramsChange(SymbolEntry *se, Operand *op) {
        params_table.find(se)->second = op;
    };
    Operand *paramsLookup(SymbolEntry *se) { return params_table[se]; };
};

#endif
