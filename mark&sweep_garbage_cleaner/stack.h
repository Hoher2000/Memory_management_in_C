#pragma once
#include <stddef.h>
#include <stdlib.h>

typedef struct Stack {  // низкоeровневый стэк, позволяющий хранить массив указателей на любые объекты (void*)
  size_t count;      //размер стека текущий
  size_t capacity;   //вместимость стека
  void **data;       //непосредственно массив указателей, положенных на стек объектов
  char* name;        //имя для логирования
} stack_t;

stack_t *stack_new(size_t, char*);
void stack_push(stack_t *, void *);
void *stack_pop(stack_t *);
void stack_free(stack_t *);
void stack_remove_nulls(stack_t *);
