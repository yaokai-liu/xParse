/**
 * Project Name: xParse
 * Module Name: vm
 * Filename: inst.c
 * Creator: Yaokai Liu
 * Create Date: 2024-03-28
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "inst.h"
#include "strfmt.h"
#include "vm.h"

#define SET_OP_NAME(_name) [inst_##_name] = string_t(#_name)

const static char_t * INST_OP_NAMES[] = {
        SET_OP_NAME(nop),
        SET_OP_NAME(clear_ma_flag),
        SET_OP_NAME(clear_cmp_flag),
        SET_OP_NAME(success),
        SET_OP_NAME(failed),

        SET_OP_NAME(set_vm_mode),
        SET_OP_NAME(set_ma_mode),

        SET_OP_NAME(load),
        SET_OP_NAME(store),
        SET_OP_NAME(shift_move),
        SET_OP_NAME(load_imm),

        SET_OP_NAME(seq_lit1),
        SET_OP_NAME(seq_lit2),
        SET_OP_NAME(seq_lit3),
        SET_OP_NAME(set_lit1),
        SET_OP_NAME(set_lit2),
        SET_OP_NAME(set_lit3),
        SET_OP_NAME(seq_reg),
        SET_OP_NAME(set_reg),
        SET_OP_NAME(range_reg),
        SET_OP_NAME(range_lit),

        // regex context change
        SET_OP_NAME(regex_enter),
        SET_OP_NAME(regex_reset),
        SET_OP_NAME(regex_exit),

        // jump
        SET_OP_NAME(jump_directly),
        SET_OP_NAME(jump_if_eq),
        SET_OP_NAME(jump_if_ne),
        SET_OP_NAME(jump_if_lt),
        SET_OP_NAME(jump_if_gt),
        SET_OP_NAME(jump_if_le),
        SET_OP_NAME(jump_if_ge),
        SET_OP_NAME(jump_if_ma),
        SET_OP_NAME(jump_if_nm),
        SET_OP_NAME(call),
        SET_OP_NAME(ret),

        // arith_reg and compare
        SET_OP_NAME(b_and),
        SET_OP_NAME(b_or),
        SET_OP_NAME(b_xor),
        SET_OP_NAME(b_lsh),
        SET_OP_NAME(b_rsh),
        SET_OP_NAME(b_inv),
        SET_OP_NAME(b_and_i),
        SET_OP_NAME(b_or_i),
        SET_OP_NAME(b_xor_i),
        SET_OP_NAME(b_lsh_i),
        SET_OP_NAME(b_rsh_i),
        SET_OP_NAME(b_inv_i),

        SET_OP_NAME(add_u),
        SET_OP_NAME(sub_u),
        SET_OP_NAME(mul_u),
        SET_OP_NAME(div_u),
        SET_OP_NAME(mod_u),
        SET_OP_NAME(cmp_u),
        SET_OP_NAME(add_u_i),
        SET_OP_NAME(sub_u_i),
        SET_OP_NAME(mul_u_i),
        SET_OP_NAME(div_u_i),
        SET_OP_NAME(mod_u_i),
        SET_OP_NAME(cmp_u_i),

        SET_OP_NAME(add_s),
        SET_OP_NAME(sub_s),
        SET_OP_NAME(mul_s),
        SET_OP_NAME(div_s),
        SET_OP_NAME(mod_s),
        SET_OP_NAME(cmp_s),
        SET_OP_NAME(add_s_i),
        SET_OP_NAME(sub_s_i),
        SET_OP_NAME(mul_s_i),
        SET_OP_NAME(div_s_i),
        SET_OP_NAME(mod_s_i),
        SET_OP_NAME(cmp_s_i),
};

xVoid inst2str(const inst * _inst, char_t * _dest) {
    char_t * dest_tail = _dest;
    const char_t * op_name = INST_OP_NAMES[_inst->single.opcode];
    if (_inst->single.opcode <= inst_failed) {
        const static char_t * fmt = string_t("%s");
        strfmt_dsc(dest_tail, fmt, op_name);
    } else if (_inst->set_value.opcode <= inst_set_ma_mode) {
        const static char_t * fmt = string_t("%s  %s");
        static const char_t * const * tables[] = {
            VM_EXECUTE_MODE_NAMES, VM_MATCH_MODE_NAMES
        };
        xuInt idx = _inst->set_value.opcode - inst_set_vm_mode;
        const char_t * const * table = tables[idx];
        const char_t * value = table[_inst->set_value.value];
        strfmt_dsc(dest_tail, fmt, op_name, value);
    } else if (_inst->msl_reg.opcode == inst_load) {
        const char_t * rd = VM_REG_NAMES[_inst->msl_reg.rd];
        const char_t * rs = VM_REG_NAMES[_inst->msl_reg.rs];
        xByte offset = _inst->msl_reg.offset;
        const static char_t * fmt = string_t("%s  [%d[%s]] -> %s");
        strfmt_dsc(dest_tail, fmt, op_name, offset, rs, rd);
    } else if (_inst->msl_reg.opcode == inst_store) {
        const char_t * rd = VM_REG_NAMES[_inst->msl_reg.rd];
        const char_t * rs = VM_REG_NAMES[_inst->msl_reg.rs];
        xByte offset = _inst->msl_reg.offset;
        const static char_t * fmt = string_t("%s  [%s] -> %d[%s]");
        strfmt_dsc(dest_tail, fmt, op_name, rs, offset, rd);
    } else if (_inst->msl_reg.opcode == inst_shift_move) {
        const char_t * rd = VM_REG_NAMES[_inst->msl_reg.rd];
        const char_t * rs = VM_REG_NAMES[_inst->msl_reg.rs];
        xByte offset = _inst->msl_reg.offset;
        const static char_t * fmt = string_t("%s  [%s], %d -> %s");
        strfmt_dsc(dest_tail, fmt, op_name, rs, offset, rd);
    } else if (_inst->match_lit.opcode <= inst_set_lit1) {
        const static char_t * fmt = string_t("%s  \"%c\"");
        const char_t * target = _inst->match_lit.target;
        strfmt_dsc(dest_tail, fmt, op_name, target[0]);
    } else if (_inst->match_lit.opcode <= inst_set_lit2) {
        const static char_t * fmt = string_t("%s  \"%c%c\"");
        const char_t * target = _inst->match_lit.target;
        strfmt_dsc(dest_tail, fmt, op_name, target[0], target[1]);
    } else if (_inst->match_lit.opcode <= inst_set_lit3) {
        const static char_t * fmt = string_t("%s  \"%c%c%c\"");
        const char_t * target = _inst->match_lit.target;
        strfmt_dsc(dest_tail, fmt, op_name, target[0], target[1], target[2]);
    } else if (_inst->match_lit.opcode == inst_range_lit) {
        const static char_t * fmt = string_t("%s  ['%c','%c']");
        const char_t * target = _inst->match_lit.target;
        strfmt_dsc(dest_tail, fmt, op_name, target[0], target[1]);
    } else if (_inst->match_reg.opcode <= inst_range_reg) {
        const static char_t * fmt;
        const char_t *reg = VM_REG_NAMES[_inst->match_reg.reg];
        xShort offset = _inst->match_reg.offset;
        if (offset) {
            fmt = string_t("%s  [%s], %d");
            strfmt_dsc(dest_tail, fmt, op_name, reg, offset);
        } else {
            fmt = string_t("%s  [%s], [count_reg]");
            strfmt_dsc(dest_tail, fmt, op_name, reg);
        }
    } else if (_inst->jump.opcode <= inst_jump_if_nm) {
        const static char_t * fmt;
        xInt offset = _inst->jump.offset;
        fmt = (offset >= 0) ? string_t("%s  <%s+%d>") : string_t("%s  <%s%d>");
        const char_t *reg = VM_REG_NAMES[vm_jump_base_reg];
        strfmt_dsc(dest_tail, fmt, op_name, reg, offset);
    } else if (_inst->jump.opcode == inst_call) {
        const static char_t * fmt = string_t("%s  <[%s]%d>");
        const char_t *reg = VM_REG_NAMES[vm_call_vec_reg];
        xuInt offset = _inst->jump.offset;
        strfmt_dsc(dest_tail, fmt, op_name, reg, offset);
    } else if (_inst->jump.opcode == inst_ret) {
        const static char_t * fmt = string_t("%s  %d");
        xuInt offset = _inst->jump.offset;
        strfmt_dsc(dest_tail, fmt, op_name, offset);
    } else if (_inst->load_imm.opcode == inst_load_imm) {
        const static char_t * fmt = string_t("%s  %d -> %s");
        const char_t *rd = VM_REG_NAMES[_inst->load_imm.rd];
        xuInt imm = _inst->load_imm.imm;
        strfmt_dsc(dest_tail, fmt, op_name, imm, rd);
    } else if (_inst->arith_reg.opcode <= inst_mod_s) {
        const static char_t * fmt = string_t("%s  ([%s], [%s]) -> %s");
        const char_t *rd = VM_REG_NAMES[_inst->arith_reg.rd];
        const char_t *rs1 = VM_REG_NAMES[_inst->arith_reg.rs1];
        const char_t *rs2 = VM_REG_NAMES[_inst->arith_reg.rs2];
        strfmt_dsc(dest_tail, fmt, op_name, rs1, rs2, rd);
    } else if (_inst->arith_imm.opcode <= inst_mod_s_i) {
        const static char_t * fmt = string_t("%s  ([%s], %d) -> %s");
        const char_t *rd = VM_REG_NAMES[_inst->arith_imm.rd];
        xuShort imm = _inst->arith_imm.imm;
        strfmt_dsc(dest_tail, fmt, op_name, rd, imm, rd);
    } else if (_inst->cmp_reg.opcode <= inst_cmp_s) {
        const static char_t * fmt = string_t("%s  [%s], [%s]");
        const char_t *rs1 = VM_REG_NAMES[_inst->cmp_reg.rs1];
        const char_t *rs2 = VM_REG_NAMES[_inst->cmp_reg.rs2];
        strfmt_dsc(dest_tail, fmt, op_name, rs1, rs2);
    } else if (_inst->cmp_imm.opcode <= inst_cmp_s_i) {
        const static char_t * fmt = string_t("%s  [%s], %d");
        const char_t *rs1 = VM_REG_NAMES[_inst->cmp_imm.rs1];
        xuShort imm = _inst->cmp_imm.imm;
        strfmt_dsc(dest_tail, fmt, op_name, rs1, imm);
    }
}
