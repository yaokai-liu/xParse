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

#define vm_reg(_name) (vm->registers._##_name##_reg)

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
                    .value = EXECUTE_REGEX_MODE,
            }
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

    ck_assert(vm_reg(status).fields.EXECUTE_MODE == EXECUTE_PROCESS_MODE);
    VM.execute(vm, &instruction);
    ck_assert(vm_reg(status).fields.EXECUTE_MODE == EXECUTE_REGEX_MODE);
    instruction.set_value.value = EXECUTE_PROCESS_MODE;
    VM.execute(vm, &instruction);
    ck_assert(vm_reg(status).fields.EXECUTE_MODE == EXECUTE_PROCESS_MODE);
} END_TEST

START_TEST(test_set_ma_mode) {
    inst instruction = {
            .set_value.opcode = inst_set_ma_mode,
            .set_value.value = MATCH_INVERSE_MODE,
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

    ck_assert(vm_reg(status).fields.MATCH_MODE == MATCH_NORMAL_MODE);
    VM.execute(vm, &instruction);
    ck_assert(vm_reg(status).fields.MATCH_MODE == MATCH_INVERSE_MODE);
    instruction.set_value.value = MATCH_NORMAL_MODE;
    VM.execute(vm, &instruction);
    ck_assert(vm_reg(status).fields.MATCH_MODE == MATCH_NORMAL_MODE);
} END_TEST

START_TEST(test_load_imm64) {
    inst instruction = {
            .match_lit.opcode = inst_seq_lit1,
            .match_lit.target = {'a', 0, 0},
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

} END_TEST

START_TEST(test_seq_lit) {
    inst instruction = {
            .match_lit.opcode = inst_seq_lit1,
            .match_lit.target = {'a', 0, 0},
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

} END_TEST

START_TEST(test_clear_ma_flag) {
    inst instruction = {
            .single.opcode = inst_clear_ma_flag,
    };
    xVirtualMachine * vm = VM.new(&StdAllocator);

}
END_TEST


Suite * vm_execute_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("xVirtualMachine");

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
