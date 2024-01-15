#include "Unit.h"
extern DummyInstruction *headGlobal;

#ifdef GENCODE_CHECK
#undef GENCODE_CHECK
#endif
#define GENCODE_CHECK 0

extern FILE *yyout;

Unit::Unit()
{
    headGlobal = new DummyInstruction();
}

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    if (GENCODE_CHECK) printf("Unit outputing...\n");
    if (GENCODE_CHECK) printf("Global instruction output...\n");
    for (auto inst = headGlobal->getNext(); inst != headGlobal; inst = inst->getNext())
        inst->output();
    if (headGlobal->getNext() != headGlobal)
        fprintf(yyout, "\n");
    if (GENCODE_CHECK) printf("Global instruction output finished.\n");
    for (auto &func : func_list) {
        func->output();
        fprintf(yyout, "\n");
    }

    if (GENCODE_CHECK) printf("Unit output finished.\n");
}

Unit::~Unit()
{
    auto delete_list = func_list;
    for(auto &func:delete_list)
        delete func;
}
