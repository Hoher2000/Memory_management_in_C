#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"hoherlang_objects.h"

snek_object_t *new_snek_integer(int value){
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) return NULL;
    obj->data.v_int = value;
    obj->kind = INTEGER;
    return obj;
}

snek_object_t *new_snek_float(float value){
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) return NULL;
    obj->data.v_float = value;
    obj->kind = FLOAT;
    return obj;
}

snek_object_t *new_snek_string(char *value) {
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) return NULL;
    char* str = malloc(sizeof(value) + 1);
    if (str == NULL){
        free(obj);
        return NULL;
    }
    strcpy(str, value);
    obj->data.v_string = str;
    obj->kind = STRING;
    return obj;
}

snek_object_t *new_snek_vector3(
    snek_object_t *x, snek_object_t *y, snek_object_t *z
) {
    if (x == NULL || y == NULL || z == NULL) return NULL;
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) return NULL;
    obj->data.v_vector3 = (snek_vector_t){x,y,z};
    obj->kind = VECTOR3;
    return obj;
}

snek_object_t *new_snek_array(size_t size) {
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) return NULL;
    snek_object_t **array = calloc(size, sizeof(snek_object_t*));
     if (array == NULL){
        free(obj);
        return NULL;
    }
    obj->data.v_array = (snek_array_t){size, array};
    obj->kind = ARRAY;
    return obj;
}

bool snek_array_set(snek_object_t *snek_obj, size_t index, snek_object_t *value) {
  if (snek_obj == NULL ||
      value == NULL ||
      snek_obj->kind != ARRAY ||
      index >= snek_obj->data.v_array.size) return false;
  snek_obj->data.v_array.elements[index]=value;
  return true;
}

snek_object_t *snek_array_get(snek_object_t *snek_obj, size_t index) {
  if (snek_obj == NULL ||
      snek_obj->kind != ARRAY ||
      index >= snek_obj->data.v_array.size) return false;
  return snek_obj->data.v_array.elements[index];
}

int snek_length(snek_object_t *obj) {
  if (obj == NULL) return -1;
  if (obj->kind == INTEGER || obj->kind == FLOAT) return 1;
  if (obj->kind == STRING) return (int) strlen(obj->data.v_string);
  if (obj->kind == VECTOR3) return 3;
  if (obj->kind == ARRAY) return (int) obj->data.v_array.size;
  return -1;
}

snek_object_t *snek_add(snek_object_t *a, snek_object_t *b) {
  if (a == NULL || b == NULL) return NULL;
  if (a->kind == INTEGER){
      if (b->kind == INTEGER) return new_snek_integer(a->data.v_int + b->data.v_int);
      if (b->kind == FLOAT) return new_snek_float(a->data.v_int + b->data.v_float);
      return NULL;
  }
  if (a->kind == FLOAT){
      if (b->kind == INTEGER) return new_snek_float(a->data.v_float + b->data.v_int);
      if (b->kind == FLOAT) return new_snek_float(a->data.v_float + b->data.v_float);
      return NULL;
  }
  if (a->kind == STRING){
      if (b->kind != STRING) return NULL;
      int new_len = snek_length(a) + snek_length(b) + 1;
      char* temp = calloc((size_t) new_len, sizeof(char));
      strcat(temp, a->data.v_string); strcat(temp, b->data.v_string);
      snek_object_t *string_object = new_snek_string(temp);
      free(temp);
      return string_object;
  }
  if (a->kind == VECTOR3){
      if (b->kind != VECTOR3) return NULL;
      return new_snek_vector3(snek_add(a->data.v_vector3.x, b->data.v_vector3.x),
                              snek_add(a->data.v_vector3.y, b->data.v_vector3.y),
                              snek_add(a->data.v_vector3.z, b->data.v_vector3.z));
  }
  if (a->kind == ARRAY){
      if (b->kind != ARRAY) return NULL;
      size_t size_a = (size_t)snek_length(a);
      size_t size_b = (size_t)snek_length(b);
      snek_object_t *res = new_snek_array(size_a + size_b);
      for (size_t i = 0; i < size_a; ++i){
          snek_array_set(res, i, snek_array_get(a, i));
      }
      for (size_t i = 0; i < size_b; ++i){
          snek_array_set(res, i + size_a, snek_array_get(b, i));
      }
      return res;
  }
  return NULL;
}

int main()
{
    snek_object_t *int_object = new_snek_integer(42);
    printf("%d\n", int_object->data.v_int);
    snek_object_t *float_object = new_snek_float(3.1415);
    printf("%f\n", float_object->data.v_float);
    snek_object_t *string_object = new_snek_string("Hoher");
    printf("%s\n", string_object->data.v_string);
    snek_object_t *vector3_object = new_snek_vector3(int_object, float_object, string_object);
    printf("%d %f %s\n", vector3_object->data.v_vector3.x->data.v_int,
                         vector3_object->data.v_vector3.y->data.v_float,
                         vector3_object->data.v_vector3.z->data.v_string);
    snek_object_t *array_obj = new_snek_array(2);
    printf("size of array object is %lu\n", array_obj->data.v_array.size);
    printf("value of all array object is %s\n", array_obj->data.v_array.elements[1]==NULL? "NULL" : "JUNK");
    
    if (snek_array_set(array_obj, 0, int_object)){
        snek_object_t *retrieved_first = snek_array_get(array_obj, 0);
        printf("value of 1st array object is %d\n", retrieved_first->data.v_int);
    }
    if (snek_array_set(array_obj, 1, string_object)){
        snek_object_t *retrieved_second = snek_array_get(array_obj, 1);
        printf("value of 2nd array object is %s\n", retrieved_second->data.v_string);
    }
    
    printf("Len of int_object %d\nLen of float_object %d\nLen of string_object %d\nLen of vector3_object %d\nLen of array_obj %d\n",
        snek_length(int_object), snek_length(float_object), snek_length(string_object), snek_length(vector3_object), snek_length(array_obj));
        
    printf("42 + 42 = %d\n", snek_add(int_object, int_object)->data.v_int);
    printf("42 + 3.1415 = %f\n", snek_add(int_object, float_object)->data.v_float);
    printf("3.1415 + 42 = %f\n", snek_add(float_object, int_object)->data.v_float);
    printf("3.1415 + 3.1415 = %f\n", snek_add(float_object, float_object)->data.v_float);
    printf("Hoher + Hoher = %s\n", snek_add(string_object, string_object)->data.v_string);
    printf("(42, 3.1415, \"Hoher\") + (42, 3.1415, \"Hoher\") = \"%s\"\n", snek_add(vector3_object, vector3_object)->data.v_vector3.z->data.v_string);
    printf("%lu\n", snek_add(array_obj, array_obj)->data.v_array.size);
    printf("%s\n", snek_add(array_obj, array_obj)->data.v_array.elements[3]->data.v_string);
    return 0;
}
