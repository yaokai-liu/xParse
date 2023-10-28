//
// Project Name: xParse
// Filename: push.c
// Creator: Yaokai Liu
// Create Date: 2023-10-24
// Copyright (c) 2023 Yaokai Liu. All rights reserved.
//

#include "pseudo.h"

inline xVoid pseudo_push(xuByte reg, mem_space *inst_space) {
    inst instruction;
    instruction.arith_imm = (struct inst_arith_imm) {
            .opcode = inst_sub_i,
            .rd = vm_stack_reg,
            .imm = sizeof(xuLong),
    };
    MemSpace.push(inst_space, &instruction);
    instruction.msl_reg = (struct inst_msl_reg) {
        .opcode = inst_store,
        .rd = vm_stack_reg,
        .rs = reg,
        .offset = sizeof(xuLong),
    };
    MemSpace.push(inst_space, &instruction);
}