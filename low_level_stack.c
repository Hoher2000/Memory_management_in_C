#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct Stack {
  size_t count;
  size_t capacity;
  void **data;
} stack_t;

stack_t *stack_new(size_t capacity){
    stack_t* stack = malloc(sizeof(stack_t));
    if (stack == NULL) return NULL;
    stack->count = 0;
    stack->capacity = capacity;
    stack->data = malloc(sizeof(void*)*capacity);
    if (stack->data == NULL){
        free(stack);
        return NULL;
    }
    return stack;
}

void stack_push(stack_t *stack, void *obj) {
  if (stack->count == stack->capacity){
    stack->capacity *= 2;
    void **newdata = realloc(stack->data, stack->capacity);
    if (newdata == NULL){
        free(stack->data);
        return;
    }
    stack->data = newdata;
  }
  stack->data[stack->count++] = obj;
}


int main() {
  stack_t *s = stack_new(1);
  if (s->capacity == 1) puts("Sets capacity to 1");
  if (s->count == 0) puts("No elements in the stack yet");
  if (s->data) puts( "Allocates the stack data");
  
  int a = 1;
  stack_push(s, &a);
  stack_push(s, &a);
  if (s->capacity == 2) puts("Sets capacity to 2");
  if (s->count == 2) puts("2 elements in the stack");
  
  return 0;
}
