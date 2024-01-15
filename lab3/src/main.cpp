#include <iostream>
#include <string.h>
#include <unistd.h>
#include "common.h"


extern FILE *yyin;
extern FILE *yyout;
extern int yylineno;

int yylex();

struct symtab_hash;

extern struct symtab_stack *symtab_stack;
extern struct symtab_stack *symtab_stack_used;
extern struct symtab_stack *create_symtab_stack();
extern symtab_hash *init_symtab();
extern void push_symtab(struct symtab_stack *stack, symtab_hash *table);
extern symtab_hash *pop_symtab(struct symtab_stack *stack);
extern void free_symtab(symtab_hash *ht);


char outfile[256] = "a.out";
dump_type_t dump_type = ASM;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "to:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
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
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    symtab_stack = create_symtab_stack();
    symtab_stack_used = create_symtab_stack();
    symtab_hash *top = init_symtab();
    push_symtab(symtab_stack, top);
    yylineno = 0;
    yylex();
    top = pop_symtab(symtab_stack);
    free_symtab(top);
    free(symtab_stack);
    symtab_stack = NULL;
    while (true)
    {
        top = pop_symtab(symtab_stack_used);
        if (top == NULL)
            break;
        free_symtab(top);
    }
    free(symtab_stack_used);
    symtab_stack_used = NULL;
    return 0;
}
