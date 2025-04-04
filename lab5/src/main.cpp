#include <iostream>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "Ast.h"
#include "Unit.h"

extern FILE *yyin;
extern FILE *yyout;
int yyparse();

#ifdef GENCODE_CHECK
#undef GENCODE_CHECK
#endif
#define GENCODE_CHECK 0

Ast ast;
Unit unit;
extern char outfile[256];
dump_type_t dump_type = ASM;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "iato:")) != -1)
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
        case 'i':
            dump_type = IR;
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
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    yyparse();
    if(dump_type == AST)
        ast.output();
    if (GENCODE_CHECK) printf("AST output passed!\n");
    ast.typeCheck();
    if (GENCODE_CHECK) printf("Type check passed!\n");
    ast.genCode(&unit);
    if (GENCODE_CHECK) printf("Code generation passed!\n");
    if(dump_type == IR)
        unit.output();
    return 0;
}
