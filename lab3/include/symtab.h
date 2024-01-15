#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdlib.h>
#include <string.h>

/* 符号表节点结构体 */
struct symtab_entry {
    char *name;     /* 变量名 */
    char *type;     /* 变量类型 */
    struct symtab_entry *next; /* 指向下一个变量 */
};

/* 符号表结构体 */
struct symtab_hash {
    symtab_entry **table;   /* 哈希表数组 */
    int size;               /* 哈希表大小 */
};

/* 符号表栈节点结构体 */
struct symtab_stack_entry {
    symtab_hash *table;              /* 符号表 */
    struct symtab_stack_entry *next;  /* 下一个符号表栈节点 */
};

/* 符号表栈结构体 */
struct symtab_stack {
    symtab_stack_entry *top;    /* 栈顶 */
};

/* 初始化符号表 */
symtab_hash *init_symtab();

/* 计算哈希值 */
unsigned int hash(const char *name, int size);

/* 向符号表中添加一个变量 */
int add_to_symtab(symtab_hash *ht, const char *name, const char *type);

/* 在符号表中查找一个变量 */
symtab_entry *find_in_symtab(symtab_hash *ht, char *name);

/* 释放符号表 */
void free_symtab(symtab_hash *ht);

/* 创建一个新的符号表栈 */
struct symtab_stack *create_symtab_stack();

/* 将一个符号表推入符号表栈中 */
void push_symtab(struct symtab_stack *stack, symtab_hash *table);

/* 从符号表栈中弹出一个符号表 */
symtab_hash *pop_symtab(struct symtab_stack *stack);

#endif