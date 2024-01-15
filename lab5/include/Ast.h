#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include <vector>

#include "IRBuilder.h"

class Node
{
private:
    static int counter;
    int seq;
    int kind;
protected:
    std::vector<BasicBlock**> true_list;
    std::vector<BasicBlock**> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<BasicBlock**> &list, BasicBlock*target);
    std::vector<BasicBlock**> merge(std::vector<BasicBlock**> &list1, std::vector<BasicBlock**> &list2);
public:
    enum {LIST, TYPE, STMT, EXPR, FUNDEF, VARDEF};
    Node(int kind) : kind(kind) {seq = counter++;}
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    bool isList() {return kind == LIST;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<BasicBlock**>& trueList() {return true_list;}
    std::vector<BasicBlock**>& falseList() {return false_list;}
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    void typeCheck();
    void genCode(Unit *unit);
};

/* 二级节点子类 */

class ListNode : public Node
{
protected:
    std::vector<Node*> list;
public:
    ListNode() : Node(Node::LIST) {};
    void add(Node*n) {list.push_back(n);}
    std::vector<Node*> getList() const {return list;}
};

class TypeNode : public Node
{
protected:
    Type* type;
public:
    TypeNode(Type* type) : Node(Node::TYPE), type(type) {};
    Type* getType() const {return type;};
};

class StmtNode : public Node
{
public:
    StmtNode() : Node(Node::STMT) {};
};

class ExprNode : public Node
{
protected:
    SymbolEntry *se;
    Operand *dst;
public:
    ExprNode(SymbolEntry *se) : Node(Node::EXPR), se(se){};
    Operand* getOperand() {return dst;};
    SymbolEntry* getSymbolEntry() const {return se;};
};

class FuncDefNode : public Node
{
protected:
    SymbolEntry *se;
    StmtNode *block;     // 这里必须是一个Block
public:
    FuncDefNode(SymbolEntry *se, StmtNode *block) : Node(Node::FUNDEF), se(se), block(block){};
    SymbolEntry* getSymbolEntry() const {return se;};
    StmtNode* getBlock() const {return block;};
};

class VarDefNode : public Node
{
protected:
    SymbolEntry *se;
    ListNode *initValue;    // 可以为空，const变量必须有初始值，这里使用ListNode是为了兼容数组与非数组
public:
    VarDefNode(SymbolEntry *se, ListNode *initValue) : Node(Node::VARDEF), se(se), initValue(initValue){};
    SymbolEntry* getSymbolEntry() const {return se;};
    ListNode* getInitValue() const {return initValue;};
};

/* 三级节点子类 */

/* 列表类节点 */

class CompUnit : public ListNode
{
public:
    CompUnit() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstDefs : public ListNode
{
public:
    ConstDefs() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstInitVal : public ListNode
{
public:
    ConstInitVal() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstInitVals : public ListNode
{
public:
    ConstInitVals() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class VarDefs : public ListNode
{
public:
    VarDefs() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class InitVal : public ListNode
{
public:
    InitVal() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class InitVals : public ListNode
{
public:
    InitVals() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncFParams : public ListNode
{
public:
    FuncFParams() {};
    std::vector<Type*> getParamsType();
    void output(int level);
    void typeCheck();
    void genCode();
};

class Arrays : public ListNode
{
public:
    Arrays() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class BlockItems : public ListNode
{
public:
    BlockItems() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncRParams : public ListNode
{
public:
    FuncRParams() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

/* 类型类节点 */

class BType : public TypeNode   // 使用类型栈来实现类型的传递了，因此Btype不会被实例化
{
public:
    BType(Type* type) : TypeNode(type) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstArrays : public TypeNode
{
public:
    ConstArrays(Type* type) : TypeNode(type) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

/* 语句类节点 */

class Stmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    Stmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Decl : public StmtNode
{
private:
    StmtNode *decl;
public:
    Decl(StmtNode *decl) : decl(decl) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstDecl : public StmtNode
{
private:
    ListNode* constDefs;
public:
    ConstDecl(ListNode* constDefs) : constDefs(constDefs) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class VarDecl : public StmtNode
{
private:
    ListNode* varDefs;
public:
    VarDecl(ListNode* varDefs) : varDefs(varDefs) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Block : public StmtNode
{
private:
    ListNode* blockItems;
public:
    Block(ListNode* blockItems) : blockItems(blockItems) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class BlockItem : public StmtNode
{
private:
    StmtNode *stmt;
public:
    BlockItem(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ExprStmt : public StmtNode
{
private:
    ExprNode *expr;
public:
    ExprStmt(ExprNode *expr) : expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *stmt;
    BasicBlock *cond_bb;
    BasicBlock *end_bb;
public:
    WhileStmt(ExprNode *cond, StmtNode *stmt) : cond(cond), stmt(stmt) {};
    void output(int level);
    BasicBlock* getCondBB() const {return cond_bb;};
    BasicBlock* getEndBB() const {return end_bb;};
    void typeCheck();
    void genCode();
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class BreakStmt : public StmtNode
{
public:
    BreakStmt() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ContinueStmt : public StmtNode
{
public:
    ContinueStmt() {};
    void output(int level);
    void typeCheck();
    void genCode();
};

/* 表达式类节点 */

class ConstExp : public ExprNode
{
private:
    ExprNode *expr;
public:
    ConstExp(ExprNode *expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {}
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncFParam : public ExprNode
{
public:
    FuncFParam(SymbolEntry *se) : ExprNode(se) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Exp : public ExprNode
{
private:
    ExprNode *expr;
public:
    Exp(ExprNode* expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Cond : public ExprNode
{
private:
    ExprNode *expr;
public:
    Cond(ExprNode* expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class PrimaryExp : public ExprNode
{
private:
    ExprNode *expr;
public:
    PrimaryExp(ExprNode* expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class LVal : public ExprNode
{
private:
    ListNode *arrs;
public:
    LVal(SymbolEntry *se, ListNode *arrs) : ExprNode(se), arrs(arrs) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Number : public ExprNode
{
public:
    Number(SymbolEntry *se) : ExprNode(se) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class UnaryExp : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {PLUS, MINUS, NOT, NON};
    UnaryExp(SymbolEntry *se, int op, ExprNode* expr) : ExprNode(se), op(op), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class MulExp : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {MULT, DIV, MOD, NON};
    MulExp(SymbolEntry *se, int op, ExprNode* expr1, ExprNode* expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class AddExp : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, NON};
    AddExp(SymbolEntry *se, int op, ExprNode* expr1, ExprNode* expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class RelExp : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {LESS, GREATER, LEQUAL, GEQUAL, NON};
    RelExp(SymbolEntry *se, int op, ExprNode* expr1, ExprNode* expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class EqExp : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {EQUAL, NEQUAL, NON};
    EqExp(SymbolEntry *se, int op, ExprNode* expr1, ExprNode* expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class LAndExp : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {AND, NON};
    LAndExp(SymbolEntry *se, int op, ExprNode* expr1, ExprNode* expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class LOrExp : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {OR, NON};
    LOrExp(SymbolEntry *se, int op, ExprNode* expr1, ExprNode* expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FuncCall : public ExprNode
{
private:
    SymbolEntry *funcSE;
    ListNode *funcRParams;
public:
    FuncCall(SymbolEntry *se, SymbolEntry *funcSE, ListNode* funcRParams) : ExprNode(se), funcSE(funcSE), funcRParams(funcRParams) {};
    void output(int level);
    void typeCheck();
    void genCode();
};


/* 定义类节点 */

class FuncDef : public FuncDefNode
{
private:
    ListNode* funcFParams;
public:
    FuncDef(SymbolEntry *se, StmtNode *block, ListNode* funcFParams) : FuncDefNode(se, block), funcFParams(funcFParams) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstDef : public VarDefNode
{
public:
    ConstDef(SymbolEntry *se, ListNode *initValue) : VarDefNode(se, initValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class VarDef : public VarDefNode
{
public:
    VarDef(SymbolEntry *se, ListNode *initValue) : VarDefNode(se, initValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

void typeCast(Operand **dst1, Operand **dst2, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);

void initVal2IntArray(int* array, std::vector<int> dims, Node* initVal);

void initVal2FloatArray(float* array, std::vector<int> dims, Node* initVal);

std::vector<Operand*> intArray2OpVector(int *array, int size);

std::vector<Operand*> floatArray2OpVector(float *array, int size);

#endif
