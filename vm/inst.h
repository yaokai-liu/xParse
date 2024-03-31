//
// Project Name: xParse
// Filename: inst.h
// Creator: Yaokai Liu
// Create Date: 2023-10-21
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#ifndef XPARSE_INST_H
#define XPARSE_INST_H

#include "char_t.h"

typedef enum: xuByte {
    inst_nop,
    inst_clear_ma_flag,
    inst_clear_cmp_flag,
    inst_regex_enter,
    inst_regex_reset,
    inst_regex_exit,
    inst_success,
    inst_failed,

    inst_set_vm_mode,
    inst_set_ma_mode,

    inst_load,
    inst_store,
    inst_shift_move,

    inst_seq_lit1,
    inst_set_lit1,
    inst_seq_lit2,
    inst_set_lit2,
    inst_seq_lit3,
    inst_set_lit3,
    inst_range_lit,

    inst_seq_reg,
    inst_set_reg,
    inst_range_reg,

    inst_jump_directly,
    inst_jump_if_eq,
    inst_jump_if_ne,
    inst_jump_if_lt,
    inst_jump_if_gt,
    inst_jump_if_le,
    inst_jump_if_ge,
    inst_jump_if_ma,
    inst_jump_if_nm,

    inst_call,
    inst_ret,

    inst_load_imm,

    inst_b_and,
    inst_b_or,
    inst_b_xor,
    inst_b_lsh,
    inst_b_rsh,
    inst_b_inv,
    inst_add_u,
    inst_sub_u,
    inst_mul_u,
    inst_div_u,
    inst_mod_u,
    inst_add_s,
    inst_sub_s,
    inst_mul_s,
    inst_div_s,
    inst_mod_s,

    inst_b_and_i,
    inst_b_or_i,
    inst_b_xor_i,
    inst_b_lsh_i,
    inst_b_rsh_i,
    inst_b_inv_i,
    inst_add_u_i,
    inst_sub_u_i,
    inst_mul_u_i,
    inst_div_u_i,
    inst_mod_u_i,
    inst_add_s_i,
    inst_sub_s_i,
    inst_mul_s_i,
    inst_div_s_i,
    inst_mod_s_i,

    inst_cmp_u,
    inst_cmp_s,

    inst_cmp_u_i,
    inst_cmp_s_i,

} __XPARSE_inst_opcode_enum__; // NOLINT(*-reserved-identifier)


struct inst_single {
    xuByte    opcode;
    xuByte    __reserved__[3]; // NOLINT(*-reserved-identifier)
};

struct inst_set_value {
    xuByte opcode;
    xuByte value;
    xuByte    __reserved__[2]; // NOLINT(*-reserved-identifier)

};

struct inst_msl_reg {
    xuByte  opcode;
    xuByte  rd;
    xuByte  rs;
    xByte   offset;
};

struct inst_load_imm {
    xuByte  opcode;
    xuByte  rd;
    xuShort imm;
};

struct inst_match_lit {
    xuByte  opcode;
    char_t  target[3];
};

struct inst_match_reg {
    xuByte  opcode;
    xuByte  reg;
    xuShort offset;
};

struct inst_jump {
    xuByte  opcode;
    xInt    offset: 24;
};

struct inst_arith_reg {
    xuByte  opcode;
    xuByte  rd;
    xuByte  rs1;
    xuByte  rs2;
};

struct inst_arith_imm {
    xuByte  opcode;
    xuByte  rd;
    xuShort imm;
};

struct inst_cmp_reg {
    xuByte  opcode;
    xuByte  rs1;
    xuByte  rs2;
    xByte   __reserved__; // NOLINT(*-reserved-identifier)
};

struct inst_cmp_imm {
    xuByte  opcode;
    xuByte  rs1;
    xuShort imm;
};

typedef union inst {
    struct inst_single      single;
    struct inst_set_value   set_value;
    struct inst_msl_reg     msl_reg;
    struct inst_match_lit   match_lit;
    struct inst_match_reg   match_reg;
    struct inst_jump        jump;
    struct inst_load_imm    load_imm;
    struct inst_arith_reg   arith_reg;
    struct inst_arith_imm   arith_imm;
    struct inst_cmp_reg     cmp_reg;
    struct inst_cmp_imm     cmp_imm;
} inst;

xVoid inst2str(const inst * _inst, char_t * _dest);

#endif //XPARSE_INST_H
