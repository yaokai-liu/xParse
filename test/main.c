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
                    .value = EXECUTE_REGEX_MODE,
            }
    };
    printf("%p\n", VM.new);
    xVirtualMachine * vm = VM.new(&StdAllocator);
    printf("%d\n",
           vm->registers._status_reg.fields.EXECUTE_MODE ==
           EXECUTE_PROCESS_MODE);
    VM.execute(vm, &instruction);
    printf("%d\n",
           vm->registers._status_reg.fields.EXECUTE_MODE ==
                   EXECUTE_PROCESS_MODE);

    xInt offset = -123987432;
    xuLong val1 = 12312312311231;
    printf("%016lx, %016lx\n", val1, val1 + offset);
    printf("%b\n", val1 > val1 + offset);
};
