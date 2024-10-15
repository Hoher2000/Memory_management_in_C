#include "vm.h"

vm_t *vm_new() {
    vm_t* vm = malloc(sizeof(vm_t));
    vm->frames = stack_new(8);
    vm->objects = stack_new(8);
    return vm;
}

void vm_free(vm_t *vm) {
    if (vm == NULL) return;
    for (int i = 0; i < vm->frames->count; ++i)
        frame_free(vm->frames->data[i]);
    stack_free(vm->frames);
    stack_free(vm->objects);
    free(vm);
    return;
}

void vm_frame_push(vm_t *vm, frame_t *frame) {
    stack_push(vm->frames, frame);
    return;
}

frame_t *vm_new_frame(vm_t *vm) {
    frame_t* vm_frame = malloc(sizeof(frame_t));
    if (vm_frame == NULL) return NULL;
    vm_frame->references = stack_new(8);
    vm_frame_push(vm, vm_frame);
    return vm_frame;
}

void frame_free(frame_t *frame) {
    if (frame == NULL) return;
    stack_free(frame->references);
    free(frame);
    return;
}
