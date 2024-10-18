#pragma once
#include "objects.h"
#include "stack.h"

//структура виртуальной миашины
typedef struct VirtualMachine {
  // фреймовый стек виртуальной машины
  stack_t *frames;

  //Стек объектов, создаваемых во время работы виртуальной машины (необходим для отслеживания помеченных объектов)
  stack_t *objects;
  // количество фреймов, размещенных на фреймовом стеке за время его существования. Необходимо для наименования фреймов
  size_t pushed_frames_count;
} vm_t;

//фрейм - это стек с объектами. 
typedef struct StackFrame {
  stack_t *references;
} frame_t;

/// Главные методы сборщика мусора.
void mark(vm_t *vm);
void trace(vm_t *vm);
void sweep(vm_t *vm);

void vm_collect_garbage(vm_t *vm);

/// Функции помощники для отслеживания объектов
void trace_blacken_object(stack_t *gray_objects, object_t *ref);
void trace_mark_object(stack_t *gray_objects, object_t *ref);

/// функция сборки мусора
void vm_collect_garbage(vm_t *vm);

/// тут все понятно
vm_t *vm_new();
void vm_free(vm_t *vm);
void vm_track_object(vm_t *vm, object_t *obj);
frame_t *vm_new_frame(vm_t *vm);
void vm_frame_push(vm_t *vm, frame_t *frame);
frame_t *vm_frame_pop(vm_t *vm);
void frame_free(frame_t *frame);

// Отмечает объекты, помещая их во фрейм
void frame_reference_object(frame_t *frame, object_t *obj);