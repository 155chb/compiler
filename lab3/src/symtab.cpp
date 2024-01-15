#include "symtab.h"

#define HASH_TABLE_SIZE 0x10

struct symtab_stack* symtab_stack;
struct symtab_stack* symtab_stack_used;

/* 初始化符号表 */
symtab_hash *init_symtab() {
    symtab_hash *ht = (symtab_hash *)malloc(sizeof(symtab_hash));
    ht->table = (symtab_entry **)calloc(HASH_TABLE_SIZE, sizeof(symtab_entry *));
    ht->size = HASH_TABLE_SIZE;
    return ht;
}

/* 计算哈希值 */
unsigned int hash(const char *name, int size) {
    unsigned int hashval = 0;
    for (; *name != '\0'; name++) {
        hashval = *name + 31 * hashval;
    }
    return hashval % size;
}

/* 向符号表中添加一个变量 */
int add_to_symtab(symtab_hash *ht, const char *name, const char *type) {
    unsigned int hashval = hash(name, ht->size);
    symtab_entry *entry = ht->table[hashval];

    /* 在符号表中查找变量 */
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return -1;  /* 变量已经存在 */
        }
        entry = entry->next;
    }

    /* 否则将变量添加到符号表中 */
    entry = (symtab_entry *)malloc(sizeof(symtab_entry));
    entry->name = strdup(name);
    entry->type = strdup(type);
    entry->next = ht->table[hashval];
    ht->table[hashval] = entry;
    return 0;
}

/* 在符号表中查找一个变量 */
symtab_entry *find_in_symtab(symtab_hash *ht, char *name) {
    unsigned int hashval = hash(name, ht->size);
    symtab_entry *entry = ht->table[hashval];

    /* 在符号表中查找变量 */
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

/* 释放符号表 */
void free_symtab(symtab_hash *ht) {
    int i;
    symtab_entry *entry, *prev;
    for (i = 0; i < ht->size; i++) {
        entry = ht->table[i];
        while (entry != NULL) {
            prev = entry;
            entry = entry->next;
            free(prev->name);
            free(prev->type);
            free(prev);
        }
    }
    free(ht->table);
    free(ht);
}

/* 创建一个新的符号表栈 */
struct symtab_stack *create_symtab_stack() {
    struct symtab_stack *stack = (struct symtab_stack *)malloc(sizeof(symtab_stack));
    stack->top = NULL;
    return stack;
}

/* 将一个符号表推入符号表栈中 */
void push_symtab(struct symtab_stack *stack, symtab_hash *table) {
    symtab_stack_entry *entry = (symtab_stack_entry *)malloc(sizeof(symtab_stack_entry));
    entry->table = table;
    entry->next = stack->top;
    stack->top = entry;
}

/* 从符号表栈中弹出一个符号表 */
symtab_hash *pop_symtab(struct symtab_stack *stack) {
    symtab_stack_entry *entry = stack->top;
    if (entry == NULL) {
        return NULL;
    }
    symtab_hash *table = entry->table;
    stack->top = entry->next;
    free(entry);
    return table;
}
