
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "new_objects.h"
#include "objects.h"
#include "vm.h"

#include "log.h"

object_t *_new_object(vm_t *vm, char* name) {
  object_t *obj = calloc(1, sizeof(object_t)); //выделяем массив памяти для хранения объекта с инициализацией его нулями
  if (obj == NULL) {
    return NULL;
  }
  obj->is_marked = false;  //метка для мусора, по умолчанию ложь (мусор)
  int len = strlen(name);
  char *dst = malloc(len + 1); //выделяем память для хранения имени объекта 
  if (dst == NULL) {
    free(obj);
    return NULL;
  }
  strcpy(dst, name);
  obj->name = dst;
  vm_track_object(vm, obj); // добавляем объект в стэк объектов виртуальной машины
  return obj;
}

//новый объект-массив (аналог списка в пайтоне)
object_t *new_array(vm_t *vm, size_t size, char* name) {
  object_t *obj = _new_object(vm, name);
  if (obj == NULL) {
    return NULL;
  }

  object_t **elements = calloc(size, sizeof(object_t *)); //выделяем память под массив указателей на объекты, инициализируем их нулями
  if (elements == NULL) {
    free(obj);
    return NULL;
  }
  obj->kind = ARRAY; //устанавливаем тип объекта - массив
  obj->data.v_array = (array_t){.size = size, .elements = elements}; //создаем массив для хранения указателей на объекты
  #ifdef LOG
    printf("Создан объект-массив \"%s\" размером %lu элементов\n", obj->name, obj->data.v_array.size);
  #endif
  return obj;
}

//новый объект-вектор с 3-мя элементами(аналог tuple в пайтон)
object_t *new_vector3(
    vm_t *vm, object_t *x, object_t *y, object_t *z, char* name
) {
  if (x == NULL || y == NULL || z == NULL) {
    return NULL;
  }

  object_t *obj = _new_object(vm, name);
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = VECTOR3;
  obj->data.v_vector3 = (vector_t){.x = x, .y = y, .z = z};
  #ifdef LOG
    printf("Создан объект-вектор \"%s\"\n", obj->name);
  #endif
  return obj;
}

//новый целочисленный объект 
object_t *new_integer(vm_t *vm, int value, char* name) {
  object_t *obj = _new_object(vm, name);
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = INTEGER;
  obj->data.v_int = value;
  #ifdef LOG
    printf("Создан целочисленный объект \"%s\" со значением %d\n", obj->name, obj->data.v_int);
  #endif
  return obj;
}

//новый объект, хранящий число с плавающей точкой
object_t *new_float(vm_t *vm, float value, char* name) {
  object_t *obj = _new_object(vm, name);
  if (obj == NULL) {
    return NULL;
  }
  obj->kind = FLOAT;
  obj->data.v_float = value;
  #ifdef LOG
    printf("Создан объект-число с плавающей точкой \"%s\" со значением %f\n", obj->name, obj->data.v_float);
  #endif
  return obj;
}

//новый объект строка
object_t *new_string(vm_t *vm, char *value, char* name) {
  object_t *obj = _new_object(vm, name);
  if (obj == NULL) {
    return NULL;
  }
  int len = strlen(value);
  char *dst = malloc(len + 1); //выделяем память под массив char для хранения строки
  if (dst == NULL) {
    free(obj);
    return NULL;
  }
  strcpy(dst, value);
  obj->kind = STRING;
  obj->data.v_string = dst;
  #ifdef LOG
    printf("Создан объект-строка \"%s\" со значением \"%s\"\n", obj->name, obj->data.v_string);
  #endif
  return obj;
}
