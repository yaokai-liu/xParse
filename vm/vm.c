//
// Project Name: xParse
// Filename: vm.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "vm.h"
#include "mem_space.h"
#include "inst.h"

struct __XPARSE_VM_Registers__ { // NOLINT(*-reserved-identifier)
    // read only registers
    const xuLong    zero_reg        [[gnu::aligned(8)]];    // zero
    char_t *        src_reg         [[gnu::aligned(8)]];    // sc
    inst *          inst_reg        [[gnu::aligned(8)]];    // pc
    inst *          ret_addr_reg    [[gnu::aligned(8)]];    // ra
    char_t **       src_top_reg     [[gnu::aligned(8)]];    // src stack top
    inst **         ra_top_reg      [[gnu::aligned(8)]];    // ra stack top
    struct status_reg status_reg    [[gnu::aligned(8)]];    // status
    xuLong          stack_reg       [[gnu::aligned(8)]];    // stack top
    // read & write registers
    xuLong          count_erg       [[gnu::aligned(8)]];    // cr

    // arithmetic registers
    xuLong          arith_reg[15];
};


#define XVM __XPARSE_VirtualMachine__
struct XVM { // NOLINT(*-reserved-identifier)
    // registers
    struct __XPARSE_VM_Registers__ registers;

    // memories
    mem_space *     SRC_STACK;
    mem_space *     RA_STACK;
    mem_space *     RAM;
    struct Allocator* allocator;
};

static thread_local struct XVM VirtMachine = {};

xVoid vm_init(struct XVM * vm);
xVoid vm_execute(struct XVM * vm);

static struct __XPARSE_VM_Method__ VM = {
        .init = vm_init,
};

xVoid vm_init(struct XVM * vm) {
    // memories init
    if (!vm->RAM) {
        vm->RAM = MemSpace.new("RAM", vm->allocator, sizeof(xuLong));
    } else {
        MemSpace.clear(vm->RAM);
    }
    if (!vm->SRC_STACK){
        vm->SRC_STACK = MemSpace.new("SRC_STACK", vm->allocator, sizeof(char_t *));
    } else {
        MemSpace.clear(vm->SRC_STACK);
    }
    if (!vm->RA_STACK){
        vm->RA_STACK = MemSpace.new("RA_STACK", vm->allocator, sizeof(inst *));
    } else {
        MemSpace.clear(vm->RA_STACK);
    }
    MemSpace.init(vm->RAM);
    MemSpace.init(vm->SRC_STACK);
    MemSpace.init(vm->RA_STACK);

    // register init
    vm->registers.src_top_reg = 0;
    vm->registers.ra_top_reg = 0;
    vm->registers.inst_reg = 0;
    vm->registers.ret_addr_reg = 0;
    vm->registers.src_reg = 0;
    vm->registers.status_reg = (struct status_reg){};
    vm->registers.count_erg = 0;
}



#undef XVM