#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

Stack* create_stack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) {
        return NULL;
    }
    
    stack->entries = (StackEntry*)malloc(capacity * sizeof(StackEntry));
    if (!stack->entries) {
        free(stack);
        return NULL;
    }
    
    stack->capacity = capacity;
    stack->top = 0;
    return stack;
}

void destroy_stack(Stack* stack) {
    if (stack) {
        free(stack->entries);
        free(stack);
    }
}

bool stack_push(Stack* stack, uint vertex_index, ushort channel, int depth) {
    if (!stack || stack_is_full(stack)) {
        return false;
    }
    
    stack->entries[stack->top].vertex_index = vertex_index;
    stack->entries[stack->top].channel = channel;
    stack->entries[stack->top].depth = depth;
    stack->top++;
    
    return true;
}

bool stack_pop(Stack* stack, StackEntry* entry) {
    if (!stack || !entry || stack_is_empty(stack)) {
        return false;
    }
    
    stack->top--;
    *entry = stack->entries[stack->top];
    return true;
}

bool stack_is_empty(Stack* stack) {
    return stack ? stack->top == 0 : true;
}

bool stack_is_full(Stack* stack) {
    return stack ? stack->top >= stack->capacity : true;
} 