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

typedef enum {
    inst_nop = 0,

    inst_set_vm_mode,
    inst_set_ma_mode, // match flag
    inst_clear_flag,

    inst_load,
    inst_store,
    inst_shift_move,
    inst_load_imm,

    inst_seq_lit1,
    inst_seq_lit2,
    inst_seq_lit3,
    inst_set_lit1,
    inst_set_lit2,
    inst_set_lit3,
    inst_seq_reg,
    inst_set_reg,
    inst_range_reg,
    inst_range_lit,

    inst_ctx_enter,
    inst_ctx_reset,
    inst_ctx_exit,


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
    inst_success,
    inst_failed,

    inst_add,
    inst_sub,
    inst_mul,
    inst_div,
    inst_mod,
    inst_b_and,
    inst_b_or,
    inst_b_xor,
    inst_b_lsh,
    inst_b_rsh,
    inst_b_inv,
    inst_cmp_signed,
    inst_cmp_unsigned,
    inst_add_i,
    inst_sub_i,
    inst_mul_i,
    inst_div_i,
    inst_mod_i,
    inst_b_and_i,
    inst_b_or_i,
    inst_b_xor_i,
    inst_b_lsh_i,
    inst_b_rsh_i,
    inst_b_inv_i,
    inst_cmp_i_signed,
    inst_cmp_i_unsigned,

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
    xShort  offset;
};

struct inst_jump {
    xuByte  opcode;
    xInt    offset: 24;
};

struct inst_arith {
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
    xShort  imm;
};

typedef union inst {
    struct inst_single      single;
    struct inst_set_value   set_value;
    struct inst_msl_reg     msl_reg;
    struct inst_load_imm    load_imm;
    struct inst_match_lit   match_lit;
    struct inst_match_reg   match_reg;
    struct inst_jump        jump;
    struct inst_arith       arith;
    struct inst_arith_imm   arith_imm;
    struct inst_cmp_reg     cmp_reg;
    struct inst_cmp_imm     cmp_imm;
} inst;


#endif //XPARSE_INST_H
