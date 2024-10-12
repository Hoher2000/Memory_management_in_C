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

void *stack_pop(stack_t *stack) {
  if (!stack->count) return NULL;
  --stack->count;
  return stack->data[stack->count];
}

void stack_free(stack_t *stack) {
  if (stack == NULL || stack->data == NULL) return;
  free(stack->data);
  free(stack);
}

void scary_double_push(stack_t *s) {
  stack_push(s, (void*)1337);
  int *ptr = malloc(sizeof(int));
  *ptr = 1024;
  stack_push(s, ptr);
}

void stack_push_multiple_types(stack_t *s) {
  float* float_ptr = malloc(sizeof(float));
  *float_ptr = 3.14;
  stack_push(s, float_ptr);
  char* string = malloc(sizeof("Sneklang is blazingly slow!"));
  string = "Sneklang is blazingly slow!";
  stack_push(s, string);
}

int main() {
  stack_t *s = stack_new(1);
  if (s->capacity == 1) puts("Sets capacity to 1");
  else puts("Something wrong");
  if (s->count == 0) puts("No elements in the stack yet");
  else puts("Something wrong");
  if (s->data) puts( "Allocates the stack data");
  else puts("Something wrong");
  
  int a = 1;
  stack_push(s, &a);
  stack_push(s, &a);
  if (s->capacity == 2) puts("Sets capacity to 2");
  else puts("Something wrong");
  if (s->count == 2) puts("2 elements in the stack");
  else puts("Something wrong");
  
  stack_pop(s);
  printf("After pop size of stack is %lu\n", s->count);
  stack_pop(s);
  printf("After pop size of stack is %lu\n", s->count);
  
  stack_push(s, &a);
  stack_push(s, &a);
  printf("After push size of stack is %lu\n", s->count);
  stack_free(s);
  
  s = stack_new(1);
  scary_double_push(s);
  printf("After scary push size of stack is %lu\n", s->count);
  printf("%d %d\n", (int*)s->data[0] == (int*)1337, *(int*)s->data[1] == 1024);
  stack_push_multiple_types(s);
  printf("After multiple push size of stack is %lu\n", s->count);
  printf("%f %s\n", *(float*)s->data[2], (char*)s->data[3]);
  return 0;
}
