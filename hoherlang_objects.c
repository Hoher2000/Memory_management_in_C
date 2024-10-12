#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct SnekObject snek_object_t;

typedef struct {
  snek_object_t *x, *y, *z;
} snek_vector_t;

typedef enum SnekObjectKind {
  INTEGER,
  FLOAT,
  STRING,
  VECTOR3
} snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
  char* v_string;
  snek_vector_t v_vector3;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

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
    return 0;
}
