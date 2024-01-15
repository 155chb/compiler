%option noyywrap
%option nounput
%option noinput
%top{
    #include <stdarg.h>
    #include "common.h"
    #include "parser.h"
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
"int" {
    dump_tokens("INT\t%s\n", yytext);
    return INT;
}

"float" {
    dump_tokens("FLOAT\t%s\n", yytext);
    return FLOAT;
}

"void" {
    dump_tokens("VOID\t%s\n", yytext);
    return VOID;
}

    /* 特殊保留字 */

"const" {
    dump_tokens("CONST\t%s\n", yytext);
    return CONST;
}

"if" {
    dump_tokens("IF\t%s\n", yytext);
    return IF;
}

"else" {
    dump_tokens("ELSE\t%s\n", yytext);
    return ELSE;
}

"return" {
    dump_tokens("RETURN\t%s\n", yytext);
    return RETURN;
}

"while" {
    dump_tokens("WHILE\t%s\n", yytext);
    return WHILE;
}

"break" {
    dump_tokens("BREAK\t%s\n", yytext);
    return BREAK;
}

"continue" {
    dump_tokens("CONTINUE\t%s\n", yytext);
    return CONTINUE;
}

    /* 比较运算符 */

"==" {
    dump_tokens("EQUAL\t%s\n", yytext);
    return EQUAL;
}

"<=" {
    dump_tokens("LEQUAL\t%s\n", yytext);
    return LEQUAL;
}

">=" {
    dump_tokens("GEQUAL\t%s\n", yytext);
    return GEQUAL;
}

"!=" {
    dump_tokens("NEQUAL\t%s\n", yytext);
    return NEQUAL;
}

"<" {
    dump_tokens("LESS\t%s\n", yytext);
    return LESS;
}

">" {
    dump_tokens("GREATER\t%s\n", yytext);
    return GREATER;
}

    /* 赋值符号 */

"=" {
    dump_tokens("ASSIGN\t%s\n", yytext);
    return ASSIGN;
}

    /* 括号大集合 */

"(" {
    dump_tokens("LPAREN\t%s\n", yytext);
    return LPAREN;
}

")" {
    dump_tokens("RPAREN\t%s\n", yytext);
    return RPAREN;
}

"[" {
    dump_tokens("LBRACK\t%s\n", yytext);
    return LBRACK;
}

"]" {
    dump_tokens("RBRACK\t%s\n", yytext);
    return RBRACK;
}

"{" {
    dump_tokens("LBRACE\t%s\n", yytext);
    return LBRACE;
}

"}" {
    dump_tokens("RBRACE\t%s\n", yytext);
    return RBRACE;
}

    /* 算术运算符 */

"+" {
    dump_tokens("ADD\t%s\n", yytext);
    return ADD;
}

"-" {
    dump_tokens("SUB\t%s\n", yytext);
    return SUB;
}

"*" {
    dump_tokens("MULT\t%s\n", yytext);
    return MULT;
}

"/" {
    dump_tokens("DIV\t%s\n", yytext);
    return DIV;
}

"%" {
    dump_tokens("MOD\t%s\n", yytext);
    return MOD;
}

    /* 逻辑运算符 */

"&&" {
    dump_tokens("AND\t%s\n", yytext);
    return AND;
}

"||" {
    dump_tokens("OR\t%s\n", yytext);
    return OR;
}

"!" {
    dump_tokens("NOT\t%s\n", yytext);
    return NOT;
}

    /* 断句的符号(分隔符) */

"," {
    dump_tokens("COMMA\t%s\n", yytext);
    return COMMA;
}

";" {
    dump_tokens("SEMICOLON\t%s\n", yytext);
    return SEMICOLON;
}

{EOL} yylineno++;

{WHITE} ;

    /* 数值定义（整型） */

{DECIMAL} {
    int decimal;
    decimal = atoi(yytext);
    dump_tokens("DECIMAL\t%s\t%d\n", yytext, decimal);
    yylval.itype = decimal;
    return INTEGER;
}

{OCTAL} {
    int octal;
    octal = strtol(yytext, NULL, 8);
    dump_tokens("OCTAL\t%s\t%d\n", yytext, octal);
    yylval.itype = octal;
    return INTEGER;
}

{HEXADECIMAL} {
    int hexadecimal;
    hexadecimal = strtol(yytext + 2, NULL, 16);
    dump_tokens("HEXADECIMAL\t%s\t%d\n", yytext, hexadecimal);
    yylval.itype = hexadecimal;
    return INTEGER;
}

    /* 数值定义（浮点型） */

{DECIMAL_F} {
    float decimal_f;
    sscanf(yytext, "%f", &decimal_f);
    dump_tokens("DECIMAL_F\t%s\t%d\n", yytext, decimal_f);
    yylval.ftype = decimal_f;
    return FLOATEGER;
}

{HEXADECIMAL_F} {
    float hexadecimal_f;
    sscanf(yytext, "%f", &hexadecimal_f);
    dump_tokens("DECIMAL_F\t%s\t%d\n", yytext, hexadecimal_f);
    yylval.ftype = hexadecimal_f;
    return FLOATEGER;
}

    /* 注释 */

"/*" BEGIN COMMENT_M;

<COMMENT_M>{EOL}  yylineno++;

<COMMENT_M>"*/" BEGIN INITIAL;

<COMMENT_M>. ;

"//" BEGIN COMMENT_S;

<COMMENT_S>{EOL} {
    yylineno++;
    BEGIN INITIAL;
}

<COMMENT_S>. ;

    /* 符号定义 */

{ID} {
    char *lexeme;
    dump_tokens("ID\t%s\n", yytext);
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}

    /*  Your code here (lab3). */
%%
/* user code section */
