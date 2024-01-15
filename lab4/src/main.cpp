#include <iostream>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "Ast.h"

#ifndef AST_CHECK
#define AST_CHECK false
#endif

extern FILE *yyin;
extern FILE *yyout;

int yyparse();

Ast ast;
extern char outfile[256];
dump_type_t dump_type = ASM;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "ato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_type = AST;
            break;
        case 't':
            dump_type = TOKENS;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (AST_CHECK) yyout = stdout;
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    yyparse();
    if (AST_CHECK) printf("parse success\n");
    if(dump_type == AST)
        ast.output();
    if (AST_CHECK) printf("dump success\n");
    printf("file: %s\tAST create success\n", outfile);
    return 0;
}
