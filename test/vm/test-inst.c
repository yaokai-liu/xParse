/**
 * Project Name: xParse
 * Module Name: test/vm
 * Filename: test-inst.c
 * Creator: Yaokai Liu
 * Create Date: 2024-03-30
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "vm.h"
#include "inst.h"
#include <stdio.h>

void _memset(xuByte * _d, xInt _v, xSize _l) { // NOLINT(*-reserved-identifier)
    for(xSize i = 0; i < _l; i ++) {
        _d[i] = _v;
    }
}

int main() {
    inst inst_list[] = {
            {.single.opcode = inst_nop },
            {.single.opcode = inst_clear_ma_flag },
            {.single.opcode = inst_clear_cmp_flag },
            {.single.opcode = inst_regex_enter },
            {.single.opcode = inst_regex_reset },
            {.single.opcode = inst_regex_exit },
            {.single.opcode = inst_success },
            {.single.opcode = inst_failed },

            {.set_value.opcode = inst_set_vm_mode, .set_value.value = EXECUTE_REGEX_MODE},
            {.set_value.opcode = inst_set_vm_mode, .set_value.value = EXECUTE_PROCESS_MODE},
            {.set_value.opcode = inst_set_ma_mode, .set_value.value = MATCH_NORMAL_MODE},
            {.set_value.opcode = inst_set_ma_mode, .set_value.value = MATCH_INVERSE_MODE},

            {.msl_reg = {.opcode = inst_load, .rd = vm_count_reg, .rs = vm_arith_reg_0, .offset = 10}},
            {.msl_reg = {.opcode = inst_store, .rd = vm_count_reg, .rs = vm_arith_reg_0, .offset = 10}},
            {.msl_reg = {.opcode = inst_shift_move, .rd = vm_count_reg, .rs = vm_arith_reg_0, .offset = 4}},

            {.match_lit = {.opcode = inst_seq_lit1, .target = {'a', '\0', '\0'}}},
            {.match_lit = {.opcode = inst_seq_lit2, .target = {'a', 'b', '\0'}}},
            {.match_lit = {.opcode = inst_seq_lit3, .target = {'a', 'b', 'c'}}},
            {.match_lit = {.opcode = inst_set_lit1, .target = {'a', '\0', '\0'}}},
            {.match_lit = {.opcode = inst_set_lit2, .target = {'a', 'b', '\0'}}},
            {.match_lit = {.opcode = inst_set_lit3, .target = {'a', 'b', 'c'}}},
            {.match_lit = {.opcode = inst_range_lit, .target = {'a', 'z', '\0'}}},

            {.match_reg = {.opcode = inst_seq_reg, .reg = vm_arith_reg_1, .offset = 0000}},
            {.match_reg = {.opcode = inst_seq_reg, .reg = vm_arith_reg_1, .offset = 123}},
            {.match_reg = {.opcode = inst_set_reg, .reg = vm_arith_reg_1, .offset = 0000}},
            {.match_reg = {.opcode = inst_set_reg, .reg = vm_arith_reg_1, .offset = 123}},
            {.match_reg = {.opcode = inst_range_reg, .reg = vm_arith_reg_1, .offset = 0000}},
            {.match_reg = {.opcode = inst_range_reg, .reg = vm_arith_reg_1, .offset = 123}},
            {.jump = {.opcode = inst_jump_directly, .offset = 1234}},
            {.jump = {.opcode = inst_jump_if_eq, .offset = -1234}},
            {.jump = {.opcode = inst_jump_if_ne, .offset = 1234}},
            {.jump = {.opcode = inst_jump_if_lt, .offset = -1234}},
            {.jump = {.opcode = inst_jump_if_gt, .offset = -1234}},
            {.jump = {.opcode = inst_jump_if_le, .offset = 1234}},
            {.jump = {.opcode = inst_jump_if_ge, .offset = -1234}},
            {.jump = {.opcode = inst_jump_if_ma, .offset = 1234}},
            {.jump = {.opcode = inst_jump_if_nm, .offset = -1234}},
            {.jump = {.opcode = inst_call, .offset = 12345}},
            {.jump = {.opcode = inst_ret, .offset = 123467}},
            {.load_imm = {.opcode = inst_load_imm, .rd = vm_arith_reg_3, .imm = 12312}},
            {.arith_reg = {
                    .opcode = inst_add_u, .rd = vm_arith_reg_3,
                    .rs1 = vm_arith_reg_2, .rs2 = vm_arith_reg_1
            }},
            {.arith_reg = {
                    .opcode = inst_add_u, .rd = vm_reg_alloc,
                    .rs1 = vm_reg_alloc, .rs2 = vm_arith_reg_1
            }},
            {
                .arith_imm.opcode = inst_add_s_i,
                .arith_imm.rd = vm_arith_reg_0,
                .arith_imm.imm = 100,
            },
            {.cmp_reg = {.opcode = inst_cmp_u, .rs1 = vm_arith_reg_2, .rs2 = vm_arith_reg_f}},
            {.cmp_imm = {.opcode = inst_cmp_s_i, .rs1 = vm_arith_reg_2, .imm = 10086}},
    };
    char_t dest[512] = {};
    for(int i = 0; i < (sizeof(inst_list) / sizeof(inst_list[0])); i++){
        inst2str(&inst_list[i], dest);
        printf("%s\n", dest);
        _memset((xuByte *) dest, 0, 512);
    }
    return 0;
}
