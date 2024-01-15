#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include <vector>
#include "Type.h"

class SymbolEntry;

class Node
{
private:
    static int counter;
    int seq;
public:
    Node() {seq = counter++;}
    int getSeq() const {return seq;};
    virtual void output(int level) = 0;
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
};

/* 二级节点子类 */

class ListNode : public Node
{
protected:
    std::vector<Node*> list;
public:
    ListNode() {};
    void add(Node*n) {list.push_back(n);}
    std::vector<Node*> getList() const {return list;}
};

class TypeNode : public Node
{
protected:
    Type* type;
public:
    TypeNode(Type* type) : type(type) {};
    Type* getType() const {return type;};
};

class StmtNode : public Node
{
public:
    StmtNode() {};
};

class ExprNode : public Node
{
protected:
    SymbolEntry *se;
public:
    ExprNode(SymbolEntry *se) : se(se){};
    SymbolEntry* getSymbolEntry() const {return se;};
};

class FuncDefNode : public Node
{
protected:
    SymbolEntry *se;
    StmtNode *block;     // 这里必须是一个Block
public:
    FuncDefNode(SymbolEntry *se, StmtNode *block) : se(se), block(block){};
    SymbolEntry* getSymbolEntry() const {return se;};
    StmtNode* getBlock() const {return block;};
};

class VarDefNode : public Node
{
protected:
    SymbolEntry *se;
    ListNode *initValue;    // 可以为空，const变量必须有初始值，这里使用ListNode是为了兼容数组与非数组
public:
    VarDefNode(SymbolEntry *se, ListNode *initValue) : se(se), initValue(initValue){};
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
};

class ConstDefs : public ListNode
{
public:
    ConstDefs() {};
    void output(int level);
};

class ConstInitVal : public ListNode
{
public:
    ConstInitVal() {};
    void output(int level);
};

class ConstInitVals : public ListNode
{
public:
    ConstInitVals() {};
    void output(int level);
};

class VarDefs : public ListNode
{
public:
    VarDefs() {};
    void output(int level);
};

class InitVal : public ListNode
{
public:
    InitVal() {};
    void output(int level);
};

class InitVals : public ListNode
{
public:
    InitVals() {};
    void output(int level);
};

class FuncFParams : public ListNode
{
public:
    FuncFParams() {};
    std::vector<Type*> getParamsType();
    void output(int level);
};

class Arrays : public ListNode
{
public:
    Arrays() {};
    void output(int level);
};

class BlockItems : public ListNode
{
public:
    BlockItems() {};
    void output(int level);
};

class FuncRParams : public ListNode
{
public:
    FuncRParams() {};
    void output(int level);
};

/* 类型类节点 */

class BType : public TypeNode   // 使用类型栈来实现类型的传递了，因此Btype不会被实例化
{
public:
    BType(Type* type) : TypeNode(type) {};
    void output(int level);
};

class ConstArrays : public TypeNode
{
public:
    ConstArrays(Type* type) : TypeNode(type) {};
    void output(int level);
};

/* 语句类节点 */

class Stmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    Stmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

class Decl : public StmtNode
{
private:
    StmtNode *decl;
public:
    Decl(StmtNode *decl) : decl(decl) {};
    void output(int level);
};

class ConstDecl : public StmtNode
{
private:
    ListNode* constDefs;
public:
    ConstDecl(ListNode* constDefs) : constDefs(constDefs) {};
    void output(int level);
};

class VarDecl : public StmtNode
{
private:
    ListNode* varDefs;
public:
    VarDecl(ListNode* varDefs) : varDefs(varDefs) {};
    void output(int level);
};

class Block : public StmtNode
{
private:
    ListNode* blockItems;
public:
    Block(ListNode* blockItems) : blockItems(blockItems) {};
    void output(int level);
};

class BlockItem : public StmtNode
{
private:
    StmtNode *stmt;
public:
    BlockItem(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

class ExprStmt : public StmtNode
{
private:
    ExprNode *expr;
public:
    ExprStmt(ExprNode *expr) : expr(expr) {};
    void output(int level);
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *stmt;
public:
    WhileStmt(ExprNode *cond, StmtNode *stmt) : cond(cond), stmt(stmt) {};
    void output(int level);
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt) {};
    void output(int level);
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
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
};

class BreakStmt : public StmtNode
{
public:
    BreakStmt() {};
    void output(int level);
};

class ContinueStmt : public StmtNode
{
public:
    ContinueStmt() {};
    void output(int level);
};

/* 表达式类节点 */

class ConstExp : public ExprNode
{
private:
    ExprNode *expr;
public:
    ConstExp(ExprNode *expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
};

class FuncFParam : public ExprNode
{
public:
    FuncFParam(SymbolEntry *se) : ExprNode(se) {};
    void output(int level);
};

class Exp : public ExprNode
{
private:
    ExprNode *expr;
public:
    Exp(ExprNode* expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
};

class Cond : public ExprNode
{
private:
    ExprNode *expr;
public:
    Cond(ExprNode* expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
};

class PrimaryExp : public ExprNode
{
private:
    ExprNode *expr;
public:
    PrimaryExp(ExprNode* expr) : ExprNode(expr->getSymbolEntry()), expr(expr) {};
    void output(int level);
};

class LVal : public ExprNode
{
private:
    ListNode *arrs;
public:
    LVal(SymbolEntry *se, ListNode *arrs) : ExprNode(se), arrs(arrs) {};
    void output(int level);
};

class Number : public ExprNode
{
public:
    Number(SymbolEntry *se) : ExprNode(se) {};
    void output(int level);
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
};

class FuncCall : public ExprNode
{
private:
    SymbolEntry *funcSE;
    ListNode *funcRParams;
public:
    FuncCall(SymbolEntry *se, SymbolEntry *funcSE, ListNode* funcRParams) : ExprNode(se), funcSE(funcSE), funcRParams(funcRParams) {};
    void output(int level);
};


/* 定义类节点 */

class FuncDef : public FuncDefNode
{
public:
    FuncDef(SymbolEntry *se, StmtNode *block) : FuncDefNode(se, block) {};
    void output(int level);
};

class ConstDef : public VarDefNode
{
public:
    ConstDef(SymbolEntry *se, ListNode *initValue) : VarDefNode(se, initValue) {};
    void output(int level);
};

class VarDef : public VarDefNode
{
public:
    VarDef(SymbolEntry *se, ListNode *initValue) : VarDefNode(se, initValue) {};
    void output(int level);
};

#endif
