%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
    #include <vector>
}

%{
    /* 自定义内容 */
    #ifndef AST_CHECK
    #define AST_CHECK false
    #endif
    char outfile[256] = "a.out";
    std::vector<Type*> typeStack;
    bool inFuncDef = false;
    bool inConstDecl = false;
%}

%union {
    int itype;
    float ftype;
    char* strtype;
    ListNode* listtype;
    TypeNode* typetype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    FuncDefNode* funcdeftype;
    VarDefNode* vardeftype;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token <ftype> FLOATEGER
%token IF ELSE
%token INT FLOAT VOID
%token CONST RETURN WHILE BREAK CONTINUE
%token LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
%token COMMA SEMICOLON
%token EQUAL LEQUAL GEQUAL NEQUAL LESS GREATER
%token ADD SUB MULT DIV MOD AND OR NOT ASSIGN

%nterm <listtype> CompUnit ConstDefs ConstInitVal ConstInitVals VarDefs InitVal InitVals FuncFParams Arrays BlockItems FuncRParams
%nterm <typetype> BType ConstArrays
%nterm <stmttype> Decl ConstDecl VarDecl Block BlockItem Stmt IfStmt
%nterm <exprtype> ConstExp FuncFParam Exp Cond LVal PrimaryExp Number UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp
%nterm <funcdeftype> FuncDef
%nterm <vardeftype> ConstDef VarDef


/*
%nterm <nodetype> CompUnit Decl FuncDef Arrays
%nterm <type> BType ConstArrays
%nterm <nodetype> ConstExp Exp Cond LVal PrimaryExp Number
%nterm <nodetype> UnaryExp MulExp AddExp
%nterm <nodetype> RelExp EqExp LAndExp LOrExp
%nterm <nodetype> ConstDecl ConstDefs ConstDef ConstInitVal ConstInitVals 
%nterm <nodetype> VarDecl VarDefs VarDef InitVal InitVals
%nterm <nodetype> FuncFParams FuncFParam FuncRParams
%nterm <nodetype> Block BlockItems BlockItem Stmt IfStmt
*/

/*
%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp
%nterm <type> Type
*/

%precedence THEN
%precedence ELSE
%%
Program
    : CompUnit {
        if (AST_CHECK) printf("Program -> CompUnit\n");
        ast.setRoot($1);
    }
    ;

CompUnit
    : Decl {
        if (AST_CHECK) printf("CompUnit -> Decl\n");
        $$ = new CompUnit();
        $$->add($1);
    }
    | FuncDef {
        if (AST_CHECK) printf("CompUnit -> FuncDef\n");
        $$ = new CompUnit();
        $$->add($1);
    }
    | CompUnit Decl {
        if (AST_CHECK) printf("CompUnit -> CompUnit Decl\n");
        $$ = $1;
        $$->add($2);
    }
    | CompUnit FuncDef {
        if (AST_CHECK) printf("CompUnit -> CompUnit FuncDef\n");
        $$ = $1;
        $$->add($2);
    }
    ;

Decl
    : ConstDecl {
        if (AST_CHECK) printf("Decl -> ConstDecl\n");
        $$ = new Decl($1);
    }
    | VarDecl {
        if (AST_CHECK) printf("Decl -> VarDecl\n");
        $$ = new Decl($1);
    }
    ;

ConstDecl
    : CONST {
        inConstDecl = true;
    } BType ConstDefs SEMICOLON {
        if (AST_CHECK) printf("ConstDecl -> CONST BType ConstDefs SEMICOLON\n");
        $$ = new ConstDecl($4);
        typeStack.pop_back();
        inConstDecl = false;
    }
    ;

BType
    : INT {
        if (AST_CHECK) printf("BType -> INT\n");
        if (inConstDecl)
            typeStack.push_back(TypeSystem::constIntType);
        else
            typeStack.push_back(TypeSystem::intType);
    }
    | FLOAT {
        if (AST_CHECK) printf("BType -> FLOAT\n");
        if (inConstDecl)
            typeStack.push_back(TypeSystem::constFloatType);
        else
            typeStack.push_back(TypeSystem::floatType);
    }
    ;

ConstDefs
    : ConstDef {
        if (AST_CHECK) printf("ConstDefs -> ConstDef\n");
        $$ = new ConstDefs();
        $$->add($1);
    }
    | ConstDefs COMMA ConstDef {
        if (AST_CHECK) printf("ConstDefs -> ConstDefs COMMA ConstDef\n");
        $$ = $1;
        $$->add($3);
    }
    ;

ConstDef
    : ID ASSIGN ConstInitVal {
        if (AST_CHECK) printf("ConstDef -> ID ASSIGN ConstInitVal(ID: %s)\n", $1);
        SymbolEntry *se, *exprSE = ((ExprNode*)($3->getList()[0]))->getSymbolEntry();
        if (typeStack.back()->isInt())
            se = new IdentifierSymbolEntry(typeStack.back(), $1, identifiers->getLevel(), exprSE->getIntValue(), 0.0, $3);
        else
            se = new IdentifierSymbolEntry(typeStack.back(), $1, identifiers->getLevel(), 0, exprSE->getIntValue(), $3);
        identifiers->install($1, se);
        $$ = new ConstDef(se, $3);  // 这里将$3也传入实际是与se重复了，本着尽量不修改已完善代码的原则，这里不进行改动了
    }
    | ID ConstArrays ASSIGN ConstInitVal {
        if (AST_CHECK) printf("ConstDef -> ID ConstArrays ASSIGN ConstInitVal(ID: %s)\n", $1);
        SymbolEntry *se = new IdentifierSymbolEntry($2->getType(), $1, identifiers->getLevel(), 0, 0.0, $4);
        identifiers->install($1, se);
        $$ = new ConstDef(se, $4);
    }
    ;

ConstArrays
    : LBRACK ConstExp RBRACK {
        if (AST_CHECK) printf("ConstArrays -> LBRACK ConstExp RBRACK\n");
        Type *newType = new ArrayType(typeStack.back(), $2->getSymbolEntry()->getIntValue());
        $$ = new ConstArrays(newType);
    }
    | ConstArrays LBRACK ConstExp RBRACK {
        if (AST_CHECK) printf("ConstArrays -> ConstArrays LBRACK ConstExp RBRACK\n");
        $$ = $1;
        Type *newType = new ArrayType(typeStack.back(), $3->getSymbolEntry()->getIntValue());
        ((ArrayType*)$$)->setBaseType(newType);
    }
    ;

ConstInitVal
    : ConstExp {
        if (AST_CHECK) printf("ConstInitVal -> ConstExp\n");
        $$ = new ConstInitVal();
        $$->add($1);
    }
    | LBRACE RBRACE {
        if (AST_CHECK) printf("ConstInitVal -> LBRACE RBRACE\n");
        $$ = new ConstInitVal();
    }
    | LBRACE ConstInitVals RBRACE {
        if (AST_CHECK) printf("ConstInitVal -> LBRACE ConstInitVals RBRACE\n");
        $$ = $2;
    }
    ;

ConstInitVals
    : ConstInitVal {
        if (AST_CHECK) printf("ConstInitVals -> ConstInitVal\n");
        $$ = new ConstInitVals();
        $$->add($1);
    }
    | ConstInitVals COMMA ConstInitVal {
        if (AST_CHECK) printf("ConstInitVals -> ConstInitVals COMMA ConstInitVal\n");
        $$ = $1;
        $$->add($3);
    }
    ;

ConstExp
    : AddExp {
        if (AST_CHECK) printf("ConstExp -> AddExp\n");
        $$ = new ConstExp($1);
    }
    ;

VarDecl
    : BType VarDefs SEMICOLON {
        if (AST_CHECK) printf("VarDecl -> BType VarDefs SEMICOLON\n");
        $$ = new VarDecl($2);
        typeStack.pop_back();
    }
    ;

VarDefs
    : VarDef {
        if (AST_CHECK) printf("VarDefs -> VarDef\n");
        $$ = new VarDefs();
        $$->add($1);
    }
    | VarDefs COMMA VarDef {
        if (AST_CHECK) printf("VarDefs -> VarDefs COMMA VarDef\n");
        $$ = $1;
        $$->add($3);
    }
    ;

VarDef
    : ID {
        if (AST_CHECK) printf("VarDef -> ID(ID: %s)\n", $1);
        SymbolEntry *se = new IdentifierSymbolEntry(typeStack.back(), $1, identifiers->getLevel(), 0, 0.0, nullptr);
        identifiers->install($1, se);
        $$ = new VarDef(se, nullptr);
    }
    | ID ASSIGN InitVal {
        if (AST_CHECK) printf("VarDef -> ID ASSIGN InitVal(ID: %s)\n", $1);
        SymbolEntry *se, *exprSE = ((ExprNode*)($3->getList()[0]))->getSymbolEntry();
        if (typeStack.back()->isInt())
            se = new IdentifierSymbolEntry(typeStack.back(), $1, identifiers->getLevel(), exprSE->getIntValue(), 0.0, $3);
        else
            se = new IdentifierSymbolEntry(typeStack.back(), $1, identifiers->getLevel(), 0, exprSE->getIntValue(), $3);
        identifiers->install($1, se);
        $$ = new VarDef(se, $3);
    }
    | ID ConstArrays {
        if (AST_CHECK) printf("VarDef -> ID ConstArrays(ID: %s)\n", $1);
        SymbolEntry *se = new IdentifierSymbolEntry($2->getType(), $1, identifiers->getLevel(), 0, 0.0, nullptr);
        identifiers->install($1, se);
        $$ = new ConstDef(se, nullptr);
    }
    | ID ConstArrays ASSIGN InitVal {
        if (AST_CHECK) printf("VarDef -> ID ConstArrays ASSIGN InitVal(ID: %s)\n", $1);
        SymbolEntry *se = new IdentifierSymbolEntry($2->getType(), $1, identifiers->getLevel(), 0, 0.0, $4);
        identifiers->install($1, se);
        $$ = new ConstDef(se, $4);
    }
    ;

InitVal
    : Exp {
        if (AST_CHECK) printf("InitVal -> Exp\n");
        $$ = new InitVal();
        $$->add($1);
    }
    | LBRACE RBRACE {
        if (AST_CHECK) printf("InitVal -> LBRACE RBRACE\n");
        $$ = new InitVal();
    }
    | LBRACE InitVals RBRACE {
        if (AST_CHECK) printf("InitVal -> LBRACE InitVals RBRACE\n");
        $$ = $2;
    }
    ;

InitVals
    : InitVal {
        if (AST_CHECK) printf("InitVals -> InitVal\n");
        $$ = new InitVals();
        $$->add($1);
    }
    | InitVals COMMA InitVal {
        if (AST_CHECK) printf("InitVals -> InitVals COMMA InitVal\n");
        $$ = $1;
        $$->add($3);
    }
    ;

FuncDef
    : BType ID LPAREN {
        identifiers = new SymbolTable(identifiers);
    } RPAREN {
        Type *newType = new FunctionType(typeStack.back(), {});
        SymbolEntry *se = new IdentifierSymbolEntry(newType, $2, 0, 0, 0.0, nullptr);
        globals->install($2, se);
    } Block {
        if (AST_CHECK) printf("FuncDef -> BType ID LPAREN RPAREN Block(ID: %s)\n", $2);
        $$ = new FuncDef(globals->lookup($2), $7, nullptr);
        typeStack.pop_back();
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | BType ID LPAREN {
        identifiers = new SymbolTable(identifiers);
    } FuncFParams RPAREN {
        Type *newType = new FunctionType(typeStack.back(), ((FuncFParams*)$5)->getParamsType());
        SymbolEntry *se = new IdentifierSymbolEntry(newType, $2, 0, 0, 0.0, nullptr);
        globals->install($2, se);
    } Block {
        if (AST_CHECK) printf("FuncDef -> BType ID LPAREN FuncFParams RPAREN Block(ID: %s)\n", $2);
        $$ = new FuncDef(globals->lookup($2), $8, $5);
        typeStack.pop_back();
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | VOID ID LPAREN {
        identifiers = new SymbolTable(identifiers);
    } RPAREN {
        Type *newType = new FunctionType(TypeSystem::voidType, {});
        SymbolEntry *se = new IdentifierSymbolEntry(newType, $2, 0, 0, 0.0, nullptr);
        globals->install($2, se);
    } Block {
        if (AST_CHECK) printf("FuncDef -> VOID ID LPAREN RPAREN Block(ID: %s)\n", $2);
        $$ = new FuncDef(globals->lookup($2), $7, nullptr);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | VOID ID LPAREN {
        identifiers = new SymbolTable(identifiers);
    } FuncFParams RPAREN {
        Type *newType = new FunctionType(TypeSystem::voidType, ((FuncFParams*)$5)->getParamsType());
        SymbolEntry *se = new IdentifierSymbolEntry(newType, $2, 0, 0, 0.0, nullptr);
        globals->install($2, se);
    } Block {
        if (AST_CHECK) printf("FuncDef -> VOID ID LPAREN FuncFParams RPAREN Block(ID: %s)\n", $2);
        $$ = new FuncDef(globals->lookup($2), $8, $5);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    ;

FuncFParams
    : FuncFParam {
        if (AST_CHECK) printf("FuncFParams -> FuncFParam\n");
        $$ = new FuncFParams();
        $$->add($1);
    }
    | FuncFParams COMMA FuncFParam {
        if (AST_CHECK) printf("FuncFParams -> FuncFParams COMMA FuncFParam\n");
        $$ = $1;
        $$->add($3);
    }
    ;

FuncFParam
    : BType ID {
        if (AST_CHECK) printf("FuncFParam -> BType ID(ID: %s)\n", $2);
        SymbolEntry *se = new IdentifierSymbolEntry(typeStack.back(), $2, identifiers->getLevel(), 0, 0.0, nullptr);
        identifiers->install($2, se);
        $$ = new FuncFParam(se);
        typeStack.pop_back();
    }
    | BType ID LBRACK RBRACK {
        if (AST_CHECK) printf("FuncFParam -> BType ID LBRACK RBRACK(ID: %s)\n", $2);
        Type *newType = new ArrayType(typeStack.back(), -1);
        SymbolEntry *se = new IdentifierSymbolEntry(newType, $2, identifiers->getLevel(), 0, 0.0, nullptr);
        identifiers->install($2, se);
        $$ = new FuncFParam(se);
        typeStack.pop_back();
    }
    | BType ID LBRACK RBRACK ConstArrays {
        if (AST_CHECK) printf("FuncFParam -> BType ID LBRACK RBRACK ConstArrays(ID: %s)\n", $2);
        Type *newType = new ArrayType($5->getType(), -1);
        SymbolEntry *se = new IdentifierSymbolEntry(newType, $2, identifiers->getLevel(), 0, 0.0, nullptr);
        identifiers->install($2, se);
        $$ = new FuncFParam(se);
        typeStack.pop_back();
    }
    ;

Arrays
    : LBRACK Exp RBRACK {
        if (AST_CHECK) printf("Arrays -> LBRACK Exp RBRACK\n");
        $$ = new Arrays();
        $$->add($2);
    }
    | Arrays LBRACK Exp RBRACK {
        if (AST_CHECK) printf("Arrays -> Arrays LBRACK Exp RBRACK\n");
        $$ = $1;
        $$->add($3);
    }
    ;

Block
    : LBRACE {
        identifiers = new SymbolTable(identifiers);
    } RBRACE {
        if (AST_CHECK) printf("Block -> LBRACE RBRACE\n");
        $$ = new Block(nullptr);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    | LBRACE {
        identifiers = new SymbolTable(identifiers);
    } BlockItems RBRACE {
        if (AST_CHECK) printf("Block -> LBRACE BlockItems RBRACE\n");
        $$ = new Block($3);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    ;

BlockItems
    : BlockItem {
        if (AST_CHECK) printf("BlockItems -> BlockItem\n");
        $$ = new BlockItems();
        $$->add($1);
    }
    | BlockItems BlockItem {
        if (AST_CHECK) printf("BlockItems -> BlockItems BlockItem\n");
        $$ = $1;
        $$->add($2);
    }
    ;

BlockItem
    : Decl {
        if (AST_CHECK) printf("BlockItem -> Decl\n");
        $$ = new BlockItem($1);
    }
    | Stmt {
        if (AST_CHECK) printf("BlockItem -> Stmt\n");
        $$ = new BlockItem($1);
    }
    ;

Stmt
    : LVal ASSIGN Exp SEMICOLON {
        if (AST_CHECK) printf("Stmt -> LVal ASSIGN Exp SEMICOLON\n");
        $$ = new Stmt(new AssignStmt($1, $3));
    }
    | SEMICOLON {
        if (AST_CHECK) printf("Stmt -> SEMICOLON\n");
        $$ = new Stmt(new ExprStmt(nullptr));
    }
    | Exp SEMICOLON {
        if (AST_CHECK) printf("Stmt -> Exp SEMICOLON\n");
        $$ = new Stmt(new ExprStmt($1));
    }
    | Block {
        if (AST_CHECK) printf("Stmt -> Block\n");
        $$ = new Stmt($1);
    }
    | IfStmt {
        if (AST_CHECK) printf("Stmt -> IfStmt\n");
        $$ = new Stmt($1);
    }
    | WHILE LPAREN Cond RPAREN Stmt {
        if (AST_CHECK) printf("Stmt -> WHILE LPAREN Cond RPAREN Stmt\n");
        $$ = new Stmt(new WhileStmt($3, $5));
    }
    | BREAK SEMICOLON {
        if (AST_CHECK) printf("Stmt -> BREAK SEMICOLON\n");
        $$ = new Stmt(new BreakStmt());
    }
    | CONTINUE SEMICOLON {
        if (AST_CHECK) printf("Stmt -> CONTINUE SEMICOLON\n");
        $$ = new Stmt(new ContinueStmt());
    }
    | RETURN SEMICOLON {
        if (AST_CHECK) printf("Stmt -> RETURN SEMICOLON\n");
        $$ = new Stmt(new ReturnStmt(nullptr));
    }
    | RETURN Exp SEMICOLON {
        if (AST_CHECK) printf("Stmt -> RETURN Exp SEMICOLON\n");
        $$ = new Stmt(new ReturnStmt($2));
    }
    ;

IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        if (AST_CHECK) printf("IfStmt -> IF LPAREN Cond RPAREN Stmt %%prec THEN\n");
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        if (AST_CHECK) printf("IfStmt -> IF LPAREN Cond RPAREN Stmt ELSE Stmt\n");
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;

Exp
    : AddExp {
        if (AST_CHECK) printf("Exp -> AddExp\n");
        $$ = new Exp($1);
    }
    ;

Cond
    : LOrExp {
        if (AST_CHECK) printf("Cond -> LOrExp\n");
        $$ = new Cond($1);
    }
    ;

LVal
    : ID {
        if (AST_CHECK) printf("LVal -> ID(ID: %s)\n", $1);
        $$ = new LVal(identifiers->lookup($1), nullptr);
    }
    | ID Arrays {
        if (AST_CHECK) printf("LVal -> ID Arrays(ID: %s)\n", $1);
        $$ = new LVal(identifiers->lookup($1), $2);
    }
    ;

PrimaryExp
    : LPAREN Exp RPAREN {
        if (AST_CHECK) printf("PrimaryExp -> LPAREN Exp RPAREN\n");
        $$ = new PrimaryExp($2);
    }
    | LVal {
        if (AST_CHECK) printf("PrimaryExp -> LVal\n");
        $$ = new PrimaryExp($1);
    }
    | Number {
        if (AST_CHECK) printf("PrimaryExp -> Number\n");
        $$ = new PrimaryExp($1);
    }
    ;

Number
    : INTEGER {
        if (AST_CHECK) printf("Number -> INTEGER\n");
        $$ = new Number(new ConstantSymbolEntry(TypeSystem::constIntType, $1, 0.0));
    }
    | FLOATEGER {
        if (AST_CHECK) printf("Number -> FLOATEGER\n");
        $$ = new Number(new ConstantSymbolEntry(TypeSystem::constFloatType, 0, $1));
    }
    ;

UnaryExp
    : PrimaryExp {
        if (AST_CHECK) printf("UnaryExp -> PrimaryExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new UnaryExp(se, UnaryExp::NON, $1);
    }
    | ID LPAREN RPAREN {
        if (AST_CHECK) printf("UnaryExp -> ID LPAREN RPAREN(ID: %s)\n", $1);
        SymbolEntry *se, *funcSE = identifiers->lookup($1);
        Type* retType = ((FunctionType*)funcSE->getType())->getRetType();
        se = new TemporarySymbolEntry(retType, SymbolTable::getLabel(), 0, 0.0);
        FuncCall *funcCall = new FuncCall(se, funcSE, nullptr);
        se = new TemporarySymbolEntry(retType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new UnaryExp(se, UnaryExp::NON, funcCall);
    }
    | ID LPAREN FuncRParams RPAREN {
        if (AST_CHECK) printf("UnaryExp -> ID LPAREN FuncRParams RPAREN(ID: %s)\n", $1);
        SymbolEntry *se, *funcSE = identifiers->lookup($1);
        Type* retType = ((FunctionType*)funcSE->getType())->getRetType();
        se = new TemporarySymbolEntry(retType, SymbolTable::getLabel(), 0, 0.0);
        FuncCall *funcCall = new FuncCall(se, funcSE, $3);
        se = new TemporarySymbolEntry(retType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new UnaryExp(se, UnaryExp::NON, funcCall);
    }
    | ADD UnaryExp {
        if (AST_CHECK) printf("UnaryExp -> AND UnaryExp\n");
        SymbolEntry *se, *exprSE = $2->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new UnaryExp(se, UnaryExp::PLUS, $2);
    }
    | SUB UnaryExp {
        if (AST_CHECK) printf("UnaryExp -> SUB UnaryExp\n");
        SymbolEntry *se, *exprSE = $2->getSymbolEntry();
        if (exprSE->getType()->isConst()) {
            if (exprSE->getType()->isInt())
                se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), -exprSE->getIntValue(), 0.0);
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, -exprSE->getFloatValue());           
        }
        else {
            if (exprSE->getType()->isInt())
                se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0);
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        }
        $$ = new UnaryExp(se, UnaryExp::MINUS, $2);
    }
    | NOT UnaryExp {
        if (AST_CHECK) printf("UnaryExp -> NOT UnaryExp\n");
        SymbolEntry *se, *exprSE = $2->getSymbolEntry();
        if (exprSE->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), !exprSE->getIntValue(), 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), !exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new UnaryExp(se, UnaryExp::NOT, $2);
    }
    ;

FuncRParams
    : Exp {
        if (AST_CHECK) printf("FuncRParams -> Exp\n");
        $$ = new FuncRParams();
        $$->add($1);
    }
    | FuncRParams COMMA Exp {
        if (AST_CHECK) printf("FuncRParams -> FuncRParams COMMA Exp\n");
        $$ = $1;
        $$->add($3);
    }
    ;

MulExp
    : UnaryExp {
        if (AST_CHECK) printf("MulExp -> UnaryExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new MulExp(se, MulExp::NON, $1, nullptr);
    }
    | MulExp MULT UnaryExp {
        if (AST_CHECK) printf("MulExp -> MulExp MULT UnaryExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        Type *type1 = exprSE1->getType(), *type2 = exprSE2->getType();
        if (type1->isInt() && type2->isInt())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constIntType, SymbolTable::getLabel(), exprSE1->getIntValue() * exprSE2->getIntValue(), 0.0);
            else
                se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0);
        else if (type1->isFloat() && type2->isFloat())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() * exprSE2->getFloatValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        else
            if (type1->isConst() && type2->isConst())
                if (type1->isInt())
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getIntValue() * exprSE2->getFloatValue());
                else
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() * exprSE2->getIntValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new MulExp(se, MulExp::MULT, $1, $3);
    }
    | MulExp DIV UnaryExp {
        if (AST_CHECK) printf("MulExp -> MulExp DIV UnaryExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        Type *type1 = exprSE1->getType(), *type2 = exprSE2->getType();
        if (type1->isInt() && type2->isInt())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constIntType, SymbolTable::getLabel(), exprSE1->getIntValue() / exprSE2->getIntValue(), 0.0);
            else
                se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0);
        else if (type1->isFloat() && type2->isFloat())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() / exprSE2->getFloatValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        else
            if (type1->isConst() && type2->isConst())
                if (type1->isInt())
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getIntValue() / exprSE2->getFloatValue());
                else
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() / exprSE2->getIntValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new MulExp(se, MulExp::DIV, $1, $3);
    }
    | MulExp MOD UnaryExp {
        if (AST_CHECK) printf("MulExp -> MulExp MOD UnaryExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        Type *type1 = exprSE1->getType(), *type2 = exprSE2->getType();
        if (type1->isConst() && type2->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constIntType, SymbolTable::getLabel(), exprSE1->getIntValue() % exprSE2->getIntValue(), 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new MulExp(se, MulExp::MOD, $1, $3);
    }
    ;

AddExp
    : MulExp {
        if (AST_CHECK) printf("AddExp -> MulExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new AddExp(se, AddExp::NON, $1, nullptr);
    }
    | AddExp ADD MulExp {
        if (AST_CHECK) printf("AddExp -> AddExp ADD MulExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        Type *type1 = exprSE1->getType(), *type2 = exprSE2->getType();
        if (type1->isInt() && type2->isInt())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constIntType, SymbolTable::getLabel(), exprSE1->getIntValue() + exprSE2->getIntValue(), 0.0);
            else
                se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0);
        else if (type1->isFloat() && type2->isFloat())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() + exprSE2->getFloatValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        else
            if (type1->isConst() && type2->isConst())
                if (type1->isInt())
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getIntValue() + exprSE2->getFloatValue());
                else
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() + exprSE2->getIntValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new AddExp(se, AddExp::ADD, $1, $3);
    }
    | AddExp SUB MulExp {
        if (AST_CHECK) printf("AddExp -> AddExp SUB MulExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        Type *type1 = exprSE1->getType(), *type2 = exprSE2->getType();
        if (type1->isInt() && type2->isInt())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constIntType, SymbolTable::getLabel(), exprSE1->getIntValue() - exprSE2->getIntValue(), 0.0);
            else
                se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel(), 0, 0.0);
        else if (type1->isFloat() && type2->isFloat())
            if (type1->isConst() && type2->isConst())
                se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() - exprSE2->getFloatValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        else
            if (type1->isConst() && type2->isConst())
                if (type1->isInt())
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getIntValue() - exprSE2->getFloatValue());
                else
                    se = new TemporarySymbolEntry(TypeSystem::constFloatType, SymbolTable::getLabel(), 0, exprSE1->getFloatValue() - exprSE2->getIntValue());
            else
                se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new AddExp(se, AddExp::SUB, $1, $3);
    }
    ;

RelExp
    : AddExp {
        if (AST_CHECK) printf("RelExp -> AddExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new RelExp(se, RelExp::NON, $1, nullptr);
    }
    | RelExp LESS AddExp {
        if (AST_CHECK) printf("RelExp -> RelExp LESS AddExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() < exprSE2->getIntValue() ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new RelExp(se, RelExp::LESS, $1, $3);
    }
    | RelExp GREATER AddExp {
        if (AST_CHECK) printf("RelExp -> RelExp GREATER AddExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() > exprSE2->getIntValue() ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new RelExp(se, RelExp::GREATER, $1, $3);
    }
    | RelExp LEQUAL AddExp {
        if (AST_CHECK) printf("RelExp -> RelExp LEQUAL AddExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() <= exprSE2->getIntValue() ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new RelExp(se, RelExp::LEQUAL, $1, $3);
    }
    | RelExp GEQUAL AddExp {
        if (AST_CHECK) printf("RelExp -> RelExp GEQUAL AddExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() >= exprSE2->getIntValue() ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new RelExp(se, RelExp::GEQUAL, $1, $3);
    }
    ;

EqExp
    : RelExp {
        if (AST_CHECK) printf("EqExp -> RelExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new EqExp(se, EqExp::NON, $1, nullptr);
    }
    | EqExp EQUAL RelExp {
        if (AST_CHECK) printf("EqExp -> EqExp EQUAL RelExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() == exprSE2->getIntValue() ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new EqExp(se, EqExp::EQUAL, $1, $3);
    }
    | EqExp NEQUAL RelExp {
        if (AST_CHECK) printf("EqExp -> EqExp NEQUAL RelExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() != exprSE2->getIntValue() ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new EqExp(se, EqExp::NEQUAL, $1, $3);
    }
    ;

LAndExp
    : EqExp {
        if (AST_CHECK) printf("LAndExp -> EqExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        // 由于再继续规约也只会有逻辑运算了，所以这里就可以将数值完全转化为bool类型了
        if (exprSE->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new LAndExp(se, LAndExp::NON, $1, nullptr);
    }
    | LAndExp AND EqExp {
        if (AST_CHECK) printf("LAndExp -> LAndExp AND EqExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() != 0 && exprSE2->getIntValue() != 0 ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new LAndExp(se, LAndExp::AND, $1, $3);
    }
    ;

LOrExp
    : LAndExp {
        if (AST_CHECK) printf("LOrExp -> LAndExp\n");
        SymbolEntry *se, *exprSE = $1->getSymbolEntry();
        se = new TemporarySymbolEntry(exprSE->getType(), SymbolTable::getLabel(), exprSE->getIntValue(), exprSE->getFloatValue());
        $$ = new LOrExp(se, LOrExp::NON, $1, nullptr);
    }
    | LOrExp OR LAndExp {
        if (AST_CHECK) printf("LOrExp -> LOrExp OR LAndExp\n");
        SymbolEntry *se, *exprSE1 = $1->getSymbolEntry(), *exprSE2 = $3->getSymbolEntry();
        if (exprSE1->getType()->isConst() && exprSE2->getType()->isConst())
            se = new TemporarySymbolEntry(TypeSystem::constBoolType, SymbolTable::getLabel(), exprSE1->getIntValue() != 0 || exprSE2->getIntValue() != 0 ? 1 : 0, 0.0);
        else
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel(), 0, 0.0);
        $$ = new LOrExp(se, LOrExp::OR, $1, $3);
    }

%%

int yyerror(char const* message)
{
    std::cerr << message << " in file: " << outfile << std::endl;
    return -1;
}
