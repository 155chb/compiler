#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

#ifndef AST_CHECK
#define AST_CHECK false
#endif

extern FILE *yyout;
int Node::counter = 0;

std::vector<Type*> FuncFParams::getParamsType()
{
    std::vector<Type*> paramsType;
    for (auto node : list)
        paramsType.push_back(((FuncFParam*)node)->getSymbolEntry()->getType());
    return paramsType;
}

void Ast::output()
{   
    if (AST_CHECK) yyout = stdout;
    if (root != nullptr)
        root->output(0);
}

void CompUnit::output(int level)
{
    if (level == 0)
        fprintf(yyout, "CompUnit\n");
    else
        fprintf(yyout, "%*cCompUnit\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void ConstDefs::output(int level)
{
    fprintf(yyout, "%*cConstDefs\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void ConstInitVal::output(int level)
{
    fprintf(yyout, "%*cConstInitVal\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void ConstInitVals::output(int level)
{
    fprintf(yyout, "%*cConstInitVals\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void VarDefs::output(int level)
{
    fprintf(yyout, "%*cVarDefs\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void InitVal::output(int level)
{
    fprintf(yyout, "%*cInitVal\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void InitVals::output(int level)
{
    fprintf(yyout, "%*cInitVals\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void FuncFParams::output(int level)
{
    fprintf(yyout, "%*cFuncFParams\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void Arrays::output(int level)
{
    fprintf(yyout, "%*cArrays\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void BlockItems::output(int level)
{
    fprintf(yyout, "%*cBlockItems\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void FuncRParams::output(int level)
{
    fprintf(yyout, "%*cFuncRParams\n", level, ' ');
    for (auto node : list)
        node->output(level + 4);
}

void BType::output(int level)
{
    ;
}

void ConstArrays::output(int level)
{
    ;
}

void Stmt::output(int level)
{
    fprintf(yyout, "%*cStmt\n", level, ' ');
    stmt->output(level + 4);
}

void Decl::output(int level)
{
    fprintf(yyout, "%*cDecl\n", level, ' ');
    decl->output(level + 4);
}

void ConstDecl::output(int level)
{
    fprintf(yyout, "%*cConstDecl\n", level, ' ');
    constDefs->output(level + 4);
}

void VarDecl::output(int level)
{
    fprintf(yyout, "%*cVarDecl\n", level, ' ');
    varDefs->output(level + 4);
}

void Block::output(int level)
{
    fprintf(yyout, "%*cBlock\n", level, ' ');
    if (blockItems != nullptr)
        blockItems->output(level + 4);
}

void BlockItem::output(int level)
{
    fprintf(yyout, "%*cBlockItem\n", level, ' ');
    stmt->output(level + 4);
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    if (expr != nullptr)
        expr->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    stmt->output(level + 4);
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    if (retValue != nullptr)
        retValue->output(level + 4);
}

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ConstExp::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cConstExp\tname: %s\ttype: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    expr->output(level + 4);
}

void FuncFParam::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFuncFParam\tname: %s\ttype: %s\n", level, ' ', 
            name.c_str(), type.c_str());
}

// Exp等虚拟类型不输出se，其se实际就是其指向的ExprNode的se
// 只是为了说明语法树的结构
void Exp::output(int level)
{
    fprintf(yyout, "%*cExp\n", level, ' ');
    expr->output(level + 4);
}

void Cond::output(int level)
{
    fprintf(yyout, "%*cCond\n", level, ' ');
    expr->output(level + 4);
}

void PrimaryExp::output(int level)
{
    fprintf(yyout, "%*cPrimaryExp\n", level, ' ');
    expr->output(level + 4);
}

void LVal::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cLVal\tname: %s\ttype: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if (arrs != nullptr)
        arrs->output(level + 4);
}

void Number::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cNumber\tname: %s\ttype: %s\n", level, ' ', 
            name.c_str(), type.c_str());
}

void UnaryExp::output(int level)
{
    std::string name, type, op_str;
    name = se->toStr();
    type = se->getType()->toStr();
    op_str = op == UnaryExp::PLUS ? "PLUS" :    \
             op == UnaryExp::MINUS ? "MINUS" :  \
             op == UnaryExp::NOT ? "NOT" : "ERROR";
    if (op == UnaryExp::NON)
        fprintf(yyout, "%*cUnaryExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cUnaryExp\tname: %s\ttype: %s\top: %s\n", level, ' ',
                name.c_str(), type.c_str(), op_str.c_str());
    expr->output(level + 4);
}

void MulExp::output(int level)
{
    std::string name, type, op_str;
    name = se->toStr();
    type = se->getType()->toStr();
    op_str = op == MulExp::MULT ? "MUL" :    \
             op == MulExp::DIV ? "DIV" :    \
             op == MulExp::MOD ? "MOD" : "ERROR";
    if (op == MulExp::NON)
        fprintf(yyout, "%*cMulExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cMulExp\tname: %s\ttype: %s\top: %s\n", level, ' ',
                name.c_str(), type.c_str(), op_str.c_str());
    expr1->output(level + 4);
    if (expr2 != nullptr)
        expr2->output(level + 4);
}

void AddExp::output(int level)
{
    std::string name, type, op_str;
    name = se->toStr();
    type = se->getType()->toStr();
    op_str = op == AddExp::ADD ? "ADD" :    \
             op == AddExp::SUB ? "SUB" : "ERROR";
    if (op == AddExp::NON)
        fprintf(yyout, "%*cAddExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cAddExp\tname: %s\ttype: %s\top: %s\n", level, ' ',
                name.c_str(), type.c_str(), op_str.c_str());
    expr1->output(level + 4);
    if (expr2 != nullptr)
        expr2->output(level + 4);
}

void RelExp::output(int level)
{
    std::string name, type, op_str;
    name = se->toStr();
    type = se->getType()->toStr();
    op_str = op == RelExp::LESS ? "LESS" :          \
             op == RelExp::GREATER ? "GREATER" :    \
             op == RelExp::LEQUAL ? "LEQUAL" :      \
             op == RelExp::GEQUAL ? "GEQUAL" : "ERROR";
    if (op == RelExp::NON)
        fprintf(yyout, "%*cRelExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cRelExp\tname: %s\ttype: %s\top: %s\n", level, ' ',
                name.c_str(), type.c_str(), op_str.c_str());
    expr1->output(level + 4);
    if (expr2 != nullptr)
        expr2->output(level + 4);
}

void EqExp::output(int level)
{
    std::string name, type, op_str;
    name = se->toStr();
    type = se->getType()->toStr();
    op_str = op == EqExp::EQUAL ? "EQUAL" :          \
             op == EqExp::NEQUAL ? "NEQUAL" : "ERROR";
    if (op == EqExp::NON)
        fprintf(yyout, "%*cEqExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cEqExp\tname: %s\ttype: %s\top: %s\n", level, ' ',
                name.c_str(), type.c_str(), op_str.c_str());
    expr1->output(level + 4);
    if (expr2 != nullptr)
        expr2->output(level + 4);
}

void LAndExp::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    if (op == LAndExp::NON)
        fprintf(yyout, "%*cLAndExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cLAndExp\tname: %s\ttype: %s\top: AND\n", level, ' ',
                name.c_str(), type.c_str());
    expr1->output(level + 4);
    if (expr2 != nullptr)
        expr2->output(level + 4);
}

void LOrExp::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    if (op == LOrExp::NON)
        fprintf(yyout, "%*cLOrExp\tname: %s\ttype: %s\n", level, ' ',
                name.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cLOrExp\tname: %s\ttype: %s\top: OR\n", level, ' ',
                name.c_str(), type.c_str());
    expr1->output(level + 4);
    if (expr2 != nullptr)
        expr2->output(level + 4);
}

void FuncCall::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFuncCall\tname: %s\ttype: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if (funcRParams != nullptr)
        funcRParams->output(level + 4);
}

void FuncDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFuncDef\tname: %s\ttype: %s\n", level, ' ',
            name.c_str(), type.c_str());
    block->output(level + 4);
}

void ConstDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cConstDef\tname: %s\ttype: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if (initValue != nullptr)
        initValue->output(level + 4);
}

void VarDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cVarDef\tname: %s\ttype: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if (initValue != nullptr)
        initValue->output(level + 4);
}