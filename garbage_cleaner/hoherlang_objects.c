#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"hoherlang_objects.h"

snek_object_t *_new_snek_object() {
    snek_object_t *obj = malloc(sizeof(snek_object_t));
    if (obj == NULL) return NULL;
    obj->refcount = 1;
    return obj;
}

void refcount_inc(snek_object_t *obj) {
    if (obj == NULL) return;
    ++obj->refcount;
    return;
}

void refcount_free(snek_object_t *obj) {
    if (obj->kind == STRING){
        free(obj->data.v_string);
    }
    if (obj->kind == VECTOR3){
        refcount_dec(obj->data.v_vector3.x); refcount_dec(obj->data.v_vector3.y); refcount_dec(obj->data.v_vector3.z);
    }
    if (obj->kind == ARRAY){
        for (size_t i = 0; i < obj->data.v_array.size; ++i){
            refcount_dec(obj->data.v_array.elements[i]);
        }
        free(obj->data.v_array.elements);
    }
    free(obj);
    return;
}

void refcount_dec(snek_object_t *obj) {
    if (obj == NULL) return;
    --obj->refcount;
    if (obj->refcount == 0) refcount_free(obj);
    return;
}

snek_object_t *new_snek_integer(int value){
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) return NULL;
    obj->data.v_int = value;
    obj->kind = INTEGER;
    return obj;
}

snek_object_t *new_snek_float(float value){
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) return NULL;
    obj->data.v_float = value;
    obj->kind = FLOAT;
    return obj;
}

snek_object_t *new_snek_string(char *value) {
    snek_object_t *obj = _new_snek_object();
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
    snek_object_t *obj = _new_snek_object();
    if (obj == NULL) return NULL;
    obj->data.v_vector3 = (snek_vector_t){x,y,z};
    refcount_inc(x); refcount_inc(y); refcount_inc(z);
    obj->kind = VECTOR3;
    return obj;
}

snek_object_t *new_snek_array(size_t size) {
    snek_object_t *obj = _new_snek_object();
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
  if (snek_array_get(snek_obj, index) != NULL) refcount_dec(snek_array_get(snek_obj, index));
  snek_obj->data.v_array.elements[index]=value;
  refcount_inc(value);
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
