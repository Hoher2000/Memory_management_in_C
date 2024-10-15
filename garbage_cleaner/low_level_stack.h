#include <stddef.h>
#include <stdlib.h>

typedef struct Stack {
  size_t count;
  size_t capacity;
  void **data;
} stack_t;

stack_t *stack_new(size_t);
void stack_push(stack_t *, void *);
void *stack_pop(stack_t *);
void stack_free(stack_t *);
void stack_remove_nulls(stack_t *);
