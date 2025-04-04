#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "Operand.h"
#include <vector>

class BasicBlock;

class Instruction
{
public:
    Instruction(unsigned instType, BasicBlock *insert_bb = nullptr);
    virtual ~Instruction();
    BasicBlock *getParent();
    bool isUncond() const {return instType == UNCOND;};
    bool isCond() const {return instType == COND;};
    bool isRet() const {return instType == RET;};
    bool isCall() const {return instType == CALL;};
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    Instruction *getNext();
    Instruction *getPrev();
    virtual Operand *getDef() { return nullptr; }
    virtual std::vector<Operand *> getUse() { return {}; }
    virtual void output() const = 0;
protected:
    unsigned instType;
    unsigned opcode;
    Instruction *prev;
    Instruction *next;
    BasicBlock *parent;
    std::vector<Operand*> operands;
    enum {BINARY, COND, UNCOND, RET, LOAD, STORE, CMP, ALLOCA, CALL, GLOBALVAR, FUNCDECL, TYPECAST, GETELEMENTPTR};
};

// meaningless instruction, used as the head node of the instruction list.
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr) {};
    void output() const {};
};

class AllocaInstruction : public Instruction
{
public:
    AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
private:
    SymbolEntry *se;
};

class LoadInstruction : public Instruction
{
public:
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1]}; }
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
    std::vector<Operand *> getUse() { return {operands[0], operands[1]}; }
};

class BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~BinaryInstruction();
    void output() const;
    enum {SUB, ADD, MULT, DIV, MOD,  AND, OR, XOR};
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }
};

class CmpInstruction : public Instruction
{
public:
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~CmpInstruction();
    void output() const;
    enum {E, NE, L, GE, G, LE};
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }
};

// unconditional branch
class UncondBrInstruction : public Instruction
{
public:
    UncondBrInstruction(BasicBlock*, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    BasicBlock **patchBranch() { return &branch; };
protected:
    BasicBlock *branch;
};

// conditional branch
class CondBrInstruction : public Instruction
{
public:
    CondBrInstruction(BasicBlock*, BasicBlock*, Operand *, BasicBlock *insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;
    void setTrueBranch(BasicBlock*);
    BasicBlock* getTrueBranch();
    void setFalseBranch(BasicBlock*);
    BasicBlock* getFalseBranch();
    BasicBlock **patchBranchTrue() { return &true_branch; };
    BasicBlock **patchBranchFalse() { return &false_branch; };
    std::vector<Operand *> getUse() { return {operands[0]}; }
protected:
    BasicBlock* true_branch;
    BasicBlock* false_branch;
};

class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src, BasicBlock *insert_bb = nullptr);
    ~RetInstruction();
    std::vector<Operand *> getUse()
    {
        if (operands.size())
            return {operands[0]};
        else
            return {};
    }
    void output() const;
};

class CallInstruction : public Instruction
{
public:
    CallInstruction(Operand *dst, SymbolEntry *funcSE, std::vector<Operand*> args, BasicBlock *insert_bb = nullptr);
    ~CallInstruction();
    std::vector<Operand *> getUse() {
        std::vector<Operand *> result;
        for (auto it = operands.begin() + 1; it != operands.end(); ++it)
            result.push_back(*it);
        return result;
    }
    void output() const;
    Operand *getDef() { return operands[0]; }
private:
    SymbolEntry *funcSE;
};

class GlobalVarInstruction : public Instruction
{
public:
    GlobalVarInstruction(Operand *dst, std::vector<Operand*> initVals);
    ~GlobalVarInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; };
    std::vector<Operand *> getUse() {
        std::vector<Operand *> result;
        for (auto it = operands.begin() + 1; it != operands.end(); ++it)
            result.push_back(*it);
        return result;
    }
};

class FuncDeclInstruction : public Instruction
{
public:
    FuncDeclInstruction(SymbolEntry *funcSE);
    ~FuncDeclInstruction();
    void output() const;
private:
    SymbolEntry *funcSE;
};

class typeCastInstruction : public Instruction
{
public:
    typeCastInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~typeCastInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1]}; }
};

class getElePtrInstruction : public Instruction
{
public:
    getElePtrInstruction(Operand *dst, Operand *src, std::vector<Operand*> idxList, BasicBlock *insert_bb = nullptr);
    ~getElePtrInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() {
        std::vector<Operand *> result;
        for (auto it = operands.begin() + 1; it != operands.end(); ++it)
            result.push_back(*it);
        return result;
    }
};

#endif