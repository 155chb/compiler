#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

#ifdef GENCODE_CHECK
#undef GENCODE_CHECK
#endif
#define GENCODE_CHECK 0

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    if (GENCODE_CHECK) printf("Function outputing...\n");
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s(", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    for (auto &i : params_list)
    {
        fprintf(yyout, "%s %s", i->getType()->toStr().c_str(), params_table.find(i)->second->toStr().c_str());
        if (i != params_list.back())
            fprintf(yyout, ", ");
    }
    fprintf(yyout, ") {\n");
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
    if (GENCODE_CHECK) printf("Function output finished.\n");
}
