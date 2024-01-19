#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"
#include <set>

bool a = 0;

#ifdef AST_CHECK
#undef AST_CHECK
#endif
#define AST_CHECK a

#define TYPE_CHECK 0

#ifdef GENCODE_CHECK
#undef GENCODE_CHECK
#endif
#define GENCODE_CHECK 0
int check_depth = 1;

extern FILE *yyout;
int Node::counter = 0;
IRBuilder *Node::builder = nullptr;

Type* retType_for_typeCheck = nullptr;
std::vector<WhileStmt*> whileStmtStack_for_genCode;
bool isLVal = false;
bool isRParams = false;
std::set<std::string> sysyRunLibFuncs = {
    "@getint", "@getch", "@getarray", "@getfloat", "@getfarray", "@putint", "@putch", "@putarray", "@putfloat", "@putfarray"
};

void Node::backPatch(std::vector<BasicBlock**> &list, BasicBlock*target)
{
    if (GENCODE_CHECK) printf("backPatch...\n");
    for(auto &bb:list)
        *bb = target;
    if (GENCODE_CHECK) {
        for (auto &bb:list)
            printf("backPatch: %p -> %p\n", bb, *bb);
    }
    if (GENCODE_CHECK) printf("backPatch passed!\n");
}

std::vector<BasicBlock**> Node::merge(std::vector<BasicBlock**> &list1, std::vector<BasicBlock**> &list2)
{
    std::vector<BasicBlock**> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

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

void Ast::typeCheck()
{
    if (root != nullptr)
        root->typeCheck();
}

void CompUnit::typeCheck()
{
    if (TYPE_CHECK) printf("CompUnit typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("CompUnit typeCheck passed!\n");
}

void ConstDefs::typeCheck()
{
    if (TYPE_CHECK) printf("ConstDefs typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("ConstDefs typeCheck passed!\n");
}

void ConstInitVal::typeCheck()
{
    if (TYPE_CHECK) printf("ConstInitVal typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("ConstInitVal typeCheck passed!\n");
}

void ConstInitVals::typeCheck()
{
    if (TYPE_CHECK) printf("ConstInitVals typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("ConstInitVals typeCheck passed!\n");
}

void VarDefs::typeCheck()
{
    if (TYPE_CHECK) printf("VarDefs typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("VarDefs typeCheck passed!\n");
}

void InitVal::typeCheck()
{
    if (TYPE_CHECK) printf("InitVal typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("InitVal typeCheck passed!\n");
}

void InitVals::typeCheck()
{
    if (TYPE_CHECK) printf("InitVals typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("InitVals typeCheck passed!\n");
}

void FuncFParams::typeCheck()
{
    if (TYPE_CHECK) printf("FuncFParams typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("FuncFParams typeCheck passed!\n");
}

void Arrays::typeCheck()
{
    if (TYPE_CHECK) printf("Arrays typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("Arrays typeCheck passed!\n");
}

void BlockItems::typeCheck()
{
    if (TYPE_CHECK) printf("BlockItems typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("BlockItems typeCheck passed!\n");
}

void FuncRParams::typeCheck()
{
    if (TYPE_CHECK) printf("FuncRParams typeChecking...\n");
    for (auto node : list)
        node->typeCheck();
    if (TYPE_CHECK) printf("FuncRParams typeCheck passed!\n");
}

void BType::typeCheck()
{
    ;
}

void ConstArrays::typeCheck()
{
    ;
}

void Stmt::typeCheck()
{
    if (TYPE_CHECK) printf("Stmt typeChecking...\n");
    stmt->typeCheck();
    if (TYPE_CHECK) printf("Stmt typeCheck passed!\n");
}

void Decl::typeCheck()
{
    if (TYPE_CHECK) printf("Decl typeChecking...\n");
    decl->typeCheck();
    if (TYPE_CHECK) printf("Decl typeCheck passed!\n");
}

void ConstDecl::typeCheck()
{
    if (TYPE_CHECK) printf("ConstDecl typeChecking...\n");
    constDefs->typeCheck();
    if (TYPE_CHECK) printf("ConstDecl typeCheck passed!\n");
}

void VarDecl::typeCheck()
{
    if (TYPE_CHECK) printf("VarDecl typeChecking...\n");
    varDefs->typeCheck();
    if (TYPE_CHECK) printf("VarDecl typeCheck passed!\n");
}

void Block::typeCheck()
{
    if (TYPE_CHECK) printf("Block typeChecking...\n");
    if (blockItems != nullptr)
        blockItems->typeCheck();
    if (TYPE_CHECK) printf("Block typeCheck passed!\n");
}

void BlockItem::typeCheck()
{
    if (TYPE_CHECK) printf("BlockItem typeChecking...\n");
    stmt->typeCheck();
    if (TYPE_CHECK) printf("BlockItem typeCheck passed!\n");
}

void ExprStmt::typeCheck()
{
    if (TYPE_CHECK) printf("ExprStmt typeChecking...\n");
    if (expr != nullptr)
        expr->typeCheck();
    if (TYPE_CHECK) printf("ExprStmt typeCheck passed!\n");
}

void AssignStmt::typeCheck()
{
    if (TYPE_CHECK) printf("AssignStmt typeChecking...\n");
    lval->typeCheck();
    expr->typeCheck();
    if (TYPE_CHECK) printf("AssignStmt typeCheck passed!\n");
}

void WhileStmt::typeCheck()
{
    if (TYPE_CHECK) printf("WhileStmt typeChecking...\n");
    cond->typeCheck();
    stmt->typeCheck();
    if (TYPE_CHECK) printf("WhileStmt typeCheck passed!\n");
}

void IfStmt::typeCheck()
{
    if (TYPE_CHECK) printf("IfStmt typeChecking...\n");
    cond->typeCheck();
    thenStmt->typeCheck();
    if (TYPE_CHECK) printf("IfStmt typeCheck passed!\n");
}

void IfElseStmt::typeCheck()
{
    if (TYPE_CHECK) printf("IfElseStmt typeChecking...\n");
    cond->typeCheck();
    thenStmt->typeCheck();
    elseStmt->typeCheck();
    if (TYPE_CHECK) printf("IfElseStmt typeCheck passed!\n");
}

void ReturnStmt::typeCheck()
{
    if (TYPE_CHECK) printf("ReturnStmt typeChecking...\n");
    if (retValue != nullptr)
        retValue->typeCheck();
    if (TYPE_CHECK) printf("ReturnStmt typeCheck passed!\n");
}

void BreakStmt::typeCheck()
{
    ;
}

void ContinueStmt::typeCheck()
{
    ;
}

void ConstExp::typeCheck()
{
    if (TYPE_CHECK) printf("ConstExp typeChecking...\n");
    expr->typeCheck();
    if (!se->getType()->isConst())
        fprintf(stderr, "Error type %s is not a const\n", se->toStr().c_str());
    if (!se->getType()->isInt())
        fprintf(stderr, "Error type %s is not an int\n", se->toStr().c_str());
    if (TYPE_CHECK) printf("ConstExp typeCheck passed!\n");
}

void FuncFParam::typeCheck()
{
    ;
}

void Exp::typeCheck()
{
    if (TYPE_CHECK) printf("Exp typeChecking...\n");
    expr->typeCheck();
    if (TYPE_CHECK) printf("Exp typeCheck passed!\n");
}

void Cond::typeCheck()
{
    if (TYPE_CHECK) printf("Cond typeChecking...\n");
    expr->typeCheck();
    if (TYPE_CHECK) printf("Cond typeCheck passed!\n");
}

void PrimaryExp::typeCheck()
{
    if (TYPE_CHECK) printf("PrimaryExp typeChecking...\n");
    expr->typeCheck();
    if (TYPE_CHECK) printf("PrimaryExp typeCheck passed!\n");
}

void LVal::typeCheck()
{
    if (TYPE_CHECK) printf("LVal typeChecking...\n");
    if (arrs != nullptr)
        arrs->typeCheck();
    if (TYPE_CHECK) printf("LVal typeCheck passed!\n");
}

void Number::typeCheck()
{
    ;
}

void UnaryExp::typeCheck()
{
    if (TYPE_CHECK) printf("UnaryExp typeChecking...\n");
    expr->typeCheck();
    if (TYPE_CHECK) printf("UnaryExp typeCheck passed!\n");
}

void MulExp::typeCheck()
{
    if (TYPE_CHECK) printf("MulExp typeChecking...\n");
    expr1->typeCheck();
    if (expr2 != nullptr)
        expr2->typeCheck();
    if (TYPE_CHECK) printf("MulExp typeCheck passed!\n");
}

void AddExp::typeCheck()
{
    if (TYPE_CHECK) printf("AddExp typeChecking...\n");
    expr1->typeCheck();
    if (expr2 != nullptr)
        expr2->typeCheck();
    if (TYPE_CHECK) printf("AddExp typeCheck passed!\n");
}

void RelExp::typeCheck()
{
    if (TYPE_CHECK) printf("RelExp typeChecking...\n");
    expr1->typeCheck();
    if (expr2 != nullptr)
        expr2->typeCheck();
    if (TYPE_CHECK) printf("RelExp typeCheck passed!\n");
}

void EqExp::typeCheck()
{
    if (TYPE_CHECK) printf("EqExp typeChecking...\n");
    expr1->typeCheck();
    if (expr2 != nullptr)
        expr2->typeCheck();
    if (TYPE_CHECK) printf("EqExp typeCheck passed!\n");
}

void LAndExp::typeCheck()
{
    if (TYPE_CHECK) printf("LAndExp typeChecking...\n");
    expr1->typeCheck();
    if (expr2 != nullptr)
        expr2->typeCheck();
    if (TYPE_CHECK) printf("LAndExp typeCheck passed!\n");
}

void LOrExp::typeCheck()
{
    if (TYPE_CHECK) printf("LOrExp typeChecking...\n");
    expr1->typeCheck();
    if (expr2 != nullptr)
        expr2->typeCheck();
    if (TYPE_CHECK) printf("LOrExp typeCheck passed!\n");
}

void FuncCall::typeCheck()
{
    if (TYPE_CHECK) printf("FuncCall typeChecking...\n");
    if (funcRParams != nullptr)
        funcRParams->typeCheck();
    if (!funcSE->getType()->isFunc())
        fprintf(stderr, "Error type %s is not a function\n", se->toStr().c_str());
    if (TYPE_CHECK) printf("FuncCall typeCheck passed!\n");
}

void FuncDef::typeCheck()
{
    if (TYPE_CHECK) printf("FuncDef typeChecking...\n");
    retType_for_typeCheck = dynamic_cast<FunctionType*>(se->getType())->getRetType();
    block->typeCheck();
    if (TYPE_CHECK) printf("FuncDef typeCheck passed!\n");
}

void ConstDef::typeCheck()
{
    if (TYPE_CHECK) printf("ConstDef typeChecking...\n");
    if (initValue != nullptr)
        initValue->typeCheck();
    if (TYPE_CHECK) printf("ConstDef typeCheck passed!\n");
}

void VarDef::typeCheck()
{
    if (TYPE_CHECK) printf("VarDef typeChecking...\n");
    if (initValue != nullptr)
        initValue->typeCheck();
    if (TYPE_CHECK) printf("VarDef typeCheck passed!\n");
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

/* 中间代码生成 */

void CompUnit::genCode()
{
    if (GENCODE_CHECK) printf("%*cCompUnit genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cCompUnit genCode passed!\n", check_depth--, ' ');
}

void ConstDefs::genCode()
{
    if (GENCODE_CHECK) printf("%*cConstDefs genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cConstDefs genCode passed!\n", check_depth--, ' ');
}

void ConstInitVal::genCode()
{
    if (GENCODE_CHECK) printf("%*cConstInitVal genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cConstInitVal genCode passed!\n", check_depth--, ' ');
}

void ConstInitVals::genCode()
{
    if (GENCODE_CHECK) printf("%*cConstInitVals genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cConstInitVals genCode passed!\n", check_depth--, ' ');
}

void VarDefs::genCode()
{
    if (GENCODE_CHECK) printf("%*cVarDefs genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cVarDefs genCode passed!\n", check_depth--, ' ');
}

void InitVal::genCode()
{
    if (GENCODE_CHECK) printf("%*cInitVal genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cInitVal genCode passed!\n", check_depth--, ' ');
}

void InitVals::genCode()
{
    if (GENCODE_CHECK) printf("%*cInitVals genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cInitVals genCode passed!\n", check_depth--, ' ');
}

void FuncFParams::genCode()
{
    if (GENCODE_CHECK) printf("%*cFuncFParams genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cFuncFParams genCode passed!\n", check_depth--, ' ');
}

void Arrays::genCode()
{
    if (GENCODE_CHECK) printf("%*cArrays genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cArrays genCode passed!\n", check_depth--, ' ');
}

void BlockItems::genCode()
{
    if (GENCODE_CHECK) printf("%*cBlockItems genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cBlockItems genCode passed!\n", check_depth--, ' ');
}

void FuncRParams::genCode()
{
    if (GENCODE_CHECK) printf("%*cFuncRParams genCoding...\n", ++check_depth, ' ');
    for (auto node : list)
        node->genCode();
    if (GENCODE_CHECK) printf("%*cFuncRParams genCode passed!\n", check_depth--, ' ');
}

void BType::genCode()
{
    ;
}

void ConstArrays::genCode()
{
    ;
}

void Stmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cStmt genCoding...\n", ++check_depth, ' ');
    stmt->genCode();
    if (GENCODE_CHECK) printf("%*cStmt genCode passed!\n", check_depth--, ' ');
}

void Decl::genCode()
{
    if (GENCODE_CHECK) printf("%*cDecl genCoding...\n", ++check_depth, ' ');
    decl->genCode();
    if (GENCODE_CHECK) printf("%*cDecl genCode passed!\n", check_depth--, ' ');
}

void ConstDecl::genCode()
{
    if (GENCODE_CHECK) printf("%*cConstDecl genCoding...\n", ++check_depth, ' ');
    constDefs->genCode();
    if (GENCODE_CHECK) printf("%*cConstDecl genCode passed!\n", check_depth--, ' ');
}

void VarDecl::genCode()
{
    if (GENCODE_CHECK) printf("%*cVarDecl genCoding...\n", ++check_depth, ' ');
    varDefs->genCode();
    if (GENCODE_CHECK) printf("%*cVarDecl genCode passed!\n", check_depth--, ' ');
}

void Block::genCode()
{
    if (GENCODE_CHECK) printf("%*cBlock genCoding...\n", ++check_depth, ' ');
    if (blockItems != nullptr)
        blockItems->genCode();
    if (GENCODE_CHECK) printf("%*cBlock genCode passed!\n", check_depth--, ' ');
}

void BlockItem::genCode()
{
    if (GENCODE_CHECK) printf("%*cBlockItem genCoding...\n", ++check_depth, ' ');
    stmt->genCode();
    if (GENCODE_CHECK) printf("%*cBlockItem genCode passed!\n", check_depth--, ' ');
}

void ExprStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cExprStmt genCoding...\n", ++check_depth, ' ');
    if (expr != nullptr)
        expr->genCode();
    if (GENCODE_CHECK) printf("%*cExprStmt genCode passed!\n", check_depth--, ' ');
}

void AssignStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cAssignStmt genCoding...\n", ++check_depth, ' ');
    isLVal = true;
    lval->genCode();
    isLVal = false;
    expr->genCode();
    Operand *dst_addr;
    if (lval->getSymbolEntry()->getType()->isConst())
        dst_addr = lval->getOperand();
    else
        dst_addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymbolEntry())->getAddr();
    new StoreInstruction(dst_addr, expr->getOperand(), builder->getInsertBB());
    if (GENCODE_CHECK) printf("%*cAssignStmt genCode passed!\n", check_depth--, ' ');
}

void WhileStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cWhileStmt genCoding...\n", ++check_depth, ' ');
    whileStmtStack_for_genCode.push_back(this);
    Function *func = builder->getInsertBB()->getParent();
    BasicBlock *cond_bb = new BasicBlock(func);
    BasicBlock *then_bb = new BasicBlock(func);
    BasicBlock *end_bb = new BasicBlock(func);
    this->cond_bb = cond_bb;
    this->end_bb = end_bb;
    new UncondBrInstruction(cond_bb, builder->getInsertBB());
    builder->setInsertBB(cond_bb);
    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);
    builder->setInsertBB(then_bb);
    stmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(cond_bb, then_bb);
    builder->setInsertBB(end_bb);
    whileStmtStack_for_genCode.pop_back();
    if (GENCODE_CHECK) printf("%*cWhileStmt genCode passed!\n", check_depth--, ' ');
}

void IfStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cIfStmt genCoding...\n", ++check_depth, ' ');
    Function *func = builder->getInsertBB()->getParent();
    BasicBlock *then_bb = new BasicBlock(func);
    BasicBlock *end_bb = new BasicBlock(func);
    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);
    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);
    builder->setInsertBB(end_bb);
    if (GENCODE_CHECK) printf("%*cIfStmt genCode passed!\n", check_depth--, ' ');
}

void IfElseStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cIfElseStmt genCoding...\n", ++check_depth, ' ');
    Function *func = builder->getInsertBB()->getParent();
    BasicBlock *then_bb = new BasicBlock(func);
    BasicBlock *else_bb = new BasicBlock(func);
    BasicBlock *end_bb = new BasicBlock(func);
    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), else_bb);
    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);
    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);
    builder->setInsertBB(end_bb);
    if (GENCODE_CHECK) printf("%*cIfElseStmt genCode passed!\n", check_depth--, ' ');
}

void ReturnStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cReturnStmt genCoding...\n", ++check_depth, ' ');
    if (retValue != nullptr) {
        retValue->genCode();
        new RetInstruction(retValue->getOperand(), builder->getInsertBB());
    }
    else {
        new RetInstruction(nullptr, builder->getInsertBB());
    }
    if (GENCODE_CHECK) printf("%*cReturnStmt genCode passed!\n", check_depth--, ' ');
}

void BreakStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cBreakStmt genCoding...\n", ++check_depth, ' ');
    new UncondBrInstruction(whileStmtStack_for_genCode.back()->getEndBB(), builder->getInsertBB());
    if (GENCODE_CHECK) printf("%*cBreakStmt genCode passed!\n", check_depth--, ' ');
}

void ContinueStmt::genCode()
{
    if (GENCODE_CHECK) printf("%*cContinueStmt genCoding...\n", ++check_depth, ' ');
    new UncondBrInstruction(whileStmtStack_for_genCode.back()->getCondBB(), builder->getInsertBB());
    if (GENCODE_CHECK) printf("%*cContinueStmt genCode passed!\n", check_depth--, ' ');
}

void ConstExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cConstExp genCoding...\n", ++check_depth, ' ');
    expr->genCode();
    dst = expr->getOperand();
    if (GENCODE_CHECK) printf("%*cConstExp genCode passed!\n", check_depth--, ' ');
}

void FuncFParam::genCode()
{
    if (GENCODE_CHECK) printf("%*cFuncFParam genCoding...\n", ++check_depth, ' ');
    Function *func = builder->getInsertBB()->getParent();
    SymbolEntry *addr_se = new TemporarySymbolEntry(new PointerType(se->getType()), SymbolTable::getLabel(), 0, 0.0);
    Operand *temp = new Operand(new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel(), 0, 0.0));
    if (!se->getType()->isPtr())
        dynamic_cast<IdentifierSymbolEntry*>(se)->setAddr(new Operand(addr_se));
    else
        dynamic_cast<IdentifierSymbolEntry*>(se)->setAddr(temp);
    if (GENCODE_CHECK) printf("%p -> %p\n", se, dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr());
    func->paramsInstall(se, temp);
    BasicBlock *bb = func->getEntry();
    if (!se->getType()->isPtr()) {
        new AllocaInstruction(dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr(), se, bb);
        new StoreInstruction(dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr(), temp, bb);       
    }
    if (GENCODE_CHECK) printf("%*cFuncFParam genCode passed!\n", check_depth--, ' ');
}

void Exp::genCode()
{
    if (GENCODE_CHECK) printf("%*cExp genCoding...\n", ++check_depth, ' ');
    expr->genCode();
    dst = expr->getOperand();
    if (GENCODE_CHECK) printf("%*cExp genCode passed!\n", check_depth--, ' ');
}

void Cond::genCode()
{
    if (GENCODE_CHECK) printf("%*cCond genCoding...\n", ++check_depth, ' ');
    expr->genCode();
    dst = expr->getOperand();
    trueList() = expr->trueList();
    falseList() = expr->falseList();
    if (GENCODE_CHECK) {
        for (auto bb : trueList())
            printf("trueList: %p -> %p\n", bb, *bb);
        for (auto bb : falseList())
            printf("falseList: %p -> %p\n", bb, *bb);
    }
    if (GENCODE_CHECK) printf("%*cCond genCode passed!\n", check_depth--, ' ');
}

void PrimaryExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cPrimaryExp genCoding...\n", ++check_depth, ' ');
    expr->genCode();
    dst = expr->getOperand();
    if (GENCODE_CHECK) printf("%*cPrimaryExp genCode passed!\n", check_depth--, ' ');
}

void LVal::genCode()
{
    if (GENCODE_CHECK) printf("%*cLVal genCoding...\n", ++check_depth, ' ');
    if (arrs == nullptr) {
        if (!se->getType()->isArray() && !se->getType()->isPtr()) {
            dst = new Operand(new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel(), 0, 0.0));
            Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr();
            new LoadInstruction(dst, addr, builder->getInsertBB());
        }
        else {
            if (isRParams && se->getType()->isArray()) {
                Operand *src = dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr();
                Type *newType = new PointerType(dynamic_cast<ArrayType*>(se->getType())->getBaseType());
                dst = new Operand(new TemporarySymbolEntry(newType, SymbolTable::getLabel(), 0, 0.0));
                new typeCastInstruction(dst, src, builder->getInsertBB());
            }
            else
                dst = dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr();
        }
    }
    else {
        bool temp_val = isLVal, temp_rparams = isRParams;
        isLVal = false, isRParams = false;
        arrs->genCode();
        std::vector<Operand*> idxs;
        idxs.push_back(new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0, 0.0)));
        for (auto expr : arrs->getList())
            idxs.push_back(dynamic_cast<ExprNode*>(expr)->getOperand());
        Type *type;
        if (se->getType()->isArray()) {
            type = dynamic_cast<ArrayType*>(se->getType())->getBaseType(idxs.size()-2);
            dst = new Operand(new TemporarySymbolEntry(new PointerType(type), SymbolTable::getLabel(), 0, 0.0));
            Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr();
            new getElePtrInstruction(dst, addr, idxs, builder->getInsertBB());
            if (!temp_val) {
                Operand *dst_val = new Operand(new TemporarySymbolEntry(type, SymbolTable::getLabel(), 0, 0.0));
                new LoadInstruction(dst_val, dst, builder->getInsertBB());
                dst = dst_val;
            }
        }
        else {
            if (idxs.size() > 2)
                type = dynamic_cast<ArrayType*>(dynamic_cast<PointerType*>(se->getType())->getValueType())->getBaseType(idxs.size()-3);
            else
                type = dynamic_cast<PointerType*>(se->getType())->getValueType();
            Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(se)->getAddr();
            dst = new Operand(new TemporarySymbolEntry(new PointerType(type), SymbolTable::getLabel(), 0, 0.0));
            idxs.erase(idxs.begin());
            new getElePtrInstruction(dst, addr, idxs, builder->getInsertBB());
            if (!temp_val) {
                Operand *dst_val = new Operand(new TemporarySymbolEntry(type, SymbolTable::getLabel(), 0, 0.0));
                new LoadInstruction(dst_val, dst, builder->getInsertBB());
                dst = dst_val;
            }
        }
        isRParams = temp_rparams;
    }
    if (GENCODE_CHECK) printf("%*cLVal genCode passed!\n", check_depth--, ' ');
}

void Number::genCode()
{
    if (GENCODE_CHECK) printf("%*cNumber genCoding...\n", ++check_depth, ' ');
    dst = new Operand(se);
    if (GENCODE_CHECK) printf("%*cNumber genCode passed!\n", check_depth--, ' ');
}

void UnaryExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cUnaryExp genCoding...\n", ++check_depth, ' ');
    if (op == NON) {
        expr->genCode();
        dst = expr->getOperand();
    }
    else if (op == PLUS) {
        expr->genCode();
        dst = expr->getOperand();
    }
    else if (op == MINUS) {
        expr->genCode();
        Operand *src = expr->getOperand();
        dst = new Operand(se);
        Operand *dst_casted = dst, *src_casted = src;
        typeCast(&dst_casted, &src_casted, dst, src, builder->getInsertBB());
        Operand *zero = new Operand(new ConstantSymbolEntry(src_casted->getType(), 0, 0.0));
        dst = dst_casted;
        new BinaryInstruction(BinaryInstruction::SUB, dst_casted, zero, src_casted, builder->getInsertBB());
    }
    else if (op == NOT) {
        expr->genCode();
        Operand *src = expr->getOperand();
        dst = new Operand(se);
        Operand *zero = new Operand(new ConstantSymbolEntry(src->getType(), 0, 0.0));
        std::string type_str = src->getType()->toStr();
        if (type_str == "i1") {
            new BinaryInstruction(BinaryInstruction::XOR, dst, src, zero, builder->getInsertBB());
        }
        else {
            new CmpInstruction(CmpInstruction::E, dst, src, zero, builder->getInsertBB());
        }
    }
    if (GENCODE_CHECK) printf("%*cUnaryExp genCode passed!\n", check_depth--, ' ');
}

void MulExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cMulExp %s%s genCoding...\n", ++check_depth, ' ', expr1->getSymbolEntry()->toStr().c_str(), expr2 == nullptr ? "" : expr2->getSymbolEntry()->toStr().c_str());
    expr1->genCode();
    if (expr2 != nullptr) {
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        dst = new Operand(se);
        int opcode;
        switch (op)
        {
        case MULT:
            opcode = BinaryInstruction::MULT;
            break;
        case DIV:
            opcode = BinaryInstruction::DIV;
            break;
        case MOD:
            opcode = BinaryInstruction::MOD;
            break;
        default:
            opcode = -1;
            break;
        }
        Operand *src1_casted = src1, *src2_casted = src2;
        typeCast(&src1_casted, &src2_casted, src1, src2, builder->getInsertBB());
        new BinaryInstruction(opcode, dst, src1_casted, src2_casted, builder->getInsertBB());        
    }
    else {
        dst = expr1->getOperand();
    }
    if (GENCODE_CHECK) printf("%*cMulExp %s%s genCode passed!\n", check_depth--, ' ', expr1->getSymbolEntry()->toStr().c_str(), expr2 == nullptr ? "" : expr2->getSymbolEntry()->toStr().c_str());
}

void AddExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cAddExp %s%s genCoding...\n", ++check_depth, ' ', expr1->getSymbolEntry()->toStr().c_str(), expr2 == nullptr ? "" : expr2->getSymbolEntry()->toStr().c_str());
    expr1->genCode();
    if (expr2 != nullptr) {
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        dst = new Operand(se);
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        default:
            opcode = -1;
            break;
        }
        Operand *src1_casted = src1, *src2_casted = src2;
        typeCast(&src1_casted, &src2_casted, src1, src2, builder->getInsertBB());
        new BinaryInstruction(opcode, dst, src1_casted, src2_casted, builder->getInsertBB());
    }
    else {
        dst = expr1->getOperand();
    }
    if (GENCODE_CHECK) printf("%*cAddExp %s%s genCode passed!\n", check_depth--, ' ', expr1->getSymbolEntry()->toStr().c_str(), expr2 == nullptr ? "" : expr2->getSymbolEntry()->toStr().c_str());
}

void RelExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cRelExp genCoding...\n", ++check_depth, ' ');
    // 查看过test文件，发现不会出现连续的关系运算
    expr1->genCode();
    if (expr2 != nullptr) {
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        dst = new Operand(se);
        int opcode;
        switch (op)
        {
        case LESS:
            opcode = CmpInstruction::L;
            break;
        case GREATER:
            opcode = CmpInstruction::G;
            break;
        case LEQUAL:
            opcode = CmpInstruction::LE;
            break;
        case GEQUAL:
            opcode = CmpInstruction::GE;
            break;
        default:
            opcode = -1;
            break;
        }
        if (!expr1->trueList().empty()) {
            builder->getInsertBB()->remove(builder->getInsertBB()->rbegin());
        }
        Operand *src1_casted = src1, *src2_casted = src2;
        typeCast(&src1_casted, &src2_casted, src1, src2, builder->getInsertBB());
        new CmpInstruction(opcode, dst, src1_casted, src2_casted, builder->getInsertBB());
        CondBrInstruction* condBr = new CondBrInstruction(nullptr, nullptr, dst, builder->getInsertBB());
        trueList().push_back(condBr->patchBranchTrue());
        falseList().push_back(condBr->patchBranchFalse());
        if (GENCODE_CHECK) {
            for (auto bb : trueList())
                printf("trueList: %p -> %p\n", bb, *bb);
            for (auto bb : falseList())
                printf("falseList: %p -> %p\n", bb, *bb);
        }
    }
    else {
        dst = expr1->getOperand();
        trueList() = expr1->trueList();
        falseList() = expr1->falseList();
    }
    if (GENCODE_CHECK) printf("%*cRelExp genCode passed!\n", check_depth--, ' ');
}

void EqExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cEqExp genCoding...\n", ++check_depth, ' ');
    expr1->genCode();
    if (expr2 != nullptr) {
        if (!expr1->trueList().empty()) {
            builder->getInsertBB()->remove(builder->getInsertBB()->rbegin());
        }
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        dst = new Operand(se);
        int opcode;
        switch (op)
        {
        case EQUAL:
            opcode = CmpInstruction::E;
            break;
        case NEQUAL:
            opcode = CmpInstruction::NE;
            break;
        default:
            opcode = -1;
            break;
        }
        if (!expr2->trueList().empty()) {
            builder->getInsertBB()->remove(builder->getInsertBB()->rbegin());
        }
        Operand *src1_casted = src1, *src2_casted = src2;
        typeCast(&src1_casted, &src2_casted, src1, src2, builder->getInsertBB());
        new CmpInstruction(opcode, dst, src1_casted, src2_casted, builder->getInsertBB());
        CondBrInstruction* condBr = new CondBrInstruction(nullptr, nullptr, dst, builder->getInsertBB());
        trueList().push_back(condBr->patchBranchTrue());
        falseList().push_back(condBr->patchBranchFalse());
    }
    else {
        dst = expr1->getOperand();
        trueList() = expr1->trueList();
        falseList() = expr1->falseList();
    }
    if (GENCODE_CHECK) {
        for (auto bb : trueList())
            printf("trueList: %p -> %p\n", bb, *bb);
        for (auto bb : falseList())
            printf("falseList: %p -> %p\n", bb, *bb);
    }
    if (GENCODE_CHECK) printf("%*cEqExp genCode passed!\n", check_depth--, ' ');
}

void LAndExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cLAndExp genCoding...\n", ++check_depth, ' ');
    if (expr2 != nullptr) {
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *true_bb = new BasicBlock(func);
        expr1->genCode();
        backPatch(expr1->trueList(), true_bb);
        builder->setInsertBB(true_bb);
        expr2->genCode();
        Operand *src = expr2->getOperand();
        std::string type_str = src->getType()->toStr();
        if (expr2->falseList().empty()) {
            if (type_str == "i1") {
                CondBrInstruction* condBr = new CondBrInstruction(nullptr, nullptr, src, builder->getInsertBB());
                expr2->trueList().push_back(condBr->patchBranchTrue());
                expr2->falseList().push_back(condBr->patchBranchFalse());
            }
            else {
                dst = new Operand(se);
                new typeCastInstruction(dst, src, builder->getInsertBB());
                CondBrInstruction* condBr = new CondBrInstruction(nullptr, nullptr, dst, builder->getInsertBB());
                expr2->trueList().push_back(condBr->patchBranchTrue());
                expr2->falseList().push_back(condBr->patchBranchFalse());
            }
        }
        trueList() = expr2->trueList();
        falseList() = merge(expr1->falseList(), expr2->falseList());
    }
    else {
        expr1->genCode();
        Operand *src = expr1->getOperand();
        std::string type_str = src->getType()->toStr();
        if (expr1->falseList().empty()) {
            if (type_str == "i1") {
                CondBrInstruction* condBr = new CondBrInstruction(nullptr, nullptr, src, builder->getInsertBB());
                expr1->trueList().push_back(condBr->patchBranchTrue());
                expr1->falseList().push_back(condBr->patchBranchFalse());
            }
            else {
                dst = new Operand(se);
                new typeCastInstruction(dst, src, builder->getInsertBB());
                CondBrInstruction* condBr = new CondBrInstruction(nullptr, nullptr, dst, builder->getInsertBB());
                expr1->trueList().push_back(condBr->patchBranchTrue());
                expr1->falseList().push_back(condBr->patchBranchFalse());
            }
        }
        trueList() = expr1->trueList();
        falseList() = expr1->falseList();
    }
    if (GENCODE_CHECK) {
        for (auto bb : trueList())
            printf("trueList: %p -> %p\n", bb, *bb);
        for (auto bb : falseList())
            printf("falseList: %p -> %p\n", bb, *bb);
    }
    if (GENCODE_CHECK) printf("%*cLAndExp genCode passed!\n", check_depth--, ' ');
}

void LOrExp::genCode()
{
    if (GENCODE_CHECK) printf("%*cLOrExp genCoding...\n", ++check_depth, ' ');
    if (expr2 != nullptr) {
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *false_bb = new BasicBlock(func);
        expr1->genCode();
        backPatch(expr1->falseList(), false_bb);
        builder->setInsertBB(false_bb);
        expr2->genCode();
        falseList() = expr2->falseList();
        trueList() = merge(expr1->trueList(), expr2->trueList());
    }
    else {
        expr1->genCode();
        trueList() = expr1->trueList();
        falseList() = expr1->falseList();
    }
    if (GENCODE_CHECK) {
        for (auto bb : trueList())
            printf("trueList: %p -> %p\n", bb, *bb);
        for (auto bb : falseList())
            printf("falseList: %p -> %p\n", bb, *bb);
    }
    if (GENCODE_CHECK) printf("%*cLOrExp genCode passed!\n", check_depth--, ' ');
}

void FuncCall::genCode()
{
    if (GENCODE_CHECK) printf("%*cFuncCall genCoding...\n", ++check_depth, ' ');
    std::vector<Operand*> args;
    if (funcRParams != nullptr) {
        isRParams = true;
        funcRParams->genCode();
        isRParams = false;
        for (auto expr : funcRParams->getList()) {
            args.push_back(dynamic_cast<ExprNode*>(expr)->getOperand());          
        }
      
    }
    Type* retType = dynamic_cast<FunctionType*>(funcSE->getType())->getRetType();
    if (retType->isVoid())
        dst = nullptr;
    else {
        dst = new Operand(new TemporarySymbolEntry(retType, SymbolTable::getLabel(), 0, 0.0));
    }
    new CallInstruction(dst, funcSE, args, builder->getInsertBB());
    auto it = sysyRunLibFuncs.find(dynamic_cast<IdentifierSymbolEntry*>(funcSE)->toStr());
    if (it != sysyRunLibFuncs.end()) {
        sysyRunLibFuncs.erase(it);
        new FuncDeclInstruction(funcSE);
    }
    if (GENCODE_CHECK) printf("%*cFuncCall genCode passed!\n", check_depth--, ' ');
}

void FuncDef::genCode()
{
    if (GENCODE_CHECK) printf("%*cFuncDef genCoding...\n", ++check_depth, ' ');
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    builder->setInsertBB(entry);
    if (funcFParams != nullptr) {
        funcFParams->genCode();
    }
    block->genCode();
    for (auto bb = func->begin(); bb != func->end(); bb++) {
        for (auto inst = (*bb)->begin(); inst != (*bb)->end(); inst = inst->getNext()) {
            if ((inst)->isCond()) {
                CondBrInstruction *condInst = dynamic_cast<CondBrInstruction*>(inst);
                BasicBlock *true_bb = condInst->getTrueBranch();
                BasicBlock *false_bb = condInst->getFalseBranch();
                if (GENCODE_CHECK && true_bb == nullptr) printf("true_bb is nullptr\n");
                if (GENCODE_CHECK && false_bb == nullptr) printf("false_bb is nullptr\n");
                (*bb)->addSucc(true_bb);
                (*bb)->addSucc(false_bb);
                true_bb->addPred(*bb);
                false_bb->addPred(*bb);
                inst->setNext((*bb)->end());
            }
            else if (inst->isUncond()) {
                UncondBrInstruction *uncondInst = dynamic_cast<UncondBrInstruction*>(inst);
                BasicBlock *succ_bb = uncondInst->getBranch();
                (*bb)->addSucc(succ_bb);
                succ_bb->addPred(*bb);
                inst->setNext((*bb)->end());
            }
            else if (inst->isRet()) {
                inst->setNext((*bb)->end());
            }
            Operand *def = inst->getDef();
            if (def != nullptr && !inst->isCall()) {
                if (def->usersNum() == 0) {
                    Instruction *next = inst->getNext();
                    (*bb)->remove(inst);
                    inst = next;
                }
            }
        }
        if ((*bb)->empty()) {
            Type *retType = dynamic_cast<FunctionType*>(se->getType())->getRetType();
            if (retType->isVoid())
                new RetInstruction(nullptr, *bb);
            else if (retType->isInt()) {
                Operand *zero = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0, 0.0));
                new RetInstruction(zero, *bb);
            }
            else {
                Operand *zero = new Operand(new ConstantSymbolEntry(TypeSystem::floatType, 0, 0.0));
                new RetInstruction(zero, *bb);
            }
            continue;
        }
        if (!(*bb)->rbegin()->isRet() && !(*bb)->rbegin()->isCond() && !(*bb)->rbegin()->isUncond())
            new RetInstruction(nullptr, *bb);
    }
    // if (func->getBlockList().size() == 1 && !entry->rbegin()->isRet()) {
    //     new RetInstruction(nullptr, entry);
    // }
    if (GENCODE_CHECK) printf("%*cFuncDef genCode passed!\n", check_depth--, ' ');
}

void ConstDef::genCode()
{
    if (GENCODE_CHECK) printf("%*cConstDef genCoding...\n", ++check_depth, ' ');
    if (dynamic_cast<IdentifierSymbolEntry*>(se)->isGlobal()) {
        SymbolEntry *addr_se = new IdentifierSymbolEntry(*dynamic_cast<IdentifierSymbolEntry*>(se));
        addr_se->setType(new PointerType(se->getType()));
        dynamic_cast<IdentifierSymbolEntry*>(se)->setAddr(new Operand(addr_se));
        std::vector<Operand*> idxs;
        if (initValue != nullptr) {
            initValue->genCode();
            if (se->getType()->isArray()) {
                std::vector<int> dims = dynamic_cast<ArrayType*>(se->getType())->getSizeList();
                Type *baseType = dynamic_cast<ArrayType*>(se->getType())->getBaseType(dims.size()-1);
                int size = 1;
                for (auto dim : dims) size *= dim;
                if (baseType->isInt()) {
                    int *array = new int[size]();
                    initVal2IntArray(array, dims, initValue);
                    idxs = intArray2OpVector(array, size);                    
                }
                else {
                    float *array = new float[size]();
                    initVal2FloatArray(array, dims, initValue);
                    idxs = floatArray2OpVector(array, size);
                }
            }
            else {
                SymbolEntry *valueSE = dynamic_cast<ExprNode*>(initValue->getList()[0])->getSymbolEntry();
                Operand *value = new Operand(new ConstantSymbolEntry(valueSE->getType(), valueSE->getIntValue(), valueSE->getFloatValue()));
                idxs.push_back(value);
            }
            new GlobalVarInstruction(new Operand(se), idxs);
        }
        else {
            new GlobalVarInstruction(new Operand(se), idxs);
        }
    }
    else if (dynamic_cast<IdentifierSymbolEntry*>(se)->isParam()) {
        ;
    }
    else if (dynamic_cast<IdentifierSymbolEntry*>(se)->isLocal()) {
        Function *func = builder->getInsertBB()->getParent();
        SymbolEntry *addr_se = new TemporarySymbolEntry(new PointerType(se->getType()), SymbolTable::getLabel(), 0, 0.0);
        Operand *addr = new Operand(addr_se);
        dynamic_cast<IdentifierSymbolEntry*>(se)->setAddr(addr);
        func->getEntry()->insertFront(new AllocaInstruction(addr, se));
        if (se->getType()->isArray()) {
            if (initValue != nullptr) {
                initValue->genCode();
                std::vector<int> dims = dynamic_cast<ArrayType*>(se->getType())->getSizeList();
                int size = 1;
                for (auto dim : dims) size *= dim;
                Operand **src = new Operand*[size]();
                initVal2OpPtrArray(src, dims, initValue);
                initArraybyOpPtr(addr, src, dims, builder->getInsertBB());
            }
        }
        else {
            if (initValue != nullptr) {
                initValue->genCode();
                Operand *src = dynamic_cast<ExprNode*>(initValue->getList()[0])->getOperand();
                new StoreInstruction(addr, src, builder->getInsertBB());
            }
        }
    }
    if (GENCODE_CHECK) printf("%*cConstDef genCode passed!\n", check_depth--, ' ');
}

void VarDef::genCode()
{
    if (GENCODE_CHECK) printf("%*cVarDef genCoding...\n", ++check_depth, ' ');
    if (dynamic_cast<IdentifierSymbolEntry*>(se)->isGlobal()) {
        SymbolEntry *addr_se = new IdentifierSymbolEntry(*dynamic_cast<IdentifierSymbolEntry*>(se));
        addr_se->setType(new PointerType(se->getType()));
        dynamic_cast<IdentifierSymbolEntry*>(se)->setAddr(new Operand(addr_se));
        std::vector<Operand*> idxs;
        if (initValue != nullptr && initValue->getList().size() != 0) {
            initValue->genCode();
            if (se->getType()->isArray()) {
                std::vector<int> dims = dynamic_cast<ArrayType*>(se->getType())->getSizeList();
                Type *baseType = dynamic_cast<ArrayType*>(se->getType())->getBaseType(dims.size()-1);
                int size = 1;
                for (auto dim : dims) size *= dim;
                if (baseType->isInt()) {
                    int *array = new int[size]();
                    initVal2IntArray(array, dims, initValue);
                    idxs = intArray2OpVector(array, size);                    
                }
                else {
                    float *array = new float[size]();
                    initVal2FloatArray(array, dims, initValue);
                    idxs = floatArray2OpVector(array, size);
                }
            }
            else {
                SymbolEntry *valueSE = dynamic_cast<ExprNode*>(initValue->getList()[0])->getSymbolEntry();
                Operand *value = new Operand(new ConstantSymbolEntry(valueSE->getType(), valueSE->getIntValue(), valueSE->getFloatValue()));
                idxs.push_back(value);
            }
            new GlobalVarInstruction(new Operand(se), idxs);
        }
        else {
            new GlobalVarInstruction(new Operand(se), idxs);
        }
    }
    else if (dynamic_cast<IdentifierSymbolEntry*>(se)->isParam()) {
        ;
    }
    else if (dynamic_cast<IdentifierSymbolEntry*>(se)->isLocal()) {
        Function *func = builder->getInsertBB()->getParent();
        SymbolEntry *addr_se = new TemporarySymbolEntry(new PointerType(se->getType()), SymbolTable::getLabel(), 0, 0.0);
        Operand *addr = new Operand(addr_se);
        dynamic_cast<IdentifierSymbolEntry*>(se)->setAddr(addr);
        func->getEntry()->insertFront(new AllocaInstruction(addr, se));
        if (se->getType()->isArray()) {
            if (initValue != nullptr) {
                initValue->genCode();
                std::vector<int> dims = dynamic_cast<ArrayType*>(se->getType())->getSizeList();
                int size = 1;
                for (auto dim : dims) size *= dim;
                Operand **src = new Operand*[size]();
                initVal2OpPtrArray(src, dims, initValue);
                initArraybyOpPtr(addr, src, dims, builder->getInsertBB());
            }
        }
        else {
            if (initValue != nullptr) {
                initValue->genCode();
                Operand *src = dynamic_cast<ExprNode*>(initValue->getList()[0])->getOperand();
                new StoreInstruction(addr, src, builder->getInsertBB());
            }
        }
    }
    if (GENCODE_CHECK) printf("%*cVarDef genCode passed!\n", check_depth--, ' ');
}

void typeCast(Operand **dst1, Operand **dst2, Operand *src1, Operand *src2, BasicBlock *insert_bb)
{
    if (GENCODE_CHECK && (src1 == nullptr || src2 == nullptr)) printf("src1 or src2 is nullptr\n");
    if (GENCODE_CHECK && (src1->getType() == nullptr || src2->getType() == nullptr)) printf("src1->getType() or src2->getType() is nullptr\n");
    std::string src1_str = src1->getType()->toStr();
    std::string src2_str = src2->getType()->toStr();

    if (src1_str == "i1" && src2_str == "i32") {
        *dst1 = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0));
        new typeCastInstruction(*dst1, src1, insert_bb);
    }
    else if (src1_str == "i32" && src2_str == "i1") {
        *dst2 = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0));
        new typeCastInstruction(*dst2, src2, insert_bb);
    }
    else if (src1_str == "float" && src2_str == "i32") {
        *dst2 = new Operand(new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0));
        new typeCastInstruction(*dst2, src2, insert_bb);
    }
    else if (src1_str == "i32" && src2_str == "float") {
        *dst1 = new Operand(new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0));
        new typeCastInstruction(*dst1, src1, insert_bb);
    }
    else if (src1_str == "float" && src2_str == "i1") {
        *dst1 = new Operand(new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0));
        new typeCastInstruction(*dst1, src1, insert_bb);
    }
    else if (src1_str == "i1" && src2_str == "float") {
        *dst2 = new Operand(new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0));
        new typeCastInstruction(*dst2, src2, insert_bb);
    }
}

void initVal2IntArray(int* array, std::vector<int> dims, Node* initVal) {
    std::vector<int> nextDims(dims);
    nextDims.erase(nextDims.begin());
    int row_size = 1;
    for (auto dim : nextDims) row_size *= dim;
    std::vector<Node*> list = ((ListNode*)initVal)->getList();
    if (nextDims.size() == 0) {
        for (int i = 0; i < (int)list.size(); i++) {
            array[i] = dynamic_cast<ExprNode*>(dynamic_cast<ListNode*>(list[i])->getList()[0])->getSymbolEntry()->getIntValue();
        }
        return;
    }
    int count1 = 0, count2 = 0, it = 0;
    while (count1 < dims[0]) {
        InitVal nextListNode;
        count2 = row_size;
        while (true) {
            if (it >= (int)list.size()) {
                initVal2IntArray(array + count1++ * row_size, nextDims, &nextListNode);
                count1 = dims[0];
                break;
            }
            if (list[it]->isList()) {
                if (nextListNode.getList().size() == 0)
                    initVal2IntArray(array + count1++ * row_size, nextDims, list[it++]);
                else
                    initVal2IntArray(array + count1++ * row_size, nextDims, &nextListNode);
                break;
            }
            if (!count2--) {
                initVal2IntArray(array + count1++ * row_size, nextDims, &nextListNode);
                break;
            }
            nextListNode.add(list[it++]);
        }
    }
}

void initVal2FloatArray(float* array, std::vector<int> dims, Node* initVal) {
    std::vector<int> nextDims(dims);
    nextDims.erase(nextDims.begin());
    int row_size = 1;
    for (auto dim : nextDims) row_size *= dim;
    std::vector<Node*> list = ((ListNode*)initVal)->getList();
    if (nextDims.size() == 0) {
        for (int i = 0; i < (int)list.size(); i++)
            array[i] = dynamic_cast<ExprNode*>(dynamic_cast<ListNode*>(list[i])->getList()[0])->getSymbolEntry()->getFloatValue();
        return;
    }
    int count1 = 0, count2 = 0, it = 0;
    while (count1 < dims[0]) {
        InitVal nextListNode;
        count2 = row_size;
        while (true) {
            if (it >= (int)list.size()) {
                initVal2FloatArray(array + count1++ * row_size, nextDims, &nextListNode);
                count1 = dims[0];
                break;
            }
            if (list[it]->isList()) {
                if (nextListNode.getList().size() == 0)
                    initVal2FloatArray(array + count1++ * row_size, nextDims, list[it++]);
                else
                    initVal2FloatArray(array + count1++ * row_size, nextDims, &nextListNode);
                break;
            }
            if (!count2--) {
                initVal2FloatArray(array + count1++ * row_size, nextDims, &nextListNode);
                break;
            }
            nextListNode.add(list[it++]);
        }
    }
}

void initVal2OpPtrArray(Operand** array, std::vector<int> dims, Node* initVal) {
    std::vector<int> nextDims(dims);
    nextDims.erase(nextDims.begin());
    int row_size = 1;
    for (auto dim : nextDims) row_size *= dim;
    std::vector<Node*> list = ((ListNode*)initVal)->getList();
    if (nextDims.size() == 0) {
        for (int i = 0; i < (int)list.size(); i++)
            if (list[i]->isList())
                array[i] = ((ExprNode*)((ListNode*)list[i])->getList()[0])->getOperand();
            else
                array[i] = ((ExprNode*)list[i])->getOperand();
        return;
    }
    int count1 = 0, count2 = 0, it = 0;
    while (count1 < dims[0]) {
        InitVal nextListNode;
        count2 = row_size;
        while (true) {
            if (it >= (int)list.size()) {
                initVal2OpPtrArray(array + count1++ * row_size, nextDims, &nextListNode);
                count1 = dims[0];
                break;
            }
            if (list[it]->isList() && !((ListNode*)list[it])->getList().empty() && ((ListNode*)list[it])->getList()[0]->isList()) {
                if (nextListNode.getList().size() == 0)
                    initVal2OpPtrArray(array + count1++ * row_size, nextDims, list[it++]);
                else
                    initVal2OpPtrArray(array + count1++ * row_size, nextDims, &nextListNode);
                break;
            }
            if (!count2--) {
                initVal2OpPtrArray(array + count1++ * row_size, nextDims, &nextListNode);
                break;
            }
            if (list[it]->isList()) {
                if (((ListNode*)list[it])->getList().size() == 1)
                    nextListNode.add(((ListNode*)list[it])->getList()[0]);
                if (((ListNode*)list[it])->getList().size() == 0)
                    count1++;
                it++;
            }
            else
                nextListNode.add(list[it++]);
        }
    }
}

std::vector<Operand*> intArray2OpVector(int *array, int size) {
    std::vector<Operand*> ret;
    for (int i = 0; i < size; i++) {
        ret.push_back(new Operand(new ConstantSymbolEntry(TypeSystem::intType, array[i], 0.0)));
    }
    return ret;
}

std::vector<Operand*> floatArray2OpVector(float *array, int size) {
    std::vector<Operand*> ret;
    for (int i = 0; i < size; i++) {
        ret.push_back(new Operand(new ConstantSymbolEntry(TypeSystem::floatType, 0, array[i])));
    }
    return ret;
}

void initArraybyOpPtr(Operand *addr, Operand **src, std::vector<int> dims, BasicBlock *insert_bb) {
    int size = 1;
    for (auto dim : dims) size *= dim;
    for (int i = 0; i < size; i++) {
        if (src[i] != nullptr) {
            std::vector<Operand*> idxs;
            int temp = i;
            for (int j = dims.size()-1; j >= 0 ; j--) {
                idxs.insert(idxs.begin(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, temp % dims[j], 0.0)));
                temp /= dims[j];
            }
            idxs.insert(idxs.begin(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0, 0.0)));
            Type* baseType = dynamic_cast<ArrayType*>(dynamic_cast<PointerType*>(addr->getType())->getValueType())->getBaseType(dims.size()-1);
            Operand *dst_addr = new Operand(new TemporarySymbolEntry(new PointerType(baseType), SymbolTable::getLabel(), 0, 0.0));
            new getElePtrInstruction(dst_addr, addr, idxs, insert_bb);
            new StoreInstruction(dst_addr, src[i], insert_bb);
        }
    }
}