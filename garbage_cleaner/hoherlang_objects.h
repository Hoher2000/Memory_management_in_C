#include <stddef.h>
#include <stdbool.h>

typedef struct SnekObject snek_object_t;

typedef struct {
  size_t size;
  snek_object_t **elements;
} snek_array_t;

typedef struct {
  snek_object_t *x, *y, *z;
} snek_vector_t;

typedef enum SnekObjectKind {
  INTEGER,
  FLOAT,
  STRING,
  VECTOR3,
  ARRAY
} snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
  char* v_string;
  snek_vector_t v_vector3;
  snek_array_t v_array;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
  int refcount;
} snek_object_t;

snek_object_t *new_snek_integer(int);
snek_object_t *new_snek_float(float);
snek_object_t *new_snek_string(char *);
snek_object_t *new_snek_vector3(snek_object_t *, snek_object_t *, snek_object_t *);
snek_object_t *new_snek_array(size_t);
bool snek_array_set(snek_object_t *, size_t, snek_object_t *);
snek_object_t *snek_array_get(snek_object_t *, size_t);
snek_object_t *_new_snek_object();
void refcount_inc(snek_object_t *);
void refcount_free(snek_object_t *);
void refcount_dec(snek_object_t *);
