#pragma once
#include <stdbool.h>
#include <stddef.h>

#include "stack.h"

typedef struct Object object_t;

typedef struct {  //структура для хранения массива указателей на объекты в объекте
  size_t size;
  object_t **elements;
} array_t;

typedef struct { //структура для хранения кортежа указателей на 3 объекта (tuple) в объекте
  object_t *x;
  object_t *y;
  object_t *z;
} vector_t;

typedef enum ObjectKind { //перечисление типов объектов
  INTEGER,
  FLOAT,
  STRING,
  VECTOR3,
  ARRAY,
} object_kind_t;

typedef union ObjectData { //юнион для возможности хранить в объекте различные типы данных
  int v_int;
  float v_float;
  char *v_string;
  vector_t v_vector3;
  array_t v_array;
} object_data_t;

typedef struct Object { //непосредственно сам объект
  bool is_marked; // метка для сборщика мусора
  object_kind_t kind; // тип хранимых данных в объекте
  object_data_t data; //непосредственно данные соответсвующего типа
  char* name; //имя объекта для логирования
} object_t;

object_kind_t kind(object_t* obj);
void object_free(object_t *obj);
bool array_set(object_t *array, size_t index, object_t *value);
object_t *array_get(object_t *array, size_t index);