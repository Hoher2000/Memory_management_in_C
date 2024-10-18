
#include <stdio.h>
#include "vm.h"
#include "objects.h"
#include "stack.h"

#include "log.h"

//непосредственно сам сборщик мусора
void vm_collect_garbage(vm_t *vm) {
    #ifdef LOG
        printf("\n");
        puts("-------------------------------------------------");
        puts("Начало работы сборщика мусора");
    #endif
    mark(vm); //помечаем объекты
    trace(vm); //проверяем какие объеты доступны из фреймового стека виртуальной машины
    sweep(vm); //если объект недоступен из фреймового стека, значит он уже не используется программой и его можно удалить 
    #ifdef LOG
        puts("Работа сборщика мусора завершена");
        puts("-------------------------------------------------");
        printf("\n");
    #endif
}

// очитска (подметание неиспользуемых объектов)
void sweep(vm_t *vm) {
    int counter = 0;
    #ifdef LOG
        puts("Начинаем выметать мусор:");
    #endif
    for (size_t i = 0; i != vm->objects->count; ++i){ 
        // пробегаемся по всем объектам, созданным в виртуальной машине
        object_t* obj = vm->objects->data[i];      
        if (obj->is_marked){          
            obj->is_marked = false;  
            //если объект уже помечен - значит он еще досягаем из главного стека и используется программой, удалять его нельзя, 
            //снимаем с него метку удаления до следующего запуска сборщика мусора
            #ifdef LOG
                printf("      Объект \"%s\" помечен и еще используется программой. Удалять нельзя.\n", obj->name); 
            #endif
        }
        else{
            // если объект не помечен, значит он уже не досягаем из главного стека, его нужно удалить и освободить память
            #ifdef LOG
                printf("      Объект \"%s\" не помечен. Это мусор. Будем удалять\n", obj->name); 
            #endif
            object_free(obj);   
            //после освобождения памяти указтель на объект-мусор в стеке объектов виртуальной машины становится нулевым
            vm->objects->data[i] = NULL;
            ++counter;
        }
    }
    // удаляем нулевые указатели из стека объектов виртуальной машины
    stack_remove_nulls(vm->objects);
    #ifdef LOG
        if (counter) printf("Мусор обнаружен. Было удалено %d ненужных объектов\n", counter);
        else puts("Мусор не обнаружен. Все объекты активны.");
    #endif
}

//помечаем досягаемые из главного стека объекты для их удаления
void mark(vm_t *vm) {
  int counter = 0;
  #ifdef LOG
    puts("Начало пометки досягаемых объектов:");
  #endif
  //пробегаем по всем объектам всех фреймов, расположенных на главном стеке виртуальной машины
  for (size_t i = 0; i < vm->frames->count; i++) {
    frame_t *frame = vm->frames->data[i];
    for (size_t j = 0; j < frame->references->count; j++) {
      object_t *obj = frame->references->data[j];
      ++counter;
      obj->is_marked = true;  //если объект досягаем из главного стека,значит он еще нужен, его удалять нельзя, помечаем его
      #ifdef LOG
        printf("Объект \"%s\" досягаем и помечен\n", obj->name);
      #endif
    }
  }
  #ifdef LOG
    if (counter) printf("Обнаружено %d досягаемых объектов\n", counter);
    else puts("Досягаемые объекты не обнаружены. Все объекты не активны.");
  #endif
}

//
void trace(vm_t *vm) {
  //обходим все объекты, созданные программой (расположены на стеке объектов виртуальной машины)
  //создаем временный стек для "серых объектов"
  stack_t *gray_objects = stack_new(8, "gray_objects");
  if (gray_objects == NULL) {
    return;
  }
  // добавляем в серый стек объекты, которые уже помечены для неудаления и доступны из предыдущего стека (корня)
  for (int i = 0; i < vm->objects->count; i++) {
    object_t *obj = vm->objects->data[i];
    if (obj->is_marked) {
      stack_push(gray_objects, obj);
      #ifdef LOG
        printf("Помеченный объект \"%s\" добавлен в серый стек. Он не будет удален.\n", obj->name);
      #endif
    }
  }

  // если объект помечен для неудаления рекурсивно обходим все его содержимое для пометки
  while (gray_objects->count > 0) {
    trace_blacken_object(gray_objects, stack_pop(gray_objects));
  }

  // после обхода всех объектов удаляем серый стек и освобождаем память из под него
  stack_free(gray_objects);
}

//если объект содержит другие объекты
void trace_blacken_object(stack_t *gray_objects, object_t *ref) {
  object_t *obj = ref;
  // если объект кортеж
  if (obj->kind == VECTOR3) {
    vector_t vec = obj->data.v_vector3;
    //рекурсивно помечаем объекты кортежа
    trace_mark_object(gray_objects, vec.x);
    trace_mark_object(gray_objects, vec.y);
    trace_mark_object(gray_objects, vec.z);
  }
  //если объект массив
  else if (obj->kind == ARRAY) {
    for (size_t i = 0; i < obj->data.v_array.size; i++) {
      //рекурсивно помечаем объекты массива
      trace_mark_object(gray_objects, obj->data.v_array.elements[i]);
    }
  }
}

//функция для обхода и отметки всех объектов, содержащих другие объекты
void trace_mark_object(stack_t *gray_objects, object_t *obj) {
  if (obj == NULL || obj->is_marked) {
    return;
  }
  stack_push(gray_objects, obj); //объект досягаем, его надо пометить для неудаления
  #ifdef LOG
    printf("Объект \"%s\" добавлен в серый стек и помечен. Он не будет удален\n", obj->name);
  #endif
  //помечаем объект, он еще пока не мусор
  obj->is_marked = true;
}

//функция для добавления объекта во фрейм (область видимости)
void frame_reference_object(frame_t *frame, object_t *obj) {
  stack_push(frame->references, obj);
  #ifdef LOG
    printf("Объект \"%s\" добавлен в область видимости фрейма \"%s\"\n", obj->name, frame->references->name);
  #endif
}

//выделяем память для новой виртуальной машины
vm_t *vm_new() {
  vm_t *vm = malloc(sizeof(vm_t));
  if (vm == NULL) {
    return NULL;
  }
  #ifdef LOG
      puts("-------------------------------------------------");
      puts("Создание новой виртуальной машины");
  #endif
  vm->frames = stack_new(8, "VM frames");
  vm->objects = stack_new(8, "VM objects");
  vm->pushed_frames_count = 0;
  #ifdef LOG
      puts("Новая виртуальная машина успешно создана");
      puts("-------------------------------------------------"); printf("\n");
  #endif
  return vm;
}

//удаление виртуальной машины и освобождение памяти из-под неё
void vm_free(vm_t *vm) {
  #ifdef LOG
    puts("Начало удаления виртуальной машины");
  #endif
  // Освобождение фреймов и стекового фрейма
  for (size_t i = 0; i < vm->frames->count; i++) {
    frame_free(vm->frames->data[i]);
  }
  stack_free(vm->frames);
  
  //Освобождение стека объектов и самих объектов
  for (size_t i = 0; i < vm->objects->count; i++) {
    object_free(vm->objects->data[i]);
  }
  stack_free(vm->objects);
  // Освобождение памяти из под самой виртуальной машины
  free(vm);
  #ifdef LOG
      puts("Виртуальная машина успешно удалена. вся используемая память освобождена.");
      puts("-------------------------------------------------");
  #endif
}

//Добавдение фрейма на стековый фрейм виртуальной машины
void vm_frame_push(vm_t *vm, frame_t *frame) {
  #ifdef LOG
    printf("Фрейм \"%s\" добавлен на фреймовый стек виртуальной машины\n", frame->references->name);
  #endif
  stack_push(vm->frames, frame);
}

//удаление верхнего фрейма со стека фреймов виртуальной машины
frame_t *vm_frame_pop(vm_t *vm) {
  frame_t * f = vm->frames->data[vm->frames->count-1];
  #ifdef LOG
    printf("Верхний фрейм \"%s\" удален с фреймового стека виртуальной машины\n", f->references->name);
  #endif
  return stack_pop(vm->frames);
}

//создание нового фрейма
frame_t *vm_new_frame(vm_t *vm) {
  frame_t *frame = malloc(sizeof(frame_t));
  char* str = malloc(20);
  sprintf(str, "Frame %lu", ++vm->pushed_frames_count);
  frame->references = stack_new(8, str);
  free(str);
  vm_frame_push(vm, frame);
  return frame;
}

//удаление фрейма и освобождение памяти
void frame_free(frame_t *frame) {
  #ifdef LOG
    printf("Начало удаления фрейма \"%s\"\n", frame->references->name);
  #endif
  stack_free(frame->references);
  free(frame);
  #ifdef LOG
    printf("Фрейм успешно удален. Память освобождена\n", frame->references->name);
  #endif
}

//добавление любого созданного объекта в стек объектов виртуальной машины - позволяет отслеживать досягаемые и недосягаемые объекты
//по умолчанию все вновь созданные объекты недосягаемы 
void vm_track_object(vm_t *vm, object_t *obj) {
  stack_push(vm->objects, obj);
  #ifdef LOG
    printf("Объект \"%s\" успешно добавлен в стек объектов и отслеживается виртуальной машиной. Объект пока не инициализирован\n", obj->name);
  #endif
}


