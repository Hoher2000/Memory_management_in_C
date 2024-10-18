#include <string.h>
#include <stdio.h>
#include "stack.h"

#include "log.h"


void stack_push(stack_t *stack, void *obj) {
  if (stack->count == stack->capacity) {
    // если стек заполнен, то удваиваем его вместимость во избежание частой реаллокации
    stack->capacity *= 2;
    stack->data = realloc(stack->data, stack->capacity * sizeof(void *));
    if (stack->data == NULL) {
      // если память под новый размер стека не выделена завершаем программу с ошибкой
      exit(1);
    }
  }
  stack->data[stack->count++] = obj;
  return;
}

void *stack_pop(stack_t *stack) {
  if (stack->count == 0) {
    return NULL;
  }
  // уменьшаем размер стека на 1 и возвращаем указатель на последний элемент
  return stack->data[--stack->count];
}

//освобождаем память из под стека
void stack_free(stack_t *stack) {
  if (stack == NULL || stack->data == NULL) {
    return;
  }
  int len = strlen(stack->name);
  char *dst = malloc(len + 1);
  if (dst == NULL) {
    free(stack);
    return;
  }
  strcpy(dst, stack->name);
  free(stack->data);
  free(stack->name);
  free(stack);
  #ifdef LOG
    printf("Выполнено освобождение памяти стека \"%s\"\n", dst);
  #endif
  free(dst);
}

void stack_remove_nulls(stack_t *stack) {
  size_t new_count = 0;
  size_t old_count = stack->count;
  // сжимаем стек, удаляя нулевые указатели 
  for (size_t i = 0; i < stack->count; ++i) {
    if (stack->data[i] != NULL) {
      stack->data[new_count++] = stack->data[i];
    }
  }

  // обновляем количество элементов после сжимания
  stack->count = new_count;

  // оставшиеся слоты делаем нулевыми указателями
  for (size_t i = new_count; i < stack->capacity; ++i) {
    stack->data[i] = NULL;
  }
  #ifdef LOG
    if (old_count != new_count) printf("Нулевые указатели удалены из стека \"%s\", текущий размер стека %lu элементов\n", stack->name, stack->count);
  #endif
}

stack_t *stack_new(size_t capacity, char* name) {
  stack_t *stack = malloc(sizeof(stack_t)); //выделяем память в куче для стека
  if (stack == NULL) {
    return NULL;
  }

  stack->count = 0;
  stack->capacity = capacity;
  stack->data = malloc(stack->capacity * sizeof(void *)); //выделяем память для массива указателей на объекты стека
  if (stack->data == NULL) {
    free(stack);
    return NULL;
  }
  int len = strlen(name);
  char *dst = malloc(len + 1); //выделяем память для хранения имени стека в чаровском массиве
  if (dst == NULL) {
    free(stack);
    return NULL;
  }
  strcpy(dst, name);
  stack->name = dst;
  #ifdef LOG
    printf("Создан стек с именем \"%s\" вместимостью %lu элементов\n", stack->name, stack->capacity);
  #endif
  return stack;
}