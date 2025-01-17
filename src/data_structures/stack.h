#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "../../CGDB.h"

#define MAX_STACK_SIZE 1000

typedef struct {
    uint vertex_index;
    ushort channel;
    int depth;
} StackEntry;

typedef struct {
    StackEntry* entries;
    int top;
    int capacity;
} Stack;

// Stack operations
Stack* create_stack(int capacity);
void destroy_stack(Stack* stack);
bool stack_push(Stack* stack, uint vertex_index, ushort channel, int depth);
bool stack_pop(Stack* stack, StackEntry* entry);
bool stack_is_empty(Stack* stack);
bool stack_is_full(Stack* stack);

#endif // STACK_H 