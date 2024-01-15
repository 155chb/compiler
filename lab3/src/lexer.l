%option noyywrap
%option nounput
%option noinput

%top{
    #include <stdarg.h>
    #include <string.h>
    #include "common.h"
    #include "symtab.h"
}
%{
    extern dump_type_t dump_type;

    inline void dump_tokens(const char* format, ...){
        va_list args;
        va_start(args, format);
        if (dump_type == TOKENS)
            vfprintf(yyout, format, args);
        va_end(args);
    }

    /* Your code here, if desired (lab3). */
    int yycolumn = 0;                       // 很显然这是列号
    int is_def = 0;                         // 标记现在处于一条定义语句中
    int next_def = 0;                       // 标记下一个符号是否需要插入符号表，通过逗号更新
    int in_brace = 0;                       // 记录从进入一条定义语句之后，进入了第几个花括号，因为数组初始化内的逗号的更新能力需要被失效
    int in_paren = 0;                       // 记录在定义语句中进入了第几个括号中
    int def_in_paren = 0;                   // 记录在括号中使用了定义
    int global_in_brace = 0;                // 记录是否正在全局定义中
    extern struct symtab_stack* symtab_stack;      // 符号表栈
    extern struct symtab_stack* symtab_stack_used; // 记录使用过的符号表，防止地址重复分配

%}

/* definitions section */
DECIMAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
/*  Your code here (lab3). */

OCTAL 0[0-7]*
HEXADECIMAL 0[xX][0-9a-fA-F]+
DECIMAL_F ((([0-9]*\.[0-9]+)|([0-9]+\.))([eE][\+-]?[0-9]+)?)|([0-9]+[eE][\+-]?[0-9]+)
HEXADECIMAL_F 0[xX]((([0-9a-fA-F]*\.[0-9a-fA-F]+)|([0-9a-fA-F]+\.))|[0-9a-fA-F]+)[pP][\+-]?[0-9]+

%x COMMENT_M
%x COMMENT_S

%%
    /* rules section */

    /* 类型 */

"int" {
    is_def = 1;
    next_def = 1;
    in_brace = 0;
    if (in_paren != 0)
        def_in_paren = 1;
    dump_tokens("INT\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 3;
}

"float" {
    is_def = 2;
    next_def = 1;
    in_brace = 0;
    if (in_paren != 0)
        def_in_paren = 1;
    dump_tokens("FLOAT\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 5;
}

"void" {
    is_def = 3;
    next_def = 1;
    in_brace = 0;
    if (in_paren != 0)
        def_in_paren = 1;
    dump_tokens("VOID\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 4;
}

    /* 特殊保留字 */

"const" {
    dump_tokens("CONST\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 5;
}

"if" {
    dump_tokens("IF\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

"else" {
    dump_tokens("ELSE\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 4;
}

"return" {
    dump_tokens("RETURN\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 6;
}

"while" {
    dump_tokens("WHILE\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 5;
}

"break" {
    dump_tokens("BREAK\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 5;
}

"continue" {
    dump_tokens("CONTINUE\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 8;
}

    /* 比较运算符 */

"==" {
    dump_tokens("EQUAL\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

"<=" {
    dump_tokens("LEQUAL\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

">=" {
    dump_tokens("GEQUAL\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

"!=" {
    dump_tokens("NEQUAL\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

"<" {
    dump_tokens("LESS\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

">" {
    dump_tokens("GREATER\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

    /* 赋值符号 */

"=" {
    dump_tokens("ASSIGN\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

    /* 括号大集合 */

"(" {
    if (is_def != 0 && global_in_brace == 0) {
        symtab_hash *newSymtab = init_symtab();
        push_symtab(symtab_stack, newSymtab);
    }
    in_paren += 1;
    dump_tokens("LPAREN\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

")" {
    if (is_def != 0 && global_in_brace == 0 && def_in_paren == 0) {
        symtab_hash *newSymtab = pop_symtab(symtab_stack);
        free_symtab(newSymtab);
    }
    in_paren -= 1;
    dump_tokens("RPAREN\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"[" {
    dump_tokens("LBRACK\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"]" {
    dump_tokens("RBRACK\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"{" {
    if (def_in_paren == 0) {
        symtab_hash *newSymtab = init_symtab();
        push_symtab(symtab_stack, newSymtab);
    }
    def_in_paren = 0;
    if (is_def != 0)
        in_brace += 1;
    dump_tokens("LBRACE\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"}" {
    symtab_hash *newSymtab = pop_symtab(symtab_stack);
    // free_symtab(newSymtab);
    push_symtab(symtab_stack_used, newSymtab);
    if (is_def != 0 && in_brace > 0)
        in_brace -= 1;
    dump_tokens("RBRACE\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

    /* 算术运算符 */

"+" {
    dump_tokens("ADD\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"-" {
    dump_tokens("MINUS\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"*" {
    dump_tokens("MULT\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"/" {
    dump_tokens("DIV\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

"%" {
    dump_tokens("MOD\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

    /* 逻辑运算符 */

"&&" {
    dump_tokens("AND\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

"||" {
    dump_tokens("OR\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 2;
}

"!" {
    dump_tokens("NOT\t\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

    /* 断句的符号(分隔符) */

"," {
    if (is_def != 0 && in_brace == 0)
        next_def = 1;
    dump_tokens("COMMA\t\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

";" {
    is_def = 0;
    next_def = 0;
    in_brace = 0;
    dump_tokens("SEMICOLON\t\t%20s\t%d\t%d\n", yytext, yylineno, yycolumn);
    yycolumn += 1;
}

{EOL} {
    is_def = 0;
    next_def = 0;
    in_brace = 0;
    yylineno++, yycolumn=0;
}

{WHITE} yycolumn += strlen(yytext);

    /* 数值定义（整型） */

{DECIMAL} {
    int decimal;
    decimal = atoi(yytext);
    dump_tokens("DECIMAL\t\t\t%20s\t%d\t%d\t%d\n", yytext, yylineno, yycolumn, decimal);
    yycolumn += strlen(yytext);
}

{OCTAL} {
    int octal;
    octal = strtol(yytext, NULL, 8);
    dump_tokens("OCTAL\t\t\t%20s\t%d\t%d\t%d\n", yytext, yylineno, yycolumn, octal);
    yycolumn += strlen(yytext);
}

{HEXADECIMAL} {
    int hexadecimal;
    hexadecimal = strtol(yytext + 2, NULL, 16);
    dump_tokens("HEXADECIMAL\t\t%20s\t%d\t%d\t%d\n", yytext, yylineno, yycolumn, hexadecimal);
    yycolumn += strlen(yytext);
}

    /* 数值定义（浮点型） */

{DECIMAL_F} {
    float decimal_f;
    sscanf(yytext, "%f", &decimal_f);
    dump_tokens("DECIMAL_F\t\t%20s\t%d\t%d\t%f\n", yytext, yylineno, yycolumn, decimal_f);
    yycolumn += strlen(yytext);
}

{HEXADECIMAL_F} {
    float hexadecimal_f;
    sscanf(yytext, "%f", &hexadecimal_f);
    dump_tokens("HEXADECIMAL_F\t%20s\t%d\t%d\t%f\n", yytext, yylineno, yycolumn, hexadecimal_f);
    yycolumn += strlen(yytext);
}

    /* 注释 */

"/*" {
    BEGIN COMMENT_M;
    yycolumn += 2;
}

<COMMENT_M>{EOL}  yylineno++, yycolumn=0;

<COMMENT_M>"*/" {
    BEGIN INITIAL;
    yycolumn += 2;
}

<COMMENT_M>. {
    yycolumn += strlen(yytext);
}

"//" {
    BEGIN COMMENT_S;
    yycolumn += 2;
}

<COMMENT_S>{EOL} {
    yylineno++, yycolumn=0;
    BEGIN INITIAL;
}

<COMMENT_S>. {
    yycolumn += strlen(yytext);
}

    /* 符号定义 */

{ID} {
    symtab_entry *entry = NULL;
    symtab_stack_entry *stack_entry = symtab_stack->top;
    if (next_def == 1) {
        if (is_def == 1) {
            add_to_symtab(stack_entry->table, yytext, "int");
        } else if (is_def == 2) {
            add_to_symtab(stack_entry->table, yytext, "float");
        } else if (is_def == 3) {
            add_to_symtab(stack_entry->table, yytext, "void");
        }
        next_def = 0;
    }
    while (entry == NULL && stack_entry != NULL) {
        entry = find_in_symtab(stack_entry->table, yytext);
        stack_entry = stack_entry->next;
    }
    dump_tokens("ID\t\t\t\t%20s\t%d\t%d\t0x%X\n", yytext, yylineno, yycolumn, entry);
    yycolumn += strlen(yytext);
}

%%
/* user code section */