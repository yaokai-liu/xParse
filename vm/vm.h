//
// Project Name: xParse
// Filename: vm.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_VM_H
#define XPARSE_VM_H

#include <xdef.h>
#include <allocator.h>
#include <mem_manager.h>
#include "inst.h"

struct status_reg {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define MODE_FILED_IDX  2
#define FLAG_FILED_IDX  3
#define TRAP_FILED_IDX  5
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
        xuByte  regex_level_error: 1;
        xuByte  call_level_error: 1;
        xuByte  illegal_inst: 1;
        xuByte  div_zero: 1;
    } TRAP_FIELD;
    xuByte __reserved_3__[2]; // NOLINT(*-reserved-identifier)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define MODE_FILED_IDX  5
#define FLAG_FILED_IDX  4
#define TRAP_FILED_IDX  2
    xuByte __reserved_1__[2]; // NOLINT(*-reserved-identifier)
    struct {
        xuByte  div_zero: 1;
        xuByte  illegal_inst: 1;
        xuByte  call_level_error: 1;
        xuByte  regex_level_error: 1;
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
    char_t *        reset_src_reg    [[gnu::aligned(sizeof(xuLong))]];   // rs
    inst *          ret_addr_reg    [[gnu::aligned(sizeof(xuLong))]];    // ra
    xuLong          regex_level_reg [[gnu::aligned(sizeof(xuLong))]];    // ea stack top
    xuLong          call_level_reg  [[gnu::aligned(sizeof(xuLong))]];    // ra stack top
    union {
        xuLong  value;
        xuByte  bytes[8];
        struct status_reg fields;
    }               status_reg      [[gnu::aligned(sizeof(xuLong))]];    // status
    xuLong          stack_top_reg   [[gnu::aligned(sizeof(xuLong))]];    // stack top

    // read & write registers
    char_t *        src_reg         [[gnu::aligned(sizeof(xuLong))]];    // sc
    inst *          inst_reg        [[gnu::aligned(sizeof(xuLong))]];    // pc
    xuLong          count_reg       [[gnu::aligned(sizeof(xuLong))]];    // cr
    inst *          jump_base_reg   [[gnu::aligned(sizeof(xuLong))]];    // jb
    inst **         call_vec_reg   [[gnu::aligned(sizeof(xuLong))]];     // cv

    [[gnu::unused]] xuLong          __reserved__[4]; // NOLINT(*-reserved-identifier)

    // arithmetic registers
    // only arithmetic registers can load immediate numbers.
    xuLong          arith_reg[16];
};

typedef struct __XPARSE_VirtualMachine__ { // NOLINT(*-reserved-identifier)
    // registers
    struct __XPARSE_VM_Registers__  registers;

    // memories
    mem_manager *                   manager;
    const struct Allocator*         allocator;
} xVirtualMachine;

#define __XVM xVirtualMachine // NOLINT(*-reserved-identifier)
extern const struct __XPARSE_VM_Method__ { // NOLINT(*-reserved-identifier)
    __XVM * (*new)(const struct Allocator * allocator);
    xVoid (*init)(__XVM * vm, const struct Allocator* allocator);
    xVoid (*prepare)(__XVM * vm);
    xVoid (*execute)(__XVM * vm, inst * instruction);
} VM;
#undef __XVM
#define VM_ARITH_REG_START ( \
offsetof(struct __XPARSE_VM_Registers__, arith_reg) / sizeof(xuLong) \
)
#define VM_ARITH_REG_END ( \
sizeof(struct __XPARSE_VM_Registers__) / sizeof(xuLong) - 1\
)
typedef enum {
    vm_zero_reg = 0,
    vm_reset_src_reg,
    vm_ret_addr_reg,
    vm_regex_level_reg,
    vm_call_level_reg,
    vm_status_reg,
    vm_stack_reg,

    vm_src_reg,
    vm_inst_reg,
    vm_count_reg,
    vm_jump_base_reg,
    vm_call_vec_reg,

    vm_arith_reg_0 = VM_ARITH_REG_START,
    vm_arith_reg_1,
    vm_arith_reg_2,
    vm_arith_reg_3,
    vm_arith_reg_4,
    vm_arith_reg_5,
    vm_arith_reg_6,
    vm_arith_reg_7,
    vm_arith_reg_8,
    vm_arith_reg_9,
    vm_arith_reg_a,
    vm_arith_reg_b,
    vm_arith_reg_c,
    vm_arith_reg_d,
    vm_arith_reg_e,
    vm_arith_reg_f = VM_ARITH_REG_END,

    vm_reg_alloc = 255,

} __XPARSE_VM_register_enum__; // NOLINT(*-reserved-identifier)

typedef enum: xSize {
    VM_STATUS_VM_MODE           = 0b1000'0000L << (2 * 8),
    VM_STATUS_MATCH_MODE        = 0b0000'1000L << (2 * 8),

    VM_STATUS_CMP_EQ            = 0b0000'0001L << (3 * 8),
    VM_STATUS_CMP_NE            = 0b0000'0010L << (3 * 8),
    VM_STATUS_CMP_LT            = 0b0000'0100L << (3 * 8),
    VM_STATUS_CMP_GT            = 0b0000'1000L << (3 * 8),
    VM_STATUS_MA                = 0b0100'0000L << (3 * 8),
    VM_STATUS_NA                = 0b1000'0000L << (3 * 8),

    VM_ERROR_DIV_ZERO           = 0b1000'0000L << (5 * 8),
    VM_ERROR_ILLEGAL_INST       = 0b0100'0000L << (5 * 8),
    VM_ERROR_INST_MISS_ALIGN    = 0b0010'0000L << (5 * 8),

    VM_DEBUG_BREAKPOINT         = 0b0000'1000L << (5 * 8),

    VM_STATUS_CMP_FLAG_MASK     = VM_STATUS_CMP_EQ | VM_STATUS_CMP_GT | VM_STATUS_CMP_LT,
} __XPARSE_VM_status_reg_status_bit_enum__; // NOLINT(*-reserved-identifier)

typedef enum : xBool {
    VM_STATUS_VM_MODE_PROGRAM       = 0,
    VM_STATUS_VM_MODE_REGEXP        = 1,
    VM_STATUS_MATCH_MODE_NORMAL     = 0,
    VM_STATUS_MATCH_MODE_INVERSE    = 1,
} __XPARSE_VM_set_status_value_enum__;  // NOLINT(*-reserved-identifier)


#endif //XPARSE_VM_H
