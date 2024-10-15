#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

int main()
{
    vm_t *vm = vm_new();
    if (vm->frames->capacity == 8) puts("frames have capacity 8");
    if (vm->objects->capacity == 8) puts("objects have capacity 8");
    vm_new_frame(vm); vm_new_frame(vm);
    if (vm->frames->count == 2) puts( "2 frames was pushed");
    vm_free(vm);
    return 0;
}
