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
#include "inst.h"

struct __XPARSE_VirtualMachine__; // NOLINT(*-reserved-identifier)
#define __XVM __XPARSE_VirtualMachine__ // NOLINT(*-reserved-identifier)
extern const struct __XPARSE_VM_Method__ { // NOLINT(*-reserved-identifier)
    struct __XVM * (*new)(const struct Allocator * allocator);
    xVoid (*init)(struct __XVM * vm, const struct Allocator* allocator);
    xVoid (*prepare)(struct __XVM * vm);
    xVoid (*execute)(struct __XVM * vm, inst * instruction);
} VM;
#undef __XVM

typedef enum {
    vm_zero_reg = 0,
    vm_src_reg,
    vm_inst_reg,
    vm_ret_addr_reg,
    vm_src_top_reg,
    vm_ra_top_reg,
    vm_status_reg,
    vm_stack_reg,

    vm_count_reg = vm_zero_reg + 8,
    vm_jump_base_reg,

    vm_arith_reg_1, vm_arith_reg_start = vm_arith_reg_1,
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
    vm_arith_reg_f, vm_arith_reg_end = vm_arith_reg_f,

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


