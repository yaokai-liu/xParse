//
// Project Name: xParse
// Filename: vm.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_VM_H
#define XPARSE_VM_H

#include "xtypes.h"

struct __XPARSE_VirtualMachine__; // NOLINT(*-reserved-identifier)
#define __XVM __XPARSE_VirtualMachine__ // NOLINT(*-reserved-identifier)
static struct __XPARSE_VM_Method__ { // NOLINT(*-reserved-identifier)
    xVoid (*init)(struct __XVM * vm);
    xVoid (*prepare)(struct __XVM * vm);
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

struct status_reg {
    // machine status
    enum: xuByte {
        vm_mode_arith = 0,
        vm_mode_regexp = 1,
    }       vm_mode: 1; // regexp or arith
    xuByte  :7;

    // regexp status
    xuByte  regexp_match_result: 1; // success or failed
    enum: xuByte {
        match_mode_normal = 0,
        match_mode_inverse = 1
    }       regexp_match_mode: 1; // normal or inverse
    xuByte  :6;

    // arith status
    xuByte  program_cmp_result;

    xuByte  __reserved__[5]; // NOLINT(*-reserved-identifier)
};

#endif //XPARSE_VM_H


