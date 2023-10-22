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

} VM;
#undef __XVM

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


