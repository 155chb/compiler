#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE* yyout;

#ifdef GENCODE_CHECK
#undef GENCODE_CHECK
#endif
#define GENCODE_CHECK 0

DummyInstruction *headGlobal;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    if (GENCODE_CHECK) printf("BinaryInstruction outputing...\n");
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case MULT:
        if (type != "float")
            op = "mul";
        else
            op = "fmul";
        break;
    case DIV:
        if (type != "float")
            op = "sdiv";
        else
            op = "fdiv";
        break;
    case MOD:
        op = "srem";
        break;
    case AND:
        op = "and";
        break;
    case OR:
        op = "or";
        break;
    case XOR:
        op = "xor";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
    if (GENCODE_CHECK) printf("BinaryInstruction output finished.\n");
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    if (GENCODE_CHECK) printf("CmpInstruction outputing...\n");
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode)
    {
    case E:
        if (type == "i32")
            op = "eq";
        else if (type == "float")
            op = "oeq";
        else if (type == "i1")
            op = "eq";
        break;
    case NE:
        if (type == "i32")
            op = "ne";
        else if (type == "float")
            op = "one";
        else if (type == "i1")
            op = "ne";
        break;
    case L:
        if (type == "i32")
            op = "slt";
        else if (type == "float")
            op = "olt";
        else if (type == "i1")
            op = "ult";
        break;
    case LE:
        if (type == "i32")
            op = "sle";
        else if (type == "float")
            op = "ole";
        else if (type == "i1")
            op = "ule";
        break;
    case G:
        if (type == "i32")
            op = "sgt";
        else if (type == "float")
            op = "ogt";
        else if (type == "i1")
            op = "ugt";
        break;
    case GE:
        if (type == "i32")
            op = "sge";
        else if (type == "float")
            op = "oge";
        else if (type == "i1")
            op = "uge";
        break;
    default:
        op = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
    if (GENCODE_CHECK) printf("CmpInstruction output finished.\n");
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    if (GENCODE_CHECK) printf("UncondBrInstruction outputing...\n");
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
    if (GENCODE_CHECK) printf("UncondBrInstruction output finished.\n");
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock*true_branch, BasicBlock*false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    if (GENCODE_CHECK) printf("CondBrInstruction outputing...\n");
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
    if (GENCODE_CHECK) printf("CondBrInstruction output finished.\n");
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if (GENCODE_CHECK) printf("RetInstruction outputing...\n");
    if(operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
    if (GENCODE_CHECK) printf("RetInstruction output finished.\n");
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    if (GENCODE_CHECK) printf("AllocaInstruction outputing...\n");
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    if (se->getType()->isArray()) {
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
        fprintf(yyout, "  store %s zeroinitializer, %s* %s, align 4\n", type.c_str(), type.c_str(), dst.c_str());
    }
    else
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
    if (GENCODE_CHECK) printf("AllocaInstruction output finished.\n");
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    if (GENCODE_CHECK) printf("LoadInstruction outputing...\n");
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
    if (GENCODE_CHECK) printf("LoadInstruction output finished.\n");
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    if (GENCODE_CHECK) printf("StoreInstruction outputing...\n");
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
    if (GENCODE_CHECK) printf("StoreInstruction output finished.\n");
}

CallInstruction::CallInstruction(Operand *dst, SymbolEntry *funcSE, std::vector<Operand *> args, BasicBlock *insert_bb) : Instruction(CALL, insert_bb)
{
    operands.push_back(dst);
    if (dst != nullptr){
        dst->setDef(this);       
    }
    this->funcSE = funcSE;
    for (auto arg : args)
    {
        operands.push_back(arg);
        arg->addUse(this);
    }
}

CallInstruction::~CallInstruction()
{
    if (operands[0] != nullptr){
        operands[0]->setDef(nullptr);
        if(operands[0]->usersNum() == 0)
            delete operands[0];        
    }
    for (auto arg = operands.begin() + 1; arg != operands.end(); ++arg)
        (*arg)->removeUse(this);
}

void CallInstruction::output() const {
    if (GENCODE_CHECK) printf("CallInstruction outputing...\n");
    if (operands[0] != nullptr) {
        std::string dst = operands[0]->toStr();
        std::string func = funcSE->toStr();
        std::string type = operands[0]->getType()->toStr();
        fprintf(yyout, "  %s = call %s %s(", dst.c_str(), type.c_str(), func.c_str());
        for (auto arg = operands.begin() + 1; arg != operands.end(); ++arg)
        {
            std::string arg_str = (*arg)->toStr();
            std::string arg_type = (*arg)->getType()->toStr();
            fprintf(yyout, "%s %s", arg_type.c_str(), arg_str.c_str());
            if (arg + 1 != operands.end())
                fprintf(yyout, ", ");
        }
        fprintf(yyout, ")\n");
    }
    else {
        std::string func = funcSE->toStr();
        fprintf(yyout, "  call void %s(", func.c_str());
        for (auto arg = operands.begin() + 1; arg != operands.end(); ++arg)
        {
            std::string arg_str = (*arg)->toStr();
            std::string arg_type = (*arg)->getType()->toStr();
            fprintf(yyout, "%s %s", arg_type.c_str(), arg_str.c_str());
            if (arg + 1 != operands.end())
                fprintf(yyout, ", ");
        }
        fprintf(yyout, ")\n");
    }
    if (GENCODE_CHECK) printf("CallInstruction output finished.\n");
}

GlobalVarInstruction::GlobalVarInstruction(Operand *dst, std::vector<Operand*> initVals) : Instruction(GLOBALVAR, nullptr)
{
    operands.push_back(dst);
    dst->setDef(this);
    for (auto initVal : initVals)
    {
        operands.push_back(initVal);
        initVal->addUse(this);
    }
    headGlobal->getPrev()->setNext(this);
    this->setPrev(headGlobal->getPrev());
    headGlobal->setPrev(this);
    this->setNext(headGlobal);
}

GlobalVarInstruction::~GlobalVarInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    for (auto initVal = operands.begin() + 1; initVal != operands.end(); ++initVal)
        (*initVal)->removeUse(this);
}

void GlobalVarInstruction::output() const
{
    if (GENCODE_CHECK) printf("GlobalVarInstruction outputing...\n");
    std::string dst = operands[0]->toStr();
    Type* type = operands[0]->getType();
    if (operands[0]->getType()->isArray()) {
        if (operands.size() == 1)
            fprintf(yyout, "%s = global %s zeroinitializer\n", dst.c_str(), type->toStr().c_str());
        else {
            // 这里检查了测试文件，全局数组要么是一维数组，要么就不需要初始化，这里就简单考虑了
            fprintf(yyout, "%s = global %s [", dst.c_str(), type->toStr().c_str());
            int size = ((ArrayType*)type)->getSize();
            std::string baseType = ((ArrayType*)type)->getBaseType()->toStr();
            for (auto initVal = operands.begin() + 1; initVal != operands.end(); ++initVal)
            {
                std::string initVal_str = (*initVal)->toStr();
                fprintf(yyout, "%s %s", baseType.c_str(), initVal_str.c_str());
                if (initVal + 1 != operands.end())
                    fprintf(yyout, ", ");
                size--;
            }
            while (size > 0) {
                fprintf(yyout, "%s 0", baseType.c_str());
                if (size > 1)
                    fprintf(yyout, ", ");
                size--;
            }
            fprintf(yyout, "]\n");
        }
    }
    else {
        if (operands.size() == 1) {
            fprintf(yyout, "%s = global %s 0\n", dst.c_str(), type->toStr().c_str());
        }
        else {
            std::string initVal_str = operands[1]->toStr();
            fprintf(yyout, "%s = global %s %s\n", dst.c_str(), type->toStr().c_str(), initVal_str.c_str());            
        }

    }
    if (GENCODE_CHECK) printf("GlobalVarInstruction output finished.\n");
}

FuncDeclInstruction::FuncDeclInstruction(SymbolEntry *funcSE) : Instruction(FUNCDECL, nullptr)
{
    this->funcSE = funcSE;
    headGlobal->getPrev()->setNext(this);
    this->setPrev(headGlobal->getPrev());
    headGlobal->setPrev(this);
    this->setNext(headGlobal);
}

FuncDeclInstruction::~FuncDeclInstruction()
{
    ;
}

void FuncDeclInstruction::output() const
{
    if (GENCODE_CHECK) printf("FuncDeclInstruction outputing...\n");
    FunctionType* funcType = dynamic_cast<FunctionType*>(funcSE->getType());
    std::string retType = funcType->getRetType()->toStr();
    fprintf(yyout, "declare %s %s(", retType.c_str(), funcSE->toStr().c_str());
    std::vector<Type*> paramsType = funcType->getParamsType();
    for (int i = 0; i < (int)paramsType.size(); i++)
    {
        if (i != 0)
            fprintf(yyout, ", ");
        std::string type = paramsType[i]->toStr();
        fprintf(yyout, "%s", type.c_str());
    }
    fprintf(yyout, ")\n");
    if (GENCODE_CHECK) printf("FuncDeclInstruction output finished.\n");
}

typeCastInstruction::typeCastInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(TYPECAST, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

typeCastInstruction::~typeCastInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void typeCastInstruction::output() const
{
    if (GENCODE_CHECK) printf("typeCastInstruction outputing...\n");
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type_str = operands[0]->getType()->toStr();
    std::string src_type_str = operands[1]->getType()->toStr();
    if (dst_type_str == "i1") {
        if (src_type_str == "i32")
            fprintf(yyout, "  %s = icmp ne i32 %s, 0\n", dst.c_str(), src.c_str());
        else if (src_type_str == "float")
            fprintf(yyout, "  %s = fcmp une float %s, 0.0\n", dst.c_str(), src.c_str());
    }
    else if (dst_type_str == "i32") {
        if (src_type_str == "i1")
            fprintf(yyout, "  %s = zext i1 %s to i32\n", dst.c_str(), src.c_str());
        else if (src_type_str == "float")
            fprintf(yyout, "  %s = fptosi float %s to i32\n", dst.c_str(), src.c_str());
    }
    else if (dst_type_str == "float") {
        if (src_type_str == "i1")
            fprintf(yyout, "  %s = sitofp i1 %s to float\n", dst.c_str(), src.c_str());
        else if (src_type_str == "i32")
            fprintf(yyout, "  %s = sitofp i32 %s to float\n", dst.c_str(), src.c_str());
    }
    else if (operands[0]->getType()->isPtr()){
        fprintf(yyout, "  %s = bitcast %s %s to %s\n", dst.c_str(), src_type_str.c_str(), src.c_str(), dst_type_str.c_str());
    }
    if (GENCODE_CHECK) printf("typeCastInstruction output finished.\n");
}

getElePtrInstruction::getElePtrInstruction(Operand *dst, Operand *src, std::vector<Operand*> idxList, BasicBlock *insert_bb) : Instruction(GETELEMENTPTR, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    for (auto idx : idxList)
    {
        operands.push_back(idx);
        idx->addUse(this);
    }
    dst->setDef(this);
    src->addUse(this);
}

getElePtrInstruction::~getElePtrInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    for (auto idx = operands.begin() + 2; idx != operands.end(); ++idx)
        (*idx)->removeUse(this);
}

void getElePtrInstruction::output() const
{
    if (GENCODE_CHECK) printf("getElePtrInstruction outputing...\n");
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type_str = dynamic_cast<PointerType*>(operands[1]->getType())->getValueType()->toStr();
    std::string src_type_str = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s", dst.c_str(), dst_type_str.c_str(), src_type_str.c_str(), src.c_str());
    for (auto idx = operands.begin() + 2; idx != operands.end(); ++idx)
    {
        std::string idx_str = (*idx)->toStr();
        std::string idx_type_str = (*idx)->getType()->toStr();
        fprintf(yyout, ", %s %s", idx_type_str.c_str(), idx_str.c_str());
    }
    fprintf(yyout, "\n");
    if (GENCODE_CHECK) printf("getElePtrInstruction output finished.\n");
}