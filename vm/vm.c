//
// Project Name: xParse
// Filename: vm.c
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "vm.h"
#include "inst.h"
#include "mem_manager.h"
#include "string.h"

struct status_reg {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    xuByte __reserved_1__[2]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  : 3;
        xuByte  ma_mode: 1;
        xuByte  : 3;
        xuByte  vm_mode: 1;
    }   MODE_FIELD;
    struct {
        xuByte  eq_flag: 1;
        xuByte  ne_flag: 1;
        xuByte  lt_flag: 1;
        xuByte  gt_flag: 1;
        xuByte  : 2;
        xuByte  ma_flag: 1;
        xuByte  nm_flag: 1;
    }   FLAG_FIELD;
    xuByte __reserved_2__[1]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  : 3;
        xuByte  breakpoint: 1;
        xuByte  : 1;
        xuByte  miss_align_inst: 1;
        xuByte  illegal_inst: 1;
        xuByte  div_zero: 1;
    } TRAP_FIELD;
    xuByte __reserved_3__[2]; // NOLINT(*-reserved-identifier)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    xuByte __reserved_1__[2]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  div_zero: 1;
        xuByte  illegal_inst: 1;
        xuByte  miss_align_inst: 1;
        xuByte  : 1;
        xuByte  breakpoint: 1;
        xuByte  : 3;
    } TRAP_FIELD;
    xuByte __reserved_2__[1]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  nm_flag: 1;
        xuByte  ma_flag: 1;
        xuByte  : 2;
        xuByte  gt_flag: 1;
        xuByte  lt_flag: 1;
        xuByte  ne_flag: 1;
        xuByte  eq_flag: 1;
    }   FLAG_FIELD;
    struct {
        xuByte  vm_mode: 1;
        xuByte  : 3;
        xuByte  ma_mode: 1;
        xuByte  : 3;
    }   MODE_FIELD;
    xuByte __reserved_3__[2]; // NOLINT(*-reserved-identifier)
#elif (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
#error "Unsupported endian: PDP ENDIAN."
#else
#error "No endian information found."
#endif
};

struct __XPARSE_VM_Registers__ { // NOLINT(*-reserved-identifier)
    // read only registers
    const xuLong    zero_reg        [[gnu::aligned(sizeof(xuLong))]];    // zero
    char_t *        src_reg         [[gnu::aligned(sizeof(xuLong))]];    // sc
    inst *          inst_reg        [[gnu::aligned(sizeof(xuLong))]];    // pc
    inst *          ret_addr_reg    [[gnu::aligned(sizeof(xuLong))]];    // ra
    xuLong          reg_level_reg   [[gnu::aligned(sizeof(xuLong))]];    // src stack top
    xuLong          call_level_reg  [[gnu::aligned(sizeof(xuLong))]];    // ra stack top
    union {
        xuLong  value;
        struct status_reg fields;
    }               status_reg      [[gnu::aligned(sizeof(xuLong))]];    // status
    xuLong          stack_reg       [[gnu::aligned(sizeof(xuLong))]];    // stack top
    // read & write registers
    xuLong          count_erg       [[gnu::aligned(sizeof(xuLong))]];    // cr

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