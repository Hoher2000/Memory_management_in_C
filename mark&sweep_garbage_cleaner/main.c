#include <stdio.h>
#include <stdlib.h>

#include "new_objects.h"
#include "objects.h"
#include "vm.h"

void test1(){
    vm_t *vm = vm_new();
    frame_t *f1 = vm_new_frame(vm);

    object_t *str1 = new_string(vm, "I wish I knew how to read.", "str1");
    frame_reference_object(f1, str1);
    object_t *arr = new_array(vm, 2, "arr");
    array_set(arr, 0, str1);
    frame_reference_object(f1, arr);
    vm_collect_garbage(vm);
    // nothing should be collected because
    // we haven't freed the frame
  
   //frame_free(vm_frame_pop(vm));
   vm_collect_garbage(vm);
   
   vm_free(vm);
   
}

void test2(){
    vm_t *vm = vm_new();
    frame_t *f1 = vm_new_frame(vm);
    frame_t *f2 = vm_new_frame(vm);
    frame_t *f3 = vm_new_frame(vm);

    object_t *str1 = new_string(vm, "This string is going into frame 1", "str1");
    frame_reference_object(f1, str1);

    object_t *str2 = new_string(vm, "This string is going into frame 2", "str2");
    frame_reference_object(f2, str2);

    object_t *str3 = new_string(vm, "This string is going into frame 3", "str3");
    frame_reference_object(f3, str3);

    object_t *i1 = new_integer(vm, 69, "i1");
    object_t *i2 = new_integer(vm, 420, "i2");
    object_t *i3 = new_integer(vm, 1337, "i3");
    object_t *v = new_vector3(vm, i1, i2, i3, "vector1");
    frame_reference_object(f2, v);
    frame_reference_object(f3, v);

    if (vm->objects->count == 7) puts("Correct number of objects in the VM is 7 before GC");
    else puts("Something wrong in code");

    // only free the top frame (f3)
    frame_free(vm_frame_pop(vm));
    vm_collect_garbage(vm);
    
    // VM pass should free the string, but not the vector
    // because its final frame hasn't been freed
    frame_free(vm_frame_pop(vm));
    frame_free(vm_frame_pop(vm));
    vm_collect_garbage(vm);
  
    if (vm->objects->count == 0) puts("No live objects remaining");
    else puts("Something wrong in code");
    vm_free(vm);
}

int main()
{
    test1(); test2();
    return 0;
}
