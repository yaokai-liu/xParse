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
    xuByte  EXECUTE_MODE;
    xuByte  MATCH_MODE;
    xuByte  MATCH_FLAG;
    xuByte  COMPARE_FLAG;
    xuByte  TRAP_FLAG;
    xuByte  STOP_FLAG;
    xuByte  __reserved__[2]; // NOLINT(*-reserved-identifier)
#elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    xuByte  __reserved__[2]; // NOLINT(*-reserved-identifier)
    xuByte  STOP_FLAG;
    xuByte  TRAP_FLAG;
    xuByte  MATCH_FLAG;
    xuByte  COMPARE_FLAG;
    xuByte  MATCH_MODE;
    xuByte  EXECUTE_MODE;
#endif
};

struct __XPARSE_VM_Registers__ { // NOLINT(*-reserved-identifier)
    // read only registers
    const xuLong    _zero_reg        [[gnu::aligned(sizeof(xuLong))]];    // zero
    union {
        xuLong  value;
        xuByte  bytes[8];
        struct status_reg fields;
    }               _status_reg      [[gnu::aligned(sizeof(xuLong))]];    // status
    char_t *        _reset_src_reg   [[gnu::aligned(sizeof(xuLong))]];   // reset position
    inst *          _ret_addr_reg    [[gnu::aligned(sizeof(xuLong))]];    // return address
    xuLong          _call_offset_reg [[gnu::aligned(sizeof(xuLong))]];    // call offset
    xuLong          _stack_top_reg   [[gnu::aligned(sizeof(xuLong))]];    // stack top
    xuLong          _regex_level_reg [[gnu::aligned(sizeof(xuLong))]];    // ea stack top
    xuLong          _call_level_reg  [[gnu::aligned(sizeof(xuLong))]];    // ra stack top

    // read & write registers
    char_t *        _src_reg         [[gnu::aligned(sizeof(xuLong))]];    // sc
    inst *          _inst_reg        [[gnu::aligned(sizeof(xuLong))]];    // pc
    xuLong          _count_reg       [[gnu::aligned(sizeof(xuLong))]];    // cr
    inst *          _jump_base_reg   [[gnu::aligned(sizeof(xuLong))]];    // jb
    inst **         _call_vec_reg   [[gnu::aligned(sizeof(xuLong))]];     // cv

    [[gnu::unused]] xuLong          __reserved__[3]; // NOLINT(*-reserved-identifier)

    // arithmetic registers
    // only arithmetic registers can load immediate numbers.
    xuLong          _arith_reg[16];
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
offsetof(struct __XPARSE_VM_Registers__, _arith_reg) / sizeof(xuLong) \
)
#define VM_ARITH_REG_END ( \
sizeof(struct __XPARSE_VM_Registers__) / sizeof(xuLong) - 1 \
)
typedef enum {
    vm_zero_reg = 0,
    vm_reset_src_reg,
    vm_ret_addr_reg,
    vm_regex_level_reg,
    vm_call_level_reg,
    vm_status_reg,

    vm_src_reg,
    vm_inst_reg,
    vm_count_reg,
    vm_stack_reg,
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
extern const char_t * const VM_REG_NAMES[];

typedef enum: xuByte {
    EXECUTE_PROCESS_MODE = 0,
    EXECUTE_REGEX_MODE = 1,

    MATCH_NORMAL_MODE = 0,
    MATCH_INVERSE_MODE = 1,

    MATCH_FLAG_UNSET = 0,
    MATCH_FLAG_MATCHED = 1,
    MATCH_FLAG_NOT_MATCHED = 2,

    COMPARE_FLAG_NC = 0, // no compares
    COMPARE_FLAG_EQ = 1, // equal
    COMPARE_FLAG_NE = 2, // equal
    COMPARE_FLAG_LT = 3, // less than
    COMPARE_FLAG_GT = 4, // greater than

    TRAP_NO_ERROR = 0,
    TRAP_DIV_ZERO_ERROR = 1,
    TRAP_ILLEGAL_INST_ERROR = 2,
    TRAP_RESET_LEVEL_ERROR = 3,
    TRAP_CALL_LEVEL_ERROR = 4,

    STOP_FLAG_CONTINUE = 0,
    STOP_FLAG_BREAKPOINT = 1,
    STOP_FLAG_SUCCESS = 2,
    STOP_FLAG_FAILED = 3,
} __XPARSE_VM_status_reg_status_bit_enum__; // NOLINT(*-reserved-identifier)
extern const char_t * const VM_EXECUTE_MODE_NAMES[];
extern const char_t * const VM_MATCH_MODE_NAMES[];

#define vm_writable(_reg) ( \
    (_reg) * sizeof(xuLong) >= offsetof(struct __XPARSE_VM_Registers__, _src_reg) \
)
#define vm_arithmetic(_reg) ( \
 (_reg) >= VM_ARITH_REG_START && (_reg) <= VM_ARITH_REG_END \
)

#endif //XPARSE_VM_H
