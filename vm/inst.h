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
    inst_set_mf, // match flag

    inst_load,
    inst_load_imm,
    inst_store,

    inst_seq_lit1,
    inst_seq_lit2,
    inst_seq_lit3,
    inst_seq_reg,
    inst_set_lit1,
    inst_set_lit2,
    inst_set_lit3,
    inst_set_reg,

    inst_jump,

    inst_ret,

    inst_ctx_enter,
    inst_ctx_reset,
    inst_ctx_exit,

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
    inst_l_and,
    inst_l_or,
    inst_l_xor,
    inst_l_inv,
    inst_cmp,

} __XPARSE_inst_opcode_enum__; // NOLINT(*-reserved-identifier)


struct inst_nop {
    xuByte    opcode;
    xuByte    __reserved__[3]; // NOLINT(*-reserved-identifier)
};

struct inst_set_value {
    xuByte opcode;
    xuByte value;
    xuByte    __reserved__[2]; // NOLINT(*-reserved-identifier)

};

struct inst_los_reg {
    xuByte  opcode;
    xuByte  rd;
    xuByte  rs;
    xByte   offset;
};

struct inst_load_imm {
    xuByte  opcode;
    xuByte  rd;
    xShort  imm;
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

// change the src_reg
struct inst_ctx_ch {
    xuByte  opcode; // enter, reset, or exit
    xuByte  sa;
    xShort  offset;
};

struct inst_jump {
    xuByte  opcode;
    enum : xuByte {
        inst_jump_directly,
        inst_jump_if_eq,
        inst_jump_if_lt,
        inst_jump_if_gt,
        inst_jump_if_le,
        inst_jump_if_ge,
        inst_jump_if_success,
        inst_jump_if_failed,
        inst_jump_as_call
    }       arg;
    xShort  offset;
};

struct inst_ret {
    xuByte    opcode;
    xuByte    __reserved__[3]; // NOLINT(*-reserved-identifier)
};

struct inst_arith {
    xuByte  opcode;
    xuByte  rd;
    xuByte  rs1;
    xuByte  rs2;
};

typedef union regexp_inst {
    struct inst_nop         nop;
    struct inst_set_value   set_value;
    struct inst_los_reg     los_reg;
    struct inst_load_imm    load_imm;
    struct inst_match_lit   match_lit;
    struct inst_match_reg   match_reg;
    struct inst_jump        jump;
    struct inst_ret         ret;
    struct inst_ctx_ch      ctx_change;
    struct inst_arith       arith;
} inst;


#endif //XPARSE_INST_H
