/**
 * Project Name: xParse
 * Module Name: test
 * Filename: main.c
 * Creator: Yaokai Liu
 * Create Date: 2024-03-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include <stdlib.h>
#include "vm.h"
#include "inst.h"
#include "allocator.h"
#include <stdio.h>

typedef struct __XPARSE_VirtualMachine__ xVirtualMachine;
const static struct Allocator StdAllocator = {
        .malloc = malloc,
        .calloc = (void *(*)(xLong, xSize)) calloc,
        .free = free,
        .realloc = realloc,
        .memset = nullptr,
        .memcpy = nullptr,
};

int main() {
    inst instruction = {
            .set_value = {
                    .opcode = inst_set_vm_mode,
                    .value = 21,
            }
    };
    printf("%p\n", VM.new);
    xVirtualMachine * vm = VM.new(&StdAllocator);
    VM.execute(vm, &instruction);
};