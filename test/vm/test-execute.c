/**
 * Project Name: xParse
 * Module Name: test/vm
 * Filename: test-execute.c
 * Creator: Yaokai Liu
 * Create Date: 2024-03-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include <check.h>
#include <stdlib.h>
#include "vm.h"
#include "inst.h"
#include "allocator.h"
#include "mem_manager.h"
#include <stdio.h>


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
        xuByte  : 1;
        xuByte  miss_align_inst: 1;
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
    xuLong          regex_level_reg [[gnu::aligned(sizeof(xuLong))]];    // src stack top
    xuLong          call_level_reg  [[gnu::aligned(sizeof(xuLong))]];    // ra stack top
    union {
        xuLong  value;
        xuByte  bytes[8];
        struct status_reg fields;
    }               status_reg      [[gnu::aligned(sizeof(xuLong))]];    // status
    xuLong          stack_reg       [[gnu::aligned(sizeof(xuLong))]];    // stack top

    // read & write registers
    xuLong          count_reg       [[gnu::aligned(sizeof(xuLong))]];    // cr
    inst *          jump_base_reg   [[gnu::aligned(sizeof(xuLong))]];

    // arithmetic registers
    xuLong          arith_reg[15];
};


#define XVM __XPARSE_VirtualMachine__
struct XVM { // NOLINT(*-reserved-identifier)
    // registers
    struct __XPARSE_VM_Registers__  registers;

    // memories
    mem_manager *                   manager;
    const struct Allocator*         allocator;
};


typedef struct __XPARSE_VirtualMachine__ xVirtualMachine;
const static struct Allocator StdAllocator = {
    .malloc = malloc,
    .calloc = (void *(*)(xLong, xSize)) calloc,
    .free = free,
    .realloc = realloc,
    .memset = nullptr,
    .memcpy = nullptr,
};

START_TEST(test_set_vm_mode) {
    inst instruction = {
            .set_value = {
                    .opcode = inst_set_vm_mode,
                    .value = VM_STATUS_VM_MODE_REGEXP,
            }
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

    ck_assert(
            vm->registers.status_reg.fields.MODE_FIELD.vm_mode ==
            VM_STATUS_VM_MODE_PROGRAM);
    VM.execute(vm, &instruction);
    ck_assert(
            vm->registers.status_reg.fields.MODE_FIELD.vm_mode ==
            VM_STATUS_VM_MODE_REGEXP);
    instruction.set_value.value = VM_STATUS_VM_MODE_PROGRAM;
    VM.execute(vm, &instruction);
    ck_assert(
            vm->registers.status_reg.fields.MODE_FIELD.vm_mode ==
            VM_STATUS_VM_MODE_PROGRAM);
}
END_TEST

START_TEST(test_set_ma_mode) {
    inst instruction = {
            .set_value = {
                    .opcode = inst_set_ma_mode,
                    .value = VM_STATUS_MATCH_MODE_INVERSE,
            }
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

    ck_assert(
            vm->registers.status_reg.fields.MODE_FIELD.ma_mode ==
            VM_STATUS_MATCH_MODE_NORMAL);
    VM.execute(vm, &instruction);
    ck_assert(
            vm->registers.status_reg.fields.MODE_FIELD.ma_mode ==
            VM_STATUS_MATCH_MODE_INVERSE);
    instruction.set_value.value = VM_STATUS_MATCH_MODE_NORMAL;
    VM.execute(vm, &instruction);
    ck_assert(
            vm->registers.status_reg.fields.MODE_FIELD.ma_mode ==
            VM_STATUS_MATCH_MODE_NORMAL);
}
END_TEST


Suite * vm_execute_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("VM");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_set_vm_mode);
    tcase_add_test(tc_core, test_set_ma_mode);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(int argc, char * argv[]) {
    int number_failed;
    Suite *s = vm_execute_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
